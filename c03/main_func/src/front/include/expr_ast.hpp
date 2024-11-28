#pragma once
#include "base_ast.hpp"
#include "operation_ast.hpp"

namespace tinyc
{

class BaseExpr: public BaseAST
{
public:
	BaseExpr(AstKind ast_kind, const LocationRange& location):
		BaseAST(ast_kind, location)
	{
		assert(ast_kind >= ast_expr && ast_kind < ast_expr_end);
	}

	[[nodiscard]] static
	auto classof(const BaseAST* ast) -> bool
	{
		return ast->get_kind() >= ast_expr &&
			   ast->get_kind() < ast_op_end;
	}
};


class LOrExpr;
using LowExpr = LOrExpr;
/**
 * Expr ::= ExprL4;
 */
class Expr: public BaseExpr
{
public:
	TINYC_AST_FILL_CLASSOF(ast_expr)

	Expr(const LocationRange& location, std::unique_ptr<LowExpr> uptr);
	~Expr();
	
	auto get_low_expr() const -> const LowExpr&
	{ return *m_value; }

private:
	std::unique_ptr<LowExpr> m_value;

};


/**
 * PrimaryExpr  ::= "(" Expr ")" | Number | Ident;
 */
class PrimaryExpr: public BaseExpr
{
public:
	TINYC_AST_FILL_CLASSOF(ast_primary_expr);
	using ExprPtr = std::unique_ptr<Expr>;
	using NumberPtr = std::unique_ptr<Number>;
	using IdentPtr = std::unique_ptr<Ident>;
	using Variant = std::variant<ExprPtr, NumberPtr, IdentPtr>;

	PrimaryExpr(const LocationRange& location, ExprPtr expr_ptr):
		BaseExpr(ast_primary_expr, location),
		m_value { std::move(expr_ptr) }
	{}

	PrimaryExpr(const LocationRange& location, NumberPtr number_ptr):
		BaseExpr(ast_primary_expr, location),
		m_value { std::move(number_ptr) }
	{}

	PrimaryExpr(const LocationRange& location, IdentPtr ident_ptr):
		BaseExpr(ast_primary_expr, location),
		m_value { std::move(ident_ptr) }
	{}

	[[nodiscard]]
	auto has_expr() const -> bool
	{
		return std::holds_alternative<ExprPtr>(m_value);
	}

	[[nodiscard]]
	auto has_number() const -> bool
	{
		 return std::holds_alternative<NumberPtr>(m_value);
	}

	[[nodiscard]]
	auto has_ident() const -> bool
	{
		return std::holds_alternative<IdentPtr>(m_value);
	}

	// 暂时搁置对于visit的实现
	// 需要详细了解std::invoke_result_t中对于lambda表达式和仿函数参数的区别
	// 并且这里的Variant存储类型为unique_ptr, 这个信息需要对用户屏蔽，
	// 让用户传入的回调函数直接处理对应类型
	
	//template <typename Func>
	//auto visit(Func&& func) const -> util::nf_visit_result_t<Func, Variant>
	//{
	//	//return std::visit(util::uptr_deref_func<Func, Variant>::func(
	//	//					  std::forward<Func>(func)),
	//	//				  m_value);
	//}
	
	auto get_expr() const -> const Expr&
	{
		return *std::get<ExprPtr>(m_value);
	}

	auto get_ident() const -> const Ident&
	{
		return *std::get<IdentPtr>(m_value);
	}

	auto get_number() const -> const Number&
	{
		return *std::get<NumberPtr>(m_value);
	}
	
private:
	Variant m_value;
};

/**
 * UnaryExpr ::= PrimaryExpr | UnaryOp UnaryExpr;
 */
class UnaryExpr: public BaseExpr
{
public:
	using PrmExpPtr = std::unique_ptr<PrimaryExpr>;
	using PackPtr = std::pair<std::unique_ptr<UnaryOp>, std::unique_ptr<UnaryExpr>>;
	using Variant = std::variant<PrmExpPtr, PackPtr>;

	TINYC_AST_FILL_CLASSOF(ast_unary_expr);

	UnaryExpr(const LocationRange& location, PrmExpPtr primary_expr):
		BaseExpr { ast_unary_expr, location },
		m_value { std::move(primary_expr) }
	{}

	UnaryExpr(const LocationRange& location, std::unique_ptr<UnaryOp> unary_op,
			std::unique_ptr<UnaryExpr> unary_expr):
		BaseExpr { ast_unary_expr, location }, 
		m_value { PackPtr { std::move(unary_op), std::move(unary_expr) } }
	{}

	[[nodiscard]]
	auto has_primary_expr() const -> bool
	{
		return std::holds_alternative<PrmExpPtr>(m_value);
	}

	[[nodiscard]]
	auto has_unary_expr() const -> bool
	{
		return std::holds_alternative<PackPtr>(m_value);
	}

	[[nodiscard]]
	auto get_primary_expr() const -> const PrimaryExpr&
	{
		assert(has_primary_expr());
		return *std::get<PrmExpPtr>(m_value);
	}

	[[nodiscard]]
	auto get_unary_op() const -> const UnaryOp&
	{
		assert(has_unary_expr());
		return *(std::get<PackPtr>(m_value).first);
	}

