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

GeneralVisitor::GeneralVisitor(llvm::LLVMContext& context, bool emit_llvm, llvm::SourceMgr& src_mgr,
				   std::string_view output_file, llvm::TargetMachine* tm):
	m_module { std::make_unique<llvm::Module>("tinyc.expr", context) },
	m_builder { m_module->getContext() },
	m_type_mgr { std::make_shared<CTypeManager>(m_module->getContext(), tm) },
	m_emit_llvm { emit_llvm },
	m_src_mgr { src_mgr },
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
	auto func_name = handle(node.get_ident()).second;
	auto param_types = handle(node.get_paramlist());

	auto func_type = llvm::FunctionType::get(return_type, param_types, false);

	//auto func =
	//	llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage,
	//						   func_name, m_module.get());

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
	case tinyc::Type::ty_signed_int:
		ret = m_type_mgr->get_signed_int();
		break;
	case tinyc::Type::ty_unsigned_int:
		ret = m_type_mgr->get_unsigned_int();
		break;
	case tinyc::Type::ty_void:
		ret = m_type_mgr->get_void();
		break;
	default:
		yq::fatal(yq::loc(), "Unkown TypeEnum int tinyc::Type when handling "
				 "tinyc::Type to llvm::Type*");
		ret = nullptr;
		break;
	}
	yq::debug("Type[{}]End", node.get_type_str());
	
	return ret;
}

auto GeneralVisitor::handle(const Ident& node) -> std::pair<llvm::Value*, std::string>
{
	yq::debug("Ident[{}]Begin:", node.get_value());
	
	llvm::Value* value = nullptr;
	std::string name = node.get_value();

	yq::debug("Ident[{}]End:", node.get_value());
	return { value, name };
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
	auto ret = handle(node.get_low_expr());
	yq::debug("ExprEnd");

	return ret;
}

auto GeneralVisitor::handle(const PrimaryExpr& node) -> llvm::Value*
{
	yq::debug("PrimaryExprBegin: ");

	// ast 搁置了对visit的实现
	//auto func = [this](auto& v) {
	//	auto ret = this->handle(v);
	//	static_assert(std::is_same_v<std::decay_t<decltype(ret)>, llvm::Value*>);
	//	return ret;
	//};
	//node.visit(func);
	
	llvm::Value* result = nullptr;
	if (node.has_expr())
	{
		result = handle(node.get_expr());	
	}
	else if (node.has_ident())
	{
		result = handle(node.get_ident()).first;
	}
	else if (node.has_number())
	{
		result = handle(node.get_number());
	}

	yq::debug("PrimaryExprEnd");
	return result;
}

auto GeneralVisitor::handle(const UnaryExpr& node) -> llvm::Value*
{
	yq::debug("UnaryExpr Begin:");

	llvm::Value* result = nullptr;
	if (node.has_unary_expr())
	{
		result = handle(node.get_unary_expr());
		result = unary_operate(node.get_unary_op(), result);
	}
	else if (node.has_primary_expr())
	{
		result = handle(node.get_primary_expr());
	}
	else
	{
		assert(false && "UnaryExpr has an unkown type in its variant");
	}

	yq::debug("UnaryExpr End");
	return result;
}

auto GeneralVisitor::handle(const Number& node) -> llvm::Value*
{
	yq::debug("Number[{}] Begin: ", node.get_int_literal());

	llvm::Value* result = llvm::ConstantInt::get(m_type_mgr->get_signed_int(),
												 node.get_int_literal());

	yq::debug("Number End");
	return result;
}

