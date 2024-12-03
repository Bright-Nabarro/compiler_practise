#pragma once
#include <memory>
#include <expected>

namespace tinyc
{

class BaseAST;

class ASTVisitor
{
public:
	virtual
	~ASTVisitor() = default;

	virtual
	auto visit(BaseAST*) -> std::expected<void, std::string> = 0;
};


/// @brief BaseAST依赖的位置抽象基类
class Location
{
public:
	enum DiagKind
	{
		dk_error,
		dk_warning,
		dk_remark,
		dk_note,
	};

	virtual
	~Location() = default;
	
	virtual
	void report(Location::DiagKind kind, std::string_view msg) const = 0;

private:
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

	BaseAST(AstKind kind, std::unique_ptr<Location> location);

	virtual
	~BaseAST() = default;
	virtual
	void accept(ASTVisitor& visitor);

	[[nodiscard]]
	auto get_kind() const -> AstKind;
	[[nodiscard]]
	auto get_kind_str() const -> const char*;
	
	void report(Location::DiagKind kind, std::string_view msg) const;

private:
	AstKind m_kind;
	std::unique_ptr<Location> m_location;
};

#define TINYC_AST_FILL_CLASSOF(ast_enum)                                       \
	[[nodiscard]]                                                              \
	static auto classof(const BaseAST* ast) -> bool                            \
	{                                                                          \
		return ast->get_kind() == ast_enum;                                    \
	}


}	//namespace tinyc