	[[nodiscard]]
	auto get_unary_expr() const -> const UnaryExpr&
	{
		assert(has_unary_expr());
		return *(std::get<PackPtr>(m_value).second);
	}

private:
	Variant m_value;
};


template<typename SelfExpr, typename HigherExpr, typename Operation>
class BinaryExpr: public BaseExpr
{
public:
	using SelfExprPtr = std::unique_ptr<SelfExpr>;
	using HigherExprPtr = std::unique_ptr<HigherExpr>;
	using OpPtr = std::unique_ptr<Operation>;

	using CombinedExpr = std::tuple<
		SelfExprPtr,
		OpPtr,
		HigherExprPtr
	>;
	using CombinedExprRef = std::tuple<
		std::reference_wrapper<const SelfExpr>,
		std::reference_wrapper<const Operation>,
		std::reference_wrapper<const HigherExpr>
	>;
	using Variant = std::variant<HigherExprPtr, CombinedExpr>;

	explicit
	BinaryExpr(AstKind kind, const LocationRange& location, HigherExprPtr ptr):
		BaseExpr { kind, location },
		m_value { std::move(ptr) }
	{}

	BinaryExpr(AstKind kind, const LocationRange& location, SelfExprPtr self_ptr,
			   OpPtr op_ptr, HigherExprPtr higher_ptr)
		: BaseExpr{kind, location},
		  m_value{CombinedExpr{std::move(self_ptr), std::move(op_ptr),
							   std::move(higher_ptr)}}
	{}

	~BinaryExpr() = 0;

	[[nodiscard]]
	auto has_higher_expr() const -> bool
	{
		return std::holds_alternative<HigherExprPtr>(m_value);
	}

	[[nodiscard]]
	auto has_combined_expr() const -> bool
	{
		return std::holds_alternative<CombinedExpr>(m_value);
	}

	[[nodiscard]]
	auto get_higher_expr() const -> const HigherExpr&
	{
		return *std::get<HigherExprPtr>(m_value);
	}

	[[nodiscard]]
	auto get_combined_expr() const -> CombinedExprRef
	{
		const auto& combined_ptr = std::get<CombinedExpr>(m_value);

		return CombinedExprRef {
			std::cref(*std::get<0>(combined_ptr)),
			std::cref(*std::get<1>(combined_ptr)),
			std::cref(*std::get<2>(combined_ptr)),
		};
	}

private:
	Variant m_value;
};

#define BINARY_EXPR_FILL_CONSTRUCTORS(expr_kind, expr_name)                    \
	TINYC_AST_FILL_CLASSOF(expr_kind)                                          \
	expr_name(const LocationRange& location, HigherExprPtr ptr)                \
		: BinaryExpr{expr_kind, location, std::move(ptr)}                      \
	{                                                                          \
	}                                                                          \
	expr_name(const LocationRange& location, SelfExprPtr self_ptr,             \
			  OpPtr op_ptr, HigherExprPtr higher_ptr)                          \
		: BinaryExpr{expr_kind, location, std::move(self_ptr),                 \
					 std::move(op_ptr), std::move(higher_ptr)}                 \
	{                                                                          \
	}

/// L3Expr ::= UnaryExpr | L3Expr L3Op UnaryExpr;
class L3Expr: public BinaryExpr<L3Expr, UnaryExpr, L3Op>
{
public:
	BINARY_EXPR_FILL_CONSTRUCTORS(ast_l3expr, L3Expr)
};

/// L4Expr ::= L3Expr | L4Expr L4Op L3Expr;
class L4Expr: public BinaryExpr<L4Expr, L3Expr, L4Op>
{
public:
	BINARY_EXPR_FILL_CONSTRUCTORS(ast_l4expr, L4Expr)
};


/**
 *	L6Expr ::= L4Expr | L6Expr L6Op L4Expr;
 */
class L6Expr: public BinaryExpr<L6Expr, L4Expr, L6Op>
{
public:
	BINARY_EXPR_FILL_CONSTRUCTORS(ast_l6expr, L6Expr)
};


/**
 * L7Expr ::= L6Expr | L7Expr L7Op L6Expr;
 */
class L7Expr: public BinaryExpr<L7Expr, L6Expr, L7Op>
{
public:
	BINARY_EXPR_FILL_CONSTRUCTORS(ast_l7expr, L7Expr);
};

/**
 * LAndExpr	::= L7Expr | LAndExpr LAndOp L7Expr;
 */
class LAndExpr: public BinaryExpr<LAndExpr, L7Expr, LAndOp>
{
public:
	BINARY_EXPR_FILL_CONSTRUCTORS(ast_land_expr, LAndExpr)
};


/**
 * LOrExpr		::= LAndExpr | LOrExpr LOrOp LAndExpr;
 */
class LOrExpr: public BinaryExpr<LOrExpr, LAndExpr, LOrOp>
{
public:
	BINARY_EXPR_FILL_CONSTRUCTORS(ast_lor_expr, LOrExpr)
};

template <typename SelfExpr, typename HigherExpr, typename Operation>
BinaryExpr<SelfExpr, HigherExpr, Operation>::~BinaryExpr()
{
}


}	//namespace tinyc
	
