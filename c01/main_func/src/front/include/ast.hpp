#pragma once
#include <memory>
#include <vector>
#include <variant>
#include <string_view>

namespace ast
{

class BaseAST;
class Number;
class Ident;
class Expr;
class Stmt;
class Block;
class Type;
class Param;
class ParamList;
class CompUnit;

class ASTVisitor
{
public:
	//如果没有定义对应类型，则退化为基类节点
	virtual void visit(BaseAST&) {}
	virtual void visit(Number&) = 0;
	virtual void visit(Ident&) = 0;
	virtual void visit(Expr&) = 0;
	virtual void visit(Stmt&) = 0;
	virtual void visit(Block&) = 0;
	virtual void visit(Type&) = 0;
	virtual void visit(Param&) = 0;
	virtual void visit(ParamList&) = 0;
	virtual void visit(CompUnit&) = 0;
};


class BaseAST
{
public:
	virtual ~BaseAST() = default;
	virtual void accept(ASTVisitor& v) = 0;
};

#define DEFINE_VISIT \
	void accept(ASTVisitor& v) override \
	{ \
		v.visit(*this); \
	}


class Number : public BaseAST
{
public:
	Number(int value) : m_value{value} {}
	auto get_int_literal() const -> int
	{
		return m_value;
	}
	
	DEFINE_VISIT;

private:
	int m_value;
};


class Ident: public BaseAST
{
public:
	Ident(std::string id): m_id { std::move(id) } {}
	auto get_id() const -> std::string_view
	{ return m_id; }

	DEFINE_VISIT;

private:
	std::string m_id;
};


class Expr : public BaseAST
{
public:
	using Variant = std::variant<std::unique_ptr<Number>, std::unique_ptr<Ident>>;

	Expr(std::unique_ptr<Number> number_ast):
		m_value { std::move(number_ast) }
	{ }

	Expr(std::unique_ptr<Ident> ident_ast):
		m_value { std::move(ident_ast) }
	{ }

	auto get_value() const -> const Variant&
	{
		return m_value;
	}

	DEFINE_VISIT;

private:
	Variant m_value;
};


class Stmt : public BaseAST
{
public:
	Stmt(std::unique_ptr<Expr> expr):
		m_expr { std::move(expr) }
	{}

	auto get_expr() const -> const Expr*
	{
		return m_expr.get();
	}

	DEFINE_VISIT;

private:
	std::unique_ptr<Expr> m_expr;
};


class Block : public BaseAST
{
public:
	using Vector = std::vector<std::unique_ptr<Stmt>>;
	Block(Vector exprs): m_exprs { std::move(exprs) }
	{}

	auto cbegin() const -> Vector::const_iterator
	{ return m_exprs.cbegin(); }

	auto cend() const -> Vector::const_iterator
	{ return m_exprs.cend(); }
	
	auto get_exprs() const -> const Vector&
	{ return m_exprs; }

	DEFINE_VISIT;
private:
	Vector m_exprs;
};


class Type : public BaseAST
{
public:
	enum TypeEnum
	{
		ast_int,
		ast_void
	};

	Type(TypeEnum type): m_type { type }
	{}

	auto get_type() const -> TypeEnum
	{ return m_type; }

	DEFINE_VISIT;

private:
	TypeEnum m_type;
};


class Param : public BaseAST
{
public:
	Param(std::unique_ptr<Type> type, std::unique_ptr<Ident> id)
		: m_type{std::move(type)}, m_id{std::move(id)}
	{
	}
	
	auto get_type() const -> const Type*
	{ return m_type.get(); }

	auto get_ident() const -> const Ident*
	{ return m_id.get(); }

	DEFINE_VISIT;

private:
	std::unique_ptr<Type> m_type;
	std::unique_ptr<Ident> m_id;
};


class ParamList: public BaseAST
{
public:
	using Vector = std::vector<std::unique_ptr<Param>>;
	ParamList(Vector params = Vector{}):
		m_params { std::move(params) }
	{}

	auto cbegin() const -> Vector::const_iterator
	{
		return m_params.cbegin();
	}

	auto cend() const -> Vector::const_iterator
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
	
	DEFINE_VISIT;

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
		m_type { std::move(type) },
		m_ident { std::move(ident) },
		m_paramlist { std::move(paramlist) },
		m_block { std::move(block) }
	{}
	
	auto get_type () const -> const Type*
	{ return m_type.get(); }

	auto get_ident () const -> const Ident*
	{ return m_ident.get(); }

	auto get_paramlist () const -> const ParamList*
	{ return m_paramlist.get(); }

	auto get_block () const -> const Block*
	{ return m_block.get(); }

	DEFINE_VISIT;

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
		m_func_def { std::move(func_def) }
	{}

	auto get_func_def() const -> const FuncDef*
	{ return m_func_def.get(); }

	DEFINE_VISIT;
private:
	std::unique_ptr<FuncDef> m_func_def;
};

#undef DEFINE_VISIT

} // namespace ast

