#pragma once
#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <cassert>
#include <easylog.hpp>
#include <tuple>
#include "location_range.hpp"
#include "base_ast.hpp"
//#include "utility.hpp"

namespace tinyc
{





/**
 * Stmt ::= "return" Expr ";";
 */
class Stmt: public BaseAST
{
public:
	Stmt(const LocationRange& location, std::unique_ptr<Expr> expr):
		BaseAST {ast_stmt, location}, m_expr { std::move(expr) }{}

	TINYC_AST_FILL_CLASSOF(ast_stmt);
	
	auto get_expr() const -> const Expr&
	{ return *m_expr; }

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
	Block(const LocationRange& location, Vector stmts = Vector{})
		: BaseAST{ast_block, location}, m_stmts{std::move(stmts)}
	{
	}

	TINYC_AST_FILL_CLASSOF(ast_block);

	auto begin() const -> Vector::const_iterator
	{ return m_stmts.cbegin(); }

	auto end() const -> Vector::const_iterator
	{ return m_stmts.cend(); }
	
	auto get_exprs() const -> const Vector&
	{ return m_stmts; }

	void add_stmt(std::unique_ptr<Stmt> stmt)
	{ m_stmts.push_back(std::move(stmt)); }

private:
	Vector m_stmts;
};


/**
 * Type ::= "int" | "void" ;
 **/
class Type : public BaseAST
{
public:
	enum TypeEnum
	{
		ty_int,
		ty_void
	};

	Type(const LocationRange& location, TypeEnum type)
		: BaseAST{ast_type, location}, m_type{type}
	{
	}

	TINYC_AST_FILL_CLASSOF(ast_type)

	auto get_type() const -> TypeEnum
	{ return m_type; }

	auto get_type_str() const -> const char*
	{
		switch(get_type())
		{
		case ty_int:
			return "int";
		case ty_void:
			return "void";
		default:
			return "unkown";
		}
	}

private:
	TypeEnum m_type;
};


/**
 * Param ::= Type Ident; 
 **/
class Param : public BaseAST
{
public:
	Param(const LocationRange& location, std::unique_ptr<Type> type,
		  std::unique_ptr<Ident> id)
		: BaseAST { ast_param, location}, m_type{std::move(type)}, m_id{std::move(id)}
	{
	}

	TINYC_AST_FILL_CLASSOF(ast_param);
	
	auto get_type() const -> const Type&
	{ return *m_type; }

	auto get_ident() const -> const Ident&
	{ return *m_id; }
	
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
	using Vector = std::vector<std::unique_ptr<Param>>;
	ParamList(const LocationRange& location, Vector params = Vector{}):
		BaseAST { ast_paramlist, location },
		m_params { std::move(params) }
	{ }

	TINYC_AST_FILL_CLASSOF(ast_paramlist);

	auto begin() const -> Vector::const_iterator
	{
		return m_params.cbegin();
	}

	auto end() const -> Vector::const_iterator
	{
		return m_params.cend();
	}

	auto get_params() const -> const Vector&
	{
		return m_params;
	}

	void add_param(std::unique_ptr<Param> param)
	{
		m_params.push_back(std::move(param));
	}
	
private:
	Vector m_params;

};


class FuncDef: public BaseAST
{
public:
	FuncDef(
		const LocationRange& location, 
		std::unique_ptr<Type> type,
		std::unique_ptr<Ident> ident,
		std::unique_ptr<ParamList> paramlist,
		std::unique_ptr<Block> block):

		BaseAST { ast_funcdef, location },
		m_type { std::move(type) },
		m_ident { std::move(ident) },
		m_paramlist { std::move(paramlist) },
		m_block { std::move(block) }
	{}

	TINYC_AST_FILL_CLASSOF(ast_funcdef)
	
	auto get_type () const -> const Type&
	{ return *m_type; }

	auto get_ident () const -> const Ident&
	{ return *m_ident; }

	auto get_paramlist () const -> const ParamList&
	{ return *m_paramlist; }

	auto get_block () const -> const Block&
	{ return *m_block; }

private:
	std::unique_ptr<Type> m_type;
	std::unique_ptr<Ident> m_ident;
	std::unique_ptr<ParamList> m_paramlist;
	std::unique_ptr<Block> m_block;
};


class CompUnit: public BaseAST
{
public:
	CompUnit(const LocationRange& location, std::unique_ptr<FuncDef> func_def):
		BaseAST { ast_comunit, location },
		m_func_def { std::move(func_def) }
	{}

	TINYC_AST_FILL_CLASSOF(ast_comunit)

	auto get_func_def() const -> const FuncDef&
	{ return *m_func_def; }

private:
	std::unique_ptr<FuncDef> m_func_def;
};


#undef BINARY_EXPR_FILL_CONSTRUCTORS

} //namespace tinyc

