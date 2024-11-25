#include "ast.hpp"

namespace tinyc
{

// 让unique_ptr的析构函数知道UnaryExpr的定义
Expr::~Expr() {}

Expr::Expr(std::unique_ptr<UnaryExpr> uptr)
	: BaseAST{ast_expr}, m_value{std::move(uptr)}
{
}

}	//namespace tinyc;
