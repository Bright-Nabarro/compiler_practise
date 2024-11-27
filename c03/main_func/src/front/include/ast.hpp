#pragma once
#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <cassert>
#include <easylog.hpp>
#include <tuple>
//#include "utility.hpp"

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

	BaseAST(AstKind kind): m_kind { kind }{}

	virtual
	~BaseAST() = default;

	virtual
	void accept(ASTVisitor& visitor)
	{
		visitor.visit(this);
	}

	[[nodiscard]]
	auto get_kind() const -> AstKind
	{ return m_kind; }

	[[nodiscard]]
	auto get_kind_str() const -> const char*;

private:
	AstKind m_kind;
};

#define FILL_CLASSOF(ast_enum)                                                 \
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
	Number(int value) : BaseAST{ast_number}, m_value{value} {}
	
	[[nodiscard]]
	auto get_int_literal() const -> int
	{ return m_value; }
	
	FILL_CLASSOF(ast_number)

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
	Ident(std::string value) : BaseAST{ast_ident}, m_value{std::move(value)}{}
	
	[[nodiscard]]
	auto get_value() const -> std::string
	{ return m_value; }

	FILL_CLASSOF(ast_ident)
private:
	std::string m_value;
};


class BaseExpr: public BaseAST
{
public:
	BaseExpr(AstKind ast_kind):
		BaseAST(ast_kind)
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
	FILL_CLASSOF(ast_expr)

	Expr(std::unique_ptr<LowExpr> uptr);
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
	FILL_CLASSOF(ast_primary_expr);
	using ExprPtr = std::unique_ptr<Expr>;
	using NumberPtr = std::unique_ptr<Number>;
	using IdentPtr = std::unique_ptr<Ident>;
	using Variant = std::variant<ExprPtr, NumberPtr, IdentPtr>;

	PrimaryExpr(ExprPtr expr_ptr):
		BaseExpr(ast_primary_expr),
		m_value { std::move(expr_ptr) }
	{}

	PrimaryExpr(NumberPtr number_ptr):
		BaseExpr(ast_primary_expr),
		m_value { std::move(number_ptr) }
	{}

	PrimaryExpr(IdentPtr ident_ptr):
		BaseExpr(ast_primary_expr),
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
 * @brief 所有操作符的基类
 * @note 在各个派生类的构造函数中，需要检查type是否合法
 */
class Operation: public BaseAST
{
public:
	[[nodiscard]]
	static auto classof(const BaseAST* ast) -> bool
	{
		return ast->get_kind() > ast_op &&
			   ast->get_kind() < ast_op_end;
	}

	enum OperationType
	{
		op_add,
		op_sub,
		op_not,
		op_mul,
		op_div,
		op_mod,
		op_lt,
		op_le,
		op_gt,
		op_ge,
		op_eq,
		op_ne,
		op_land,
		op_lor,
	};
	
	Operation(AstKind ast_kind, OperationType type) :
		BaseAST { ast_kind },
		m_type { type }
	{
		assert(ast_kind > ast_op && ast_kind < ast_op_end);
	}

	auto get_type() const -> OperationType
	{
		return m_type;
	}

	auto get_type_str() const -> const char*
	{
		switch(m_type)
		{
		case op_add:
			return "add";
		case op_sub:
			return "sub";
		case op_not:
			return "not";
		case op_mul:
			return "mul";
		case op_div:
			return "div";
		case op_mod:
			return "mod";
		default:
			return "unkown";
		};
	}

protected:
	OperationType m_type;
};


/**
 * UnaryOp ::= "+" | "-" | "!";
 */
class UnaryOp: public Operation
{
public:
	FILL_CLASSOF(ast_unary_op);

	UnaryOp(OperationType type):
		Operation { ast_unary_op, type}
	{
		if (type < op_add || type > op_not)
			yq::fatal("Invalid UnaryOp");
	}
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

	FILL_CLASSOF(ast_unary_expr);

	UnaryExpr(PrmExpPtr primary_expr):
		BaseExpr { ast_unary_expr },
		m_value { std::move(primary_expr) }
	{}

	UnaryExpr(std::unique_ptr<UnaryOp> unary_op,
			std::unique_ptr<UnaryExpr> unary_expr):
		BaseExpr { ast_unary_expr }, 
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
	BinaryExpr(AstKind kind, HigherExprPtr ptr):
		BaseExpr { kind },
		m_value { std::move(ptr) }
	{}

	BinaryExpr(AstKind kind, SelfExprPtr self_ptr,
			   OpPtr op_ptr, HigherExprPtr higher_ptr)
		: BaseExpr{kind},
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
	FILL_CLASSOF(expr_kind)                                                    \
	expr_name(HigherExprPtr ptr) : BinaryExpr{expr_kind, std::move(ptr)} {}    \
	expr_name(SelfExprPtr self_ptr, OpPtr op_ptr, HigherExprPtr higher_ptr)    \
		: BinaryExpr{expr_kind, std::move(self_ptr), std::move(op_ptr),        \
					 std::move(higher_ptr)}                                    \
	{}

/// L3Op ::= "*" | "/" | "%"
class L3Op: public Operation
{
public:
	FILL_CLASSOF(ast_l3op);

