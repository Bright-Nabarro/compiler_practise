#include "ast.hpp"

namespace tinyc
{

// 让unique_ptr的析构函数知道UnaryExpr的定义
Expr::~Expr() {}

Expr::Expr(const LocationRange& location, std::unique_ptr<LowExpr> uptr)
	: BaseExpr{ ast_expr, location }, m_value{std::move(uptr)}
{
}

[[nodiscard]]
auto BaseAST::get_kind_str() const -> const char*
{
	switch(get_kind())
	{
	case ast_number:
		return "ast_number";
	case ast_ident:
		return "ast_ident";
	case ast_expr:
		return "ast_expr";
	case ast_primary_expr:
		return "ast_primary_expr";
	case ast_unary_expr:
		return "ast_unary_expr";
	case ast_l3expr:
		return "ast_l3expr";
	case ast_l4expr:
		return "ast_l4expr";
	case ast_l6expr:
		return "ast_l6expr";
	case ast_l7expr:
		return "ast_l7expr";
	case ast_land_expr:
		return "ast_land_expr";
	case ast_lor_expr:
		return "ast_lor_expr";
	case ast_expr_end:
		return "ast_expr_end";
	case ast_op:
		return "ast_op";
	case ast_unary_op:
		return "ast_unary_op";
	case ast_l3op:
		return "ast_l3op";
	case ast_l4op:
		return "ast_l4op";
	case ast_l6op:
		return "ast_l6op";
	case ast_l7op:
		return "ast_l7op";
	case ast_land_op:
		return "ast_land_op";
	case ast_lor_op:
		return "ast_lor_op";
	case ast_op_end:
		return "ast_op_end";
	case ast_stmt:
		return "ast_stmt";
	case ast_block:
		return "ast_block";
	case ast_type:
		return "ast_type";
	case ast_param:
		return "ast_param";
	case ast_paramlist:
		return "ast_paramlist";
	case ast_funcdef:
		return "ast_funcdef";
	case ast_comunit:
		return "ast_comunit";
	default:
		return "unkown";
	}
}


}	//namespace tinyc;

