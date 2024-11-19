#include <llvm/Support/Casting.h>
#include <print>
#include "general_visitor.hpp"

namespace tinyc
{

GeneralVisitor::GeneralVisitor(llvm::LLVMContext& context):
	m_error_log {},
	m_debug_log {},
	m_llvm_context { context },
	m_module { std::make_shared<llvm::Module>("tinyc.expr", m_llvm_context) },
	m_builder { m_module->getContext() },
	m_void_ty { llvm::Type::getVoidTy(m_module->getContext()) },
	m_int32_ty { llvm::Type::getInt32Ty(m_module->getContext()) }
{
	m_error_log.set_enable_flush(true);
}

auto GeneralVisitor::visit(BaseAST* ast) -> bool
{
	if (ast == nullptr)
	{
		m_error_log(yq::loc(), "visit paramater is null");
		return false;
	}

	auto comp_unit_ptr = llvm::dyn_cast<CompUnit>(ast);

	if (comp_unit_ptr == nullptr)
	{
		m_error_log(yq::loc(), "output visitor paramater should be a CompUnit");
		return false;
	}
	
	handle(*comp_unit_ptr);
	return true;
}

void GeneralVisitor::handle(const CompUnit& node)
{
	m_debug_log("CompUnit:");
	handle(node.get_func_def());
}

void GeneralVisitor::handle(const FuncDef& node)
{
	m_debug_log("FuncDef:");
	auto return_type = handle(node.get_type());
	auto func_name = handle(node.get_ident());
	auto param_types = handle(node.get_paramlist());
	handle(node.get_block());

	auto func_type = llvm::FunctionType::get(return_type, param_types, false);
	auto func =
		llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage,
							   func_name, m_module.get());

}

auto GeneralVisitor::handle(const Type& node) -> llvm::Type*
{
	m_debug_log("Type: {}", node.get_type_str());
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
	m_debug_log("Ident: {}", node.get_value());
	return node.get_value();
}

auto GeneralVisitor::handle(const ParamList& node) -> std::vector<llvm::Type*>
{
	m_debug_log("ParamList: ");
	std::vector<llvm::Type*> type_list;
	type_list.reserve(node.get_params().size());

	for (const auto& param : node)
	{
		assert(param != nullptr);
		type_list.push_back(handle(*param));
	}

	return type_list;
}

void GeneralVisitor::handle(const Block& node)
{
	m_debug_log("Block: ");
	for (const auto& stmt : node)
	{
		assert(stmt != nullptr);
		handle(*stmt);
	}
}

void GeneralVisitor::handle(const Stmt& node)
{
	m_debug_log("Stmt:");
	handle(node.get_expr());
}

void GeneralVisitor::handle(const Expr& node)
{
	m_debug_log("Expr:");
	if (node.has_number())
		handle(node.get_number());
	else if (node.has_ident())
		handle(node.get_ident());
}

void GeneralVisitor::handle(const Number& node)
{
	m_debug_log("Number: {}", node.get_int_literal());
}

auto GeneralVisitor::handle(const Param& node) -> llvm::Type*
{
	m_debug_log("Param: ");
	auto type = handle(node.get_type());
	handle(node.get_ident());

	return type;
}

}	//namespace tinyc


