#pragma once
#include "base_ast.hpp"
#include "expr_ast.hpp"
#include "decl_ast.hpp"
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
 * Param ::= Type Ident; 
 **/
class Param : public BaseAST
{
public:
	Param(std::unique_ptr<Location> location, std::unique_ptr<BuiltinType> type,
		  std::unique_ptr<Ident> id);

	TINYC_AST_FILL_CLASSOF(ast_param);
	
	[[nodiscard]]
	auto get_type() const -> const BuiltinType&;
	[[nodiscard]]
	auto get_ident() const -> const Ident&;
	
private:
	std::unique_ptr<BuiltinType> m_type;
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


///BlockItem		::= Decl | Stmt;
class BlockItem: public BaseAST
{
public:
	using DeclPtr = std::unique_ptr<Decl>;
	using StmtPtr = std::unique_ptr<Stmt>;
	using Variant = std::variant<DeclPtr, StmtPtr>;

	TINYC_AST_FILL_CLASSOF(ast_block_item);

	BlockItem(std::unique_ptr<Location> location, DeclPtr decl);
	BlockItem(std::unique_ptr<Location> location, StmtPtr stmt);

	[[nodiscard]]
	auto has_decl() const -> bool;
	[[nodiscard]]
	auto has_stmt() const -> bool;
	
	[[nodiscard]]
	auto get_decl() const -> const Decl&;
	[[nodiscard]]
	auto get_stmt() const -> const Stmt&;

private:
	Variant m_value;
};


/// BlockItemList 	::= /* empty */ | BlockItem BlockItemList
class BlockItemList : public BaseAST
{
public:
	using Vector = std::vector<std::unique_ptr<BlockItem>>;
	TINYC_AST_FILL_CLASSOF(ast_block_item_list);
	
	BlockItemList(std::unique_ptr<Location> location);
	BlockItemList(std::unique_ptr<Location> location,
				  std::unique_ptr<BlockItem> block_item,
				  std::unique_ptr<BlockItemList> block_item_list);

	auto begin() const -> Vector::const_iterator;
	auto end() const -> Vector::const_iterator;
	
	auto get_vector() -> Vector&;

private:
	Vector m_block_items;
};


/// Block       	::= "{" BlockItemList "}";
class Block: public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_block);
	Block(std::unique_ptr<Location> location,
		  std::unique_ptr<BlockItemList> block_item_list);

	auto get_block_item_list() const -> const BlockItemList&;

private:
	std::unique_ptr<BlockItemList> m_block_item_list;
};


class FuncDef: public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_funcdef)

	FuncDef(
		std::unique_ptr<Location> location, 
		std::unique_ptr<BuiltinType> type,
		std::unique_ptr<Ident> ident,
		std::unique_ptr<ParamList> paramlist,
		std::unique_ptr<Block> block);

	[[nodiscard]]
	auto get_type () const -> const BuiltinType&;
	[[nodiscard]]
	auto get_ident () const -> const Ident&;
	[[nodiscard]]
	auto get_paramlist () const -> const ParamList&;
	[[nodiscard]]
	auto get_block () const -> const Block&;

private:
	std::unique_ptr<BuiltinType> m_type;
	std::unique_ptr<Ident> m_ident;
	std::unique_ptr<ParamList> m_paramlist;
	std::unique_ptr<Block> m_block;
};

} // namespace tinyc

