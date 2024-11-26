#pragma once
#include <string>
#include <variant>
#include <memory>
#include <vector>
#include <cassert>
#include <easylog.hpp>
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
		ast_expr,
		ast_primary_expr,
		ast_unary_expr,	//l2 expr
		ast_l3expr,
		ast_l4expr,
		ast_expr_end,
		ast_op,
		ast_unary_op,	// l2_op
		ast_l3op,
		ast_l4op,
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


class UnaryExpr;

/**
 * Expr ::= UnaryExpr;
 */
class Expr: public BaseAST
{
public:
	FILL_CLASSOF(ast_expr)

	Expr(std::unique_ptr<UnaryExpr> uptr);
	~Expr();
	
	auto get_unary_expr() const -> const UnaryExpr&
	{ return *m_value; }

private:
	std::unique_ptr<UnaryExpr> m_value;

};


/**
 * PrimaryExpr  ::= "(" Expr ")" | Number | Ident;
 */
class PrimaryExpr: public BaseAST
{
public:
	FILL_CLASSOF(ast_primary_expr);
	using ExprPtr = std::unique_ptr<Expr>;
	using NumberPtr = std::unique_ptr<Number>;
	using IdentPtr = std::unique_ptr<Ident>;
	using Variant = std::variant<ExprPtr, NumberPtr, IdentPtr>;

	PrimaryExpr(ExprPtr expr_ptr):
		BaseAST(ast_primary_expr),
		m_value { std::move(expr_ptr) }
	{}

	PrimaryExpr(NumberPtr number_ptr):
		BaseAST(ast_primary_expr),
		m_value { std::move(number_ptr) }
	{}

	PrimaryExpr(IdentPtr ident_ptr):
		BaseAST(ast_primary_expr),
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


/// 所有操作符的基类
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
	};
	
	Operation(AstKind ast_kind, OperationType type) :
		BaseAST { ast_kind },
		m_type { type }
	{
		assert(ast_kind > ast_op && ast_kind < ast_op_end);
	}

	auto get_type() -> OperationType
	{
		return m_type;
	}

	auto get_type_str() -> const char*
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
		defualt:
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
class UnaryExpr: public BaseAST
{
public:
	using PrmExpPtr = std::unique_ptr<PrimaryExpr>;
	using PackPtr = std::pair<std::unique_ptr<UnaryOp>, std::unique_ptr<UnaryExpr>>;
	using Variant = std::variant<PrmExpPtr, PackPtr>;

	FILL_CLASSOF(ast_unary_expr);

	UnaryExpr(PrmExpPtr primary_expr):
		BaseAST { ast_unary_expr },
		m_value { std::move(primary_expr) }
	{}

	UnaryExpr(std::unique_ptr<UnaryOp> unary_op,
			std::unique_ptr<UnaryExpr> unary_expr):
		BaseAST { ast_unary_expr }, 
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


/// L4Op ::= "+" | "-"
class L4Op: public BaseAST
{
};

/**
 * MulExpr ::= UnaryExpr | MulExpr ("*" | "/" | "%") UnaryExpr;
 */
class ExprL3: public BaseAST
{
public:
	using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
	using Variant = std::variant<>;

private:
	
};


/**
 * AddExpr ::= MulExpr | AddExpr ("+" | "-") MulExpr;
 */
class AddExpr: public BaseAST
{
public:
private:
};


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

} //namespace tinyc

