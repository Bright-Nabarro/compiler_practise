#include <llvm/Support/Casting.h>
#include <print>
#include "output_visitor.hpp"

namespace tinyc
{

OutputVisitor::OutputVisitor(bool enable_display):
	m_error_log {}
{
	m_error_log.set_enable_flush(true);
}


auto OutputVisitor::visit(BaseAST* ast) -> bool
{
	auto comp_unit_ptr = llvm::dyn_cast<CompUnit>(ast);

	if (comp_unit_ptr == nullptr)
	{
		m_error_log(yq::loc(), "output visitor paramater should be a CompUnit");
		return false;
	}
	
	handle(*comp_unit_ptr);
	return true;
}

void OutputVisitor::handle(const CompUnit& node)
{
	m_debug_log("CompUnit:");
	handle(node.get_func_def());
}

void OutputVisitor::handle(const FuncDef& node)
{
	m_debug_log("FuncDef:");
	handle(node.get_type());
	handle(node.get_ident());
	handle(node.get_paramlist());
	handle(node.get_block());
}

}	//namespace tinyc


