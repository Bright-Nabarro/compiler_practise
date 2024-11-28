#pragma once
#include "base_ast.hpp"
#include <easylog.hpp>

namespace tinyc
{
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
	
	Operation(AstKind ast_kind, const LocationRange& location, OperationType type) :
		BaseAST { ast_kind, location },
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
	TINYC_AST_FILL_CLASSOF(ast_unary_op);

	UnaryOp(const LocationRange& location, OperationType type):
		Operation { ast_unary_op, location, type}
	{
		if (type < op_add || type > op_not)
			yq::fatal("Invalid UnaryOp");
	}
};


/// L3Op ::= "*" | "/" | "%"
class L3Op: public Operation
{
public:
	TINYC_AST_FILL_CLASSOF(ast_l3op);

	L3Op(const LocationRange& location, OperationType type):
		Operation(ast_l3op, location, type)
	{
		if (get_type() < op_mul || get_type() > op_mod)
			yq::fatal("Invalid L3Op");
	}
};


/// L4Op ::= "+" | "-"
class L4Op: public Operation
{
public:
	TINYC_AST_FILL_CLASSOF(ast_l4op);

	L4Op(const LocationRange& location, OperationType type):
		Operation(ast_l4op, location, type)
	{
		if (get_type() < op_add || get_type() > op_not)
			yq::fatal("Invalid L4Op");
	}
};




/**
 * L6Op	::= "<" | ">" | "<=" | ">="
 */
class L6Op: public Operation
{
public:
	TINYC_AST_FILL_CLASSOF(ast_l6op);

	L6Op(const LocationRange& location, OperationType type):
		Operation(ast_l6op, location, type)
	{
		if (get_type() < op_lt || get_type() > op_ge)
			yq::fatal("Invalid L6Op");
	}
};


/**
 * L7Op	::= "==" | "!="
 */
class L7Op : public Operation
{
public:
	TINYC_AST_FILL_CLASSOF(ast_l7op)

	L7Op(const LocationRange& location, OperationType type)
		: Operation{ast_l7op, location, type}
	{
		if (get_type() < op_eq || get_type() > op_ne)
			yq::fatal("Invalid L7Op");
	}
};


/**
 * LAndOp		::= "&&"
 */
class LAndOp: public Operation
{
public:
	TINYC_AST_FILL_CLASSOF(ast_land_op);
	LAndOp(const LocationRange& location, OperationType type)
		: Operation{ast_land_op, location, type}
	{
		if (get_type() != op_land)
			yq::fatal("Invalid LAndOp");
	}
};



/**
 * LOrOp		::= "||"
 */
class LOrOp: public Operation
{
public:
	TINYC_AST_FILL_CLASSOF(ast_lor_op);
	LOrOp(const LocationRange& location, OperationType type)
		: Operation{ast_lor_op, location, type}
	{
		if (get_type() != op_lor)
			yq::fatal("Invalid LOrOp");
	}
};



}	//namespace tinyc
