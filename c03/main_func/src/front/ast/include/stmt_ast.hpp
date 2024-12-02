#pragma once
#include "base_ast.hpp"
#include "expr_ast.hpp"
#include "base_components_ast.hpp"

namespace tinyc
{

/**
 * Stmt ::= "return" Expr ";";
 */
class Stmt: public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_stmt);
	Stmt(std::unique_ptr<Location> location, std::unique_ptr<Expr> expr);
	
	[[nodiscard]]
	auto get_expr() const -> const Expr&;

private:
	std::unique_ptr<Expr> m_expr;
};


/**
 * Block ::= "{" Stmt* "}";
 **/
class Block : public BaseAST
{
public:
	using Vector = std::vector<std::unique_ptr<Stmt>>;
	Block(std::unique_ptr<Location> location, Vector stmts = Vector{});

	TINYC_AST_FILL_CLASSOF(ast_block);

	[[nodiscard]]
	auto begin() const -> Vector::const_iterator;
	[[nodiscard]]
	auto end() const -> Vector::const_iterator;
	[[nodiscard]]
	auto get_exprs() const -> const Vector&;
	void add_stmt(std::unique_ptr<Stmt> stmt);

private:
	Vector m_stmts;
};


/**
 * Param ::= Type Ident; 
 **/
class Param : public BaseAST
{
public:
	Param(std::unique_ptr<Location> location, std::unique_ptr<Type> type,
		  std::unique_ptr<Ident> id);

	TINYC_AST_FILL_CLASSOF(ast_param);
	
	[[nodiscard]]
	auto get_type() const -> const Type&;
	[[nodiscard]]
	auto get_ident() const -> const Ident&;
	
private:
	std::unique_ptr<Type> m_type;
	std::unique_ptr<Ident> m_id;
};


/**
 * ParamList   ::= empty | Param ("," Param)* ;
 **/
class ParamList: public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_paramlist);
	using Vector = std::vector<std::unique_ptr<Param>>;

	ParamList(std::unique_ptr<Location> location, Vector params = Vector{});

	[[nodiscard]]
	auto begin() const -> Vector::const_iterator;
	[[nodiscard]]
	auto end() const -> Vector::const_iterator;
	[[nodiscard]]
	auto get_params() const -> const Vector&;
	void add_param(std::unique_ptr<Param> param);
	
private:
	Vector m_params;

};


class FuncDef: public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_funcdef)

	FuncDef(
		std::unique_ptr<Location> location, 
		std::unique_ptr<Type> type,
		std::unique_ptr<Ident> ident,
		std::unique_ptr<ParamList> paramlist,
		std::unique_ptr<Block> block);

	[[nodiscard]]
	auto get_type () const -> const Type&;
	[[nodiscard]]
	auto get_ident () const -> const Ident&;
	[[nodiscard]]
	auto get_paramlist () const -> const ParamList&;
	[[nodiscard]]
	auto get_block () const -> const Block&;

private:
	std::unique_ptr<Type> m_type;
	std::unique_ptr<Ident> m_ident;
	std::unique_ptr<ParamList> m_paramlist;
	std::unique_ptr<Block> m_block;
};

} // namespace tinyc