	L3Op(OperationType type):
		Operation(ast_l3op, type)
	{
		if (get_type() < op_mul || get_type() > op_mod)
			yq::fatal("Invalid L3Op");
	}
};



/// L3Expr ::= UnaryExpr | L3Expr L3Op UnaryExpr;
class L3Expr: public BinaryExpr<L3Expr, UnaryExpr, L3Op>
{
public:
	BINARY_EXPR_FILL_CONSTRUCTORS(ast_l3expr, L3Expr)
};


/// L4Op ::= "+" | "-"
class L4Op: public Operation
{
public:
	FILL_CLASSOF(ast_l4op);

	L4Op(OperationType type):
		Operation(ast_l4op, type)
	{
		if (get_type() < op_add || get_type() > op_not)
			yq::fatal("Invalid L4Op");
	}
};



/// L4Expr ::= L3Expr | L4Expr L4Op L3Expr;
class L4Expr: public BinaryExpr<L4Expr, L3Expr, L4Op>
{
public:
	BINARY_EXPR_FILL_CONSTRUCTORS(ast_l4expr, L4Expr)
};


/**
 * L6Op	::= "<" | ">" | "<=" | ">="
 */
class L6Op: public Operation
{
public:
	FILL_CLASSOF(ast_l6op);

	L6Op(OperationType type):
		Operation(ast_l6op, type)
	{
		if (get_type() < op_lt || get_type() > op_ge)
			yq::fatal("Invalid L6Op");
	}
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
 * L7Op	::= "==" | "!="
 */
class L7Op : public Operation
{
public:
	FILL_CLASSOF(ast_l7op)

	L7Op(OperationType type) : Operation(ast_l7op, type)
	{
		if (get_type() < op_eq || get_type() > op_ne)
			yq::fatal("Invalid L7Op");
	}
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
 * LAndOp		::= "&&"
 */
class LAndOp: public Operation
{
public:
	FILL_CLASSOF(ast_land_op);
	LAndOp(OperationType type): Operation { ast_land_op, type }
	{
		if (get_type() != op_land)
			yq::fatal("Invalid LAndOp");
	}
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
 * LOrOp		::= "||"
 */
class LOrOp: public Operation
{
public:
	FILL_CLASSOF(ast_lor_op);
	LOrOp(OperationType type): Operation { ast_lor_op, type }
	{
		if (get_type() != op_lor)
			yq::fatal("Invalid LOrOp");
	}
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

/**
 * Stmt ::= "return" Expr ";";
 */
class Stmt: public BaseAST
{
public:
	Stmt(std::unique_ptr<Expr> expr):
		BaseAST(ast_stmt), m_expr { std::move(expr) }{}

	FILL_CLASSOF(ast_stmt);
	
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
	Block(Vector stmts = Vector{})
		: BaseAST{ast_block}, m_stmts{std::move(stmts)}
	{ }

	FILL_CLASSOF(ast_block);

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

	Type(TypeEnum type): BaseAST { ast_type }, m_type { type }
	{ }

	FILL_CLASSOF(ast_type)

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
	Param(std::unique_ptr<Type> type, std::unique_ptr<Ident> id)
		: BaseAST { ast_param }, m_type{std::move(type)}, m_id{std::move(id)}
	{ }

	FILL_CLASSOF(ast_param);
	
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
	ParamList(Vector params = Vector{}):
		BaseAST(ast_paramlist),
		m_params { std::move(params) }
	{ }

	FILL_CLASSOF(ast_paramlist);

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
		std::unique_ptr<Type> type,
		std::unique_ptr<Ident> ident,
		std::unique_ptr<ParamList> paramlist,
		std::unique_ptr<Block> block):

		BaseAST{ast_funcdef},
		m_type { std::move(type) },
		m_ident { std::move(ident) },
		m_paramlist { std::move(paramlist) },
		m_block { std::move(block) }
	{}

	FILL_CLASSOF(ast_funcdef)
	
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
	CompUnit(std::unique_ptr<FuncDef> func_def):
		BaseAST{ast_comunit},
		m_func_def { std::move(func_def) }
	{}

	FILL_CLASSOF(ast_comunit)

	auto get_func_def() const -> const FuncDef&
	{ return *m_func_def; }

private:
	std::unique_ptr<FuncDef> m_func_def;
};


#undef FILL_CLASSOF
#undef BINARY_EXPR_FILL_CONSTRUCTORS
} //namespace tinyc