auto GeneralVisitor::unary_operate(const UnaryOp& op, llvm::Value* operand)
	-> llvm::Value*
{
	yq::debug("UnaryOp[{}] Begin:",op.get_type_str());

	llvm::Type* type = operand->getType();
	llvm::Value* result = nullptr;

	if (!type->isIntegerTy() && !type->isFloatingPointTy())
	{
		yq::error("Expected type in UnaryOp");
		return nullptr;
	}

	switch(op.get_type())
	{
	case UnaryOp::op_add:
		result = operand;
		break;
	case UnaryOp::op_sub:
		if (type->isIntegerTy())
			result = m_builder.CreateNeg(operand);
		else 
			result = m_builder.CreateFNeg(operand);
		break;
	/// c语言not操作将操作数转换为int类型
	case UnaryOp::op_not: {
		llvm::Value* zero = llvm::ConstantInt::get(type, 0);
		llvm::Value* is_nonzero = nullptr;
		if (type->isIntegerTy())
		{
			is_nonzero = m_builder.CreateICmpNE(operand, zero);
		}
		else
		{
			is_nonzero = m_builder.CreateFCmpUNE(operand, zero);
		}
		llvm::Value* int_value = m_builder.CreateZExt(is_nonzero, m_type_mgr->get_signed_int());

		result = m_builder.CreateNot(int_value);
		break;
	}
	default:
		yq::error("Unkown operation: {}, category: {}",
				  static_cast<int>(op.get_type()), op.get_type_str());
		result = nullptr;
	}
	
	yq::debug("UnaryOp End");

	return result;
}

auto GeneralVisitor::handle(const Param& node) -> llvm::Type*
{
	yq::debug("ParamBegin: ");
	auto type = handle(node.get_type());
	handle(node.get_ident());
	yq::debug("ParamEnd");

	return type;
}

template<typename BinaryExpr>
auto GeneralVisitor::handle(const BinaryExpr& node) -> llvm::Value*
{
	yq::debug("{} begin:", node.get_kind_str());

	llvm::Value* result = nullptr;

	if (node.has_higher_expr())
	{
		result = handle(node.get_higher_expr());
	}
	else if (node.has_combined_expr())
	{
		auto [ self_expr_ref, op, higher_expr_ref ] = node.get_combined_expr();
		auto left = handle(self_expr_ref.get());
		auto right = handle(higher_expr_ref.get());
		
		result = binary_operate(left, op, right);
	}
	else
	{
		yq::fatal("{}'s Variant has an unkown type", node.get_kind_str());
	}

	yq::debug("{} end", node.get_kind_str());
	return result;
}


auto GeneralVisitor::binary_operate(llvm::Value* left, const Operation& op,
									llvm::Value* right) -> llvm::Value*
{
	yq::debug("{} [{}] Begin:", op.get_kind_str(), op.get_type_str());

	llvm::Value* result = nullptr;
	
	switch(op.get_type())
	{
	case Operation::op_add:
		result = m_builder.CreateAdd(left, right);
		break;
	case Operation::op_sub:
		result = m_builder.CreateSub(left, right);
		break;
	case Operation::op_mul:
		result = m_builder.CreateMul(left, right);
		break;
	case Operation::op_div:
		result = m_builder.CreateSDiv(left, right);
		break;
	case Operation::op_mod:
		result = m_builder.CreateSRem(left, right);
		break;
	case Operation::op_lt:
		result = m_builder.CreateICmpSLT(left, right);
		break;
	case Operation::op_le:
		result = m_builder.CreateICmpSLE(left, right);
		break;
	case Operation::op_gt:
		result = m_builder.CreateICmpSGT(left, right);
		break;
	case Operation::op_ge:
		result = m_builder.CreateICmpSGE(left, right);
		break;
	case Operation::op_eq:
		result = m_builder.CreateICmpEQ(left, right);
		break;
	case Operation::op_ne:
		result = m_builder.CreateICmpNE(left, right);
		break;
	case Operation::op_land:
		left = m_builder.CreateTrunc(left, llvm::Type::getInt1Ty(m_module->getContext()));
		right = m_builder.CreateTrunc(right, llvm::Type::getInt1Ty(m_module->getContext()));
		result = m_builder.CreateLogicalAnd(left, right);
		break;
	case Operation::op_lor:
		left = m_builder.CreateTrunc(left, llvm::Type::getInt1Ty(m_module->getContext()));
		right = m_builder.CreateTrunc(right, llvm::Type::getInt1Ty(m_module->getContext()));
		result = m_builder.CreateLogicalOr(left, right);
		break;
	default:
		//在二元运算符中
		yq::fatal(yq::loc(), "Unprocessed binary operate");
	}

	assert(result != nullptr);
	m_builder.CreateZExt(result, m_type_mgr->get_signed_int());

	yq::debug("{} [{}] End", op.get_kind_str(), op.get_type_str());

	return result;
}


}	//namespace tinyc


