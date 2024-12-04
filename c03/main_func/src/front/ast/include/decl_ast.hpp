#pragma once
#include "expr_ast.hpp"

namespace tinyc
{

/// ConstInitVal 	::= ConstExpr; 
class ConstInitVal: public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_const_init_val);
	ConstInitVal(std::unique_ptr<Location> location,
				 std::unique_ptr<ConstExpr> const_expr);

	[[nodiscard]]
	auto get_const_expr() const -> const ConstExpr&;

private:
	std::unique_ptr<ConstExpr> m_const_expr;
};


/// ConstDef		::= Ident "=" ConstInitVal;
class ConstDef: public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_const_def);
	ConstDef(std::unique_ptr<Location> location,
			 std::unique_ptr<Ident> ident,
             std::unique_ptr<ConstInitVal> const_int_val);

	[[nodiscard]]
	auto get_ident() const -> const Ident&;
	[[nodiscard]]
	auto get_const_int_val() const -> const ConstInitVal&;

private:
	std::unique_ptr<Ident> m_ident;
	std::unique_ptr<ConstInitVal> m_const_init_val;
};


/// ConstDecl 		::= "const" ScalarType ConstDef ConstDefList ";";
class ConstDecl: public BaseAST
{
public:
	using Vector = std::vector<std::unique_ptr<ConstDef>>;
	TINYC_AST_FILL_CLASSOF(ast_const_decl);
	ConstDecl(std::unique_ptr<Location> location,
			  std::unique_ptr<ScalarType> scalar_type,
			  std::vector<std::unique_ptr<ConstDef>> const_defs = {});

	[[nodiscard]]
	auto get_scalar_type() const -> const ScalarType&;
	[[nodiscard]]
	auto get_const_defs() const -> const Vector&;

	[[nodiscard]]
	auto begin() const -> Vector::const_iterator;
	[[nodiscard]]
	auto end() const -> Vector::const_iterator;

private:
	std::unique_ptr<ScalarType> m_scalar_type;
	std::vector<std::unique_ptr<ConstDef>> m_const_defs;
};


/// ConstDefList 	::= /* empty */ | ConstDefList ',' ConstDef
class ConstDefList: public BaseAST
{
public:
	
private:
	
};


/// Decl			::=  ConstDecl;
class Decl: public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_decl);
	Decl(std::unique_ptr<Location> location,
		 std::unique_ptr<ConstDecl> const_decl);

	[[nodiscard]]
	auto get_const_decl() const -> const ConstDecl&;
private:
	std::unique_ptr<ConstDecl> m_const_decl;
};

}	//namespace tinyc

