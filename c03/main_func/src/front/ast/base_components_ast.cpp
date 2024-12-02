#include "base_components_ast.hpp"

namespace tinyc
{

/// Number
Number::Number(std::unique_ptr<Location> location, int value)
	: BaseAST{ast_number, std::move(location)}, m_value{value}
{
}

auto Number::get_int_literal() const -> int
{ return m_value; }


/// Ident
Ident::Ident(std::unique_ptr<Location> location, std::string value)
	: BaseAST{ast_ident, std::move(location)}, m_value{std::move(value)}
{
}

auto Ident::get_value() const -> std::string
{ return m_value; }


/// Type
Type::Type(std::unique_ptr<Location> location, TypeEnum type)
	: BaseAST{ast_type, std::move(location)}, m_type{type}
{
}


auto Type::get_type() const -> TypeEnum
{ return m_type; }

auto Type::get_type_str() const -> const char*
{
	signed int n;
	switch(get_type())
	{
	case ty_void:
		return "void";
	case ty_signed_int:
		return "signed_int";
	case ty_unsigned_int:
		return "unsigned_int";
	default:
		return "unkown";
	}
}

}	//namespace tinyc
