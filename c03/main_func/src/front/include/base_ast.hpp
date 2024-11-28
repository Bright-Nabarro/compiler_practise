#pragma once
#include <memory>
#include "location_range.hpp"

namespace tinyc
{

class BaseAST;

class ASTVisitor
{
public:
	virtual
	~ASTVisitor() = default;

	virtual
	auto visit(BaseAST*) -> bool = 0;
};

/// 使用llvm-rtti进行动态转换
class BaseAST
{
public:
	enum AstKind
	{
		ast_number,
		ast_ident,
		// expration
		ast_expr,
		ast_primary_expr,
		ast_unary_expr,	//l2 expr
		ast_l3expr,
		ast_l4expr,
		ast_l6expr,
		ast_l7expr,
		ast_land_expr,
		ast_lor_expr,
		ast_expr_end,
		// operation
		ast_op,
		ast_unary_op,	// l2_op
		ast_l3op,
		ast_l4op,
		ast_l6op,
		ast_l7op,
		ast_land_op,
		ast_lor_op,
		ast_op_end,

		ast_stmt,
		ast_block,
		ast_type,
		ast_param,
		ast_paramlist,
		ast_funcdef,
		ast_comunit,
	};

	BaseAST(AstKind kind, const LocationRange& location);

	virtual
	~BaseAST() = default;

	virtual
	void accept(ASTVisitor& visitor);


	[[nodiscard]]
	auto get_kind() const -> AstKind;

	[[nodiscard]]
	auto get_kind_str() const -> const char*;
	
	void report(const llvm::SourceMgr& src_mgr,
					  llvm::SourceMgr::DiagKind dk,
					  std::string_view msg) const;

private:
	AstKind m_kind;
	std::unique_ptr<LocationRange> m_location;
};

#define TINYC_AST_FILL_CLASSOF(ast_enum)                                       \
	[[nodiscard]]                                                              \
	static auto classof(const BaseAST* ast) -> bool                            \
	{                                                                          \
		return ast->get_kind() == ast_enum;                                    \
	}

/**
 * 存储INT_LITERAL
 * 对应文法 Number ::= INT_LITERAL;
 **/
class Number: public BaseAST
{
public:
	Number(const LocationRange& location, int value) : BaseAST{ast_number, location}, m_value{value} {}
	
	[[nodiscard]]
	auto get_int_literal() const -> int
	{ return m_value; }
	
	TINYC_AST_FILL_CLASSOF(ast_number)

private:
	/// INT_LITERAL
	int m_value;
};


/**
 * 对应文法 Ident ::= [a-zA-Z_][0-9a-zA-Z_]*;
 **/
class Ident: public BaseAST
{
public:
	Ident(const LocationRange& location, std::string value) : BaseAST{ast_ident, location}, m_value{std::move(value)}{}
	
	[[nodiscard]]
	auto get_value() const -> std::string
	{ return m_value; }

	TINYC_AST_FILL_CLASSOF(ast_ident)
private:
	std::string m_value;
};

}	//namespace tinyc
