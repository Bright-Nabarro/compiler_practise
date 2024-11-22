#include "general_visitor.hpp"
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/CodeGen/CommandFlags.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Pass.h>

#include <print>

namespace tinyc
{

GeneralVisitor::GeneralVisitor(llvm::LLVMContext& context, bool emit_llvm,
				   std::string_view output_file, llvm::TargetMachine* tm):
	m_module { std::make_shared<llvm::Module>("tinyc.expr", context) },
	m_builder { m_module->getContext() },
	m_void_ty { llvm::Type::getVoidTy(m_module->getContext()) },
	m_int32_ty { llvm::Type::getInt32Ty(m_module->getContext()) },
	m_emit_llvm { emit_llvm },
	m_output_file { output_file },
	m_target_machine { tm }
{
}

auto GeneralVisitor::visit(BaseAST* ast) -> bool
{
	if (ast == nullptr)
	{
		yq::error(yq::loc(), "visit paramater is null");
		return false;
	}

	auto comp_unit_ptr = llvm::dyn_cast<CompUnit>(ast);

	if (comp_unit_ptr == nullptr)
	{
		yq::error(yq::loc(), "output visitor paramater should be a CompUnit");
		return false;
	}
	
	handle(*comp_unit_ptr);

	return true;
}

auto GeneralVisitor::emit() -> bool
{
	std::error_code ec;
	
	std::string output_file_name { m_output_file };

	auto file_type = llvm::codegen::getFileType();
	switch(file_type)
	{
	case llvm::CodeGenFileType::AssemblyFile:
		output_file_name.append(".s");
		break;
	case llvm::CodeGenFileType::ObjectFile:
		output_file_name.append(".o");
		break;
	case llvm::CodeGenFileType::Null:
		output_file_name.append(".null");
		break;
	}

	auto open_flags = llvm::sys::fs::OF_None;
	
	llvm::raw_fd_ostream os{output_file_name, ec, open_flags };
	if (ec)
	{
		yq::error("Could not open file {}: {}", m_output_file.data(), ec.message());
		return false;
	}

	llvm::legacy::PassManager pm;

	//输出llvm ir文件
	if (file_type == llvm::CodeGenFileType::AssemblyFile && m_emit_llvm)
	{
		pm.add(createPrintModulePass(os));
		//m_module->print(os, nullptr);
	}
	else
	{
		if (m_target_machine->addPassesToEmitFile(pm, os, nullptr, file_type))
		{
			yq::error(yq::loc(), "No support for file type");
			return false;
		}
		//llvm::WriteBitcodeToFile(*m_module, os);
	}
	pm.run(*m_module);

	return true;
}

void GeneralVisitor::handle(const CompUnit& node)
{
	yq::debug("CompUnit:");
	handle(node.get_func_def());
}

void GeneralVisitor::handle(const FuncDef& node)
{
	yq::debug("FuncDef:");
	auto return_type = handle(node.get_type());
	auto func_name = handle(node.get_ident());
	auto param_types = handle(node.get_paramlist());

	auto func_type = llvm::FunctionType::get(return_type, param_types, false);

	auto func =
		llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage,
							   func_name, m_module.get());
	handle(node.get_block(), func, "entry");

	
}

auto GeneralVisitor::handle(const Type& node) -> llvm::Type*
{
	yq::debug("Type: {}", node.get_type_str());
	switch(node.get_type())
	{
	case tinyc::Type::ty_int:
		return m_int32_ty;
	case tinyc::Type::ty_void:
		return m_void_ty;
	default:
		yq::fatal(yq::loc(), "Unkown TypeEnum int tinyc::Type when handling "
				 "tinyc::Type to llvm::Type*");
		return nullptr;
	}
}

auto GeneralVisitor::handle(const Ident& node) -> std::string
{
	yq::debug("Ident: {}", node.get_value());
	return node.get_value();
}

auto GeneralVisitor::handle(const ParamList& node) -> std::vector<llvm::Type*>
{
	yq::debug("ParamList: ");
	std::vector<llvm::Type*> type_list;
	type_list.reserve(node.get_params().size());

	for (const auto& param : node)
	{
		assert(param != nullptr);
		type_list.push_back(handle(*param));
	}

	return type_list;
}

auto GeneralVisitor::handle(const Block& node, llvm::Function* func,
							std::string_view block_name) -> llvm::BasicBlock*
{
	yq::debug("Block: ");

	auto basic_block =
		llvm::BasicBlock::Create(m_module->getContext(), block_name.data(), func);
	m_builder.SetInsertPoint(basic_block);

	for (const auto& stmt : node)
	{
		assert(stmt != nullptr);
		handle(*stmt);
	}

	return basic_block;
}

void GeneralVisitor::handle(const Stmt& node)
{
	yq::debug("Stmt:");
	auto value = handle(node.get_expr());
	assert(value != nullptr);
	
	m_builder.CreateRet(value);
}

auto GeneralVisitor::handle(const Expr& node) -> llvm::Value*
{
	yq::debug("Expr:");
	if (node.has_number())
	{
		auto const_int = handle(node.get_number());
		return llvm::ConstantInt::get(m_module->getContext(), llvm::APInt(32, const_int));
	}
	else if (node.has_ident())
	{
		handle(node.get_ident());
	}

	return nullptr;
}

auto GeneralVisitor::handle(const Number& node) -> int
{
	yq::debug("Number: {}", node.get_int_literal());
	return node.get_int_literal();
}

auto GeneralVisitor::handle(const Param& node) -> llvm::Type*
{
	yq::debug("Param: ");
	auto type = handle(node.get_type());
	handle(node.get_ident());

	return type;
}

}	//namespace tinyc


