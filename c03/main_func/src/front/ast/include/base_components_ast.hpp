#pragma once
#include "base_ast.hpp"

namespace tinyc
{

/**
 * 存储INT_LITERAL
 * 对应文法 Number ::= INT_LITERAL;
 **/
class Number: public BaseAST
{
public:
	Number(std::unique_ptr<Location> location, int value);

	[[nodiscard]]
	auto get_int_literal() const -> int;
	
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
	Ident(std::unique_ptr<Location> location, std::string value);

	[[nodiscard]]
	auto get_value() const -> std::string;

	TINYC_AST_FILL_CLASSOF(ast_ident)
private:
	std::string m_value;
};


/**
 * Type ::= "int" | "void" ;
 **/
class Type : public BaseAST
{
public:
	TINYC_AST_FILL_CLASSOF(ast_type)
	enum TypeEnum
	{
		ty_void,
		ty_signed_int,
		ty_unsigned_int,
	};
	Type(std::unique_ptr<Location> location, TypeEnum type);

	auto get_type() const -> TypeEnum;
	auto get_type_str() const -> const char*;

private:
	TypeEnum m_type;
};

}	//namespace tinyc
