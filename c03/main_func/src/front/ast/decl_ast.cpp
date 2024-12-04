#include "decl_ast.hpp"

namespace tinyc
{

/// ConstInitVal Implementation
ConstInitVal::ConstInitVal(std::unique_ptr<Location> location,
						   std::unique_ptr<ConstExpr> const_expr)
	: BaseAST{ast_const_init_val, std::move(location)},
	  m_const_expr{std::move(const_expr)}
{
}

auto ConstInitVal::get_const_expr() const -> const ConstExpr&
{
	return *m_const_expr;
}

/// ConstDef Implementation
ConstDef::ConstDef(std::unique_ptr<Location> location,
				   std::unique_ptr<Ident> ident,
				   std::unique_ptr<ConstInitVal> const_init_val)
	: BaseAST{ast_const_def, std::move(location)}, m_ident{std::move(ident)},
	  m_const_init_val{std::move(const_init_val)}
{
}

auto ConstDef::get_ident() const -> const Ident&
{
	return *m_ident;
}

auto ConstDef::get_const_int_val() const -> const ConstInitVal&
{
	return *m_const_init_val;
}

/// ConstDecl Implementation
ConstDecl::ConstDecl(std::unique_ptr<Location> location,
					 std::unique_ptr<ScalarType> scalar_type,
					 std::vector<std::unique_ptr<ConstDef>> const_defs)
	: BaseAST{ast_const_decl, std::move(location)},
	  m_scalar_type{std::move(scalar_type)}, m_const_defs{std::move(const_defs)}
{
}

auto ConstDecl::get_scalar_type() const -> const ScalarType&
{
	return *m_scalar_type;
}

auto ConstDecl::get_const_defs() const -> const Vector&
{
	return m_const_defs;
}

auto ConstDecl::begin() const -> Vector::const_iterator
{
	return m_const_defs.cbegin();
}

auto ConstDecl::end() const -> Vector::const_iterator
{
	return m_const_defs.cend();
}

/// Decl Implementation
Decl::Decl(std::unique_ptr<Location> location,
		   std::unique_ptr<ConstDecl> const_decl)
	: BaseAST{ast_decl, std::move(location)},
	  m_const_decl{std::move(const_decl)}
{
}

auto Decl::get_const_decl() const -> const ConstDecl&
{
	return *m_const_decl;
}

}	//namespace tinyc

