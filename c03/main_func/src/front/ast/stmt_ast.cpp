#include "stmt_ast.hpp"

namespace tinyc
{

/// Stmt
Stmt::Stmt(std::unique_ptr<Location> location, std::unique_ptr<Expr> expr):
	BaseAST {ast_stmt, std::move(location)}, m_expr { std::move(expr) }{}

auto Stmt::get_expr() const -> const Expr&
{ return *m_expr; }


/// Block
Block::Block(std::unique_ptr<Location> location, Vector stmts)
	: BaseAST{ast_block, std::move(location)}, m_stmts{std::move(stmts)}
{
}

auto Block::begin() const -> Vector::const_iterator
{ return m_stmts.cbegin(); }

auto Block::end() const -> Vector::const_iterator
{ return m_stmts.cend(); }
	
auto Block::get_exprs() const -> const Vector&
{ return m_stmts; }

void Block::add_stmt(std::unique_ptr<Stmt> stmt)
{ m_stmts.push_back(std::move(stmt)); }


/// Param
Param::Param(std::unique_ptr<Location> location, std::unique_ptr<Type> type,
	  std::unique_ptr<Ident> id)
	: BaseAST { ast_param, std::move(location)}, m_type{std::move(type)}, m_id{std::move(id)}
{
}

auto Param::get_type() const -> const Type&
{ return *m_type; }

auto Param::get_ident() const -> const Ident&
{ return *m_id; }


/// ParamList
ParamList::ParamList(std::unique_ptr<Location> location, Vector params)
	: BaseAST{ast_paramlist, std::move(location)}, m_params{std::move(params)}
{
}

auto ParamList::begin() const -> Vector::const_iterator
{
	return m_params.cbegin();
}

auto ParamList::end() const -> Vector::const_iterator
{
	return m_params.cend();
}

auto ParamList::get_params() const -> const Vector&
{
	return m_params;
}

void ParamList::add_param(std::unique_ptr<Param> param)
{
	m_params.push_back(std::move(param));
}

/// FuncDef
FuncDef::FuncDef(std::unique_ptr<Location> location, std::unique_ptr<Type> type,
				 std::unique_ptr<Ident> ident,
				 std::unique_ptr<ParamList> paramlist,
				 std::unique_ptr<Block> block)
	:

	  BaseAST{ast_funcdef, std::move(location)}, m_type{std::move(type)},
	  m_ident{std::move(ident)}, m_paramlist{std::move(paramlist)},
	  m_block{std::move(block)}
{
}

auto FuncDef::get_type() const -> const Type&
{
	return *m_type;
}

auto FuncDef::get_ident() const -> const Ident&
{
	return *m_ident;
}

auto FuncDef::get_paramlist() const -> const ParamList&
{
	return *m_paramlist;
}

auto FuncDef::get_block() const -> const Block&
{
	return *m_block;
}

}	//namespace tinyc
