#include <llvm/Support/Casting.h>
#include <print>
#include "general_visitor.hpp"

namespace tinyc
{

GeneralVisitor::GeneralVisitor():
	m_error_log {},
	m_debug_log {}
{
	m_error_log.set_enable_flush(true);
}


auto GeneralVisitor::visit(BaseAST* ast) -> bool
{
	if (ast == nullptr)
	{
		//m_error_log(yq::loc(), "
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
	handle(node.get_type());
	handle(node.get_ident());
	handle(node.get_paramlist());
	handle(node.get_block());
}

void GeneralVisitor::handle(const Type& node)
{
	m_debug_log("Type: {}", node.get_type_str());
}

void GeneralVisitor::handle(const Ident& node)
{
	m_debug_log("Ident: {}", node.get_value());
}

void GeneralVisitor::handle(const ParamList& node)
{
	m_debug_log("ParamList: ");
	for (const auto& param : node)
	{
		assert(param != nullptr);
		handle(*param);
	}
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

void GeneralVisitor::handle(const Param& node)
{
	m_debug_log("Param: ");
	handle(node.get_type());
	handle(node.get_ident());
}

}	//namespace tinyc


