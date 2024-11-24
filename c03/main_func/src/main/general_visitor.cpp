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
	yq::debug("CompUnitBegin:");
	handle(node.get_func_def());
	yq::debug("CompUnitEnd");
}

void GeneralVisitor::handle(const FuncDef& node)
{
	yq::debug("FuncDefBegin:");
	auto return_type = handle(node.get_type());
	auto func_name = handle(node.get_ident());
	auto param_types = handle(node.get_paramlist());

	auto func_type = llvm::FunctionType::get(return_type, param_types, false);

	auto func =
		llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage,
							   func_name, m_module.get());
	handle(node.get_block(), func, "entry");

	yq::debug("FuncDefEnd");
}

auto GeneralVisitor::handle(const Type& node) -> llvm::Type*
{
	yq::debug("Type[{}]Begin: ", node.get_type_str());
	llvm::Type* ret;
	switch(node.get_type())
	{
	case tinyc::Type::ty_int:
		ret = m_int32_ty;
	case tinyc::Type::ty_void:
		ret = m_void_ty;
	default:
		yq::fatal(yq::loc(), "Unkown TypeEnum int tinyc::Type when handling "
				 "tinyc::Type to llvm::Type*");
		ret = nullptr;
	}
	yq::debug("Type[{}]End", node.get_type_str());
	
	return ret;
}

auto GeneralVisitor::handle(const Ident& node) -> llvm::Value*
{
	yq::debug("Ident[{}]Begin:", node.get_value());
	auto ret = nullptr;
	yq::debug("Ident[{}]End:", node.get_value());

	return ret;
}

auto GeneralVisitor::handle(const ParamList& node) -> std::vector<llvm::Type*>
{
	yq::debug("ParamListBegin: ");
	std::vector<llvm::Type*> type_list;
	type_list.reserve(node.get_params().size());

	for (const auto& param : node)
	{
		assert(param != nullptr);
		type_list.push_back(handle(*param));
	}

	yq::debug("ParamListEnd");

	return type_list;
}

auto GeneralVisitor::handle(const Block& node, llvm::Function* func,
							std::string_view block_name) -> llvm::BasicBlock*
{
	yq::debug("BlockBegin: ");

	auto basic_block =
		llvm::BasicBlock::Create(m_module->getContext(), block_name.data(), func);
	m_builder.SetInsertPoint(basic_block);

	for (const auto& stmt : node)
	{
		assert(stmt != nullptr);
		handle(*stmt);
	}
	yq::debug("BlockEnd");

	return basic_block;
}

void GeneralVisitor::handle(const Stmt& node)
{
	yq::debug("StmtBegin:");
	auto value = handle(node.get_expr());
	assert(value != nullptr);
	
	m_builder.CreateRet(value);
	yq::debug("StmtEnd");
}

auto GeneralVisitor::handle(const Expr& node) -> llvm::Value*
{
	yq::debug("ExprBegin:");
	auto ret = handle(node.get_unary_expr());
	yq::debug("ExprEnd");

	return ret;
}

auto GeneralVisitor::handle(const PrimaryExpr& node) -> llvm::Value*
{
	yq::debug("PrimaryExprBegin: ");

	auto func = [this](auto& v) {
		auto ret = this->handle(v);
		static_assert(std::is_same_v<std::decay_t<decltype(ret)>, llvm::Value*>);
		return ret;
	};

	node.visit(func);

	yq::debug("PrimaryExprEnd");
}

auto GeneralVisitor::handle(const UnaryExpr& node) -> llvm::Value*
{
}

auto GeneralVisitor::handle(const Number& node) -> llvm::Value*
{
	yq::debug("Number[{}] Begin: ", node.get_int_literal());
	yq::debug("Number End");

	//return node.get_int_literal();
}

auto GeneralVisitor::handle(const Param& node) -> llvm::Type*
{
	yq::debug("ParamBegin: ");
	auto type = handle(node.get_type());
	handle(node.get_ident());
	yq::debug("ParamEnd");

	return type;
}

}	//namespace tinyc


