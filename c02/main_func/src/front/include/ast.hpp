#pragma once
#include <string_view>
#include <string>
#include <variant>
#include <memory>
#include <vector>

namespace tinyc
{

class BaseAST;

class ASTVisitor
{
public:
	virtual
	void visit(BaseAST*) = 0;
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
	auto get_value() -> std::string
	{ return m_value; }

	FILL_CLASSOF(ast_ident)
private:
	std::string m_value;
};


/**
 * Expr ::= Number | Ident ;
 **/
class Expr: public BaseAST
{
public:
	using NumPtr = std::unique_ptr<Number>;
	using IdPtr = std::unique_ptr<Ident>;
	using Variant =
		std::variant<NumPtr, IdPtr>;

	Expr(NumPtr uptr)
		: BaseAST{ast_expr}, m_value{std::move(uptr)}
	{ }

	Expr(IdPtr uptr)
		: BaseAST{ast_expr}, m_value{std::move(uptr)}
	{ }

	[[nodiscard]]
	auto has_number() const -> bool
	{
		return std::holds_alternative<NumPtr>(m_value);
	}

	[[nodiscard]]
	auto has_ident() const -> bool
	{
		return std::holds_alternative<IdPtr>(m_value);
	}

	[[nodiscard]]
	auto get_number() const -> const Number&
	{
		return *std::get<NumPtr>(m_value);
	}
	
	[[nodiscard]]
	auto get_ident() const -> const Ident&
	{
		return *std::get<IdPtr>(m_value);
	}

	FILL_CLASSOF(ast_expr)

private:
	Variant m_value;
};
	

/**
 * Stmt ::= "return" Expr ";";
 **/
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

	auto cbegin() const -> Vector::const_iterator
	{ return m_stmts.cbegin(); }

	auto cend() const -> Vector::const_iterator
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
		ast_int,
		ast_void
	};

	Type(TypeEnum type): BaseAST { ast_type }, m_type { type }
	{ }

	FILL_CLASSOF(ast_type)

	auto get_type() const -> TypeEnum
	{ return m_type; }

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

