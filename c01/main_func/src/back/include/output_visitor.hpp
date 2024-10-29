#include <print>
#include <iostream>
#include "ast.hpp"

class OutputVisitor: public ast::ASTVisitor
{
public:
	void visit(const ast::Number& ast) override
	{
		std::println("Number: <{}>", ast.get_int_literal());
	}

	void visit(const ast::Ident& ast) override
	{
		std::println("Ident: <{}>", ast.get_id());
	}

	void visit(const ast::Expr& ast) override
	{
		std::println("Expr:");
		auto& value = ast.get_value();
		std::visit([](const auto& value) {
			std::cout<<'<'<<value<<'>'<<" ";
		}, value);
	}

	void visit(const ast::Stmt& ast) override
	{
		std::println("Stmt:");
		visit(*ast.get_expr());
	}

	void visit(const ast::Block& ast) override
	{
		//目前默认只有一个Expr
		std::println("Block: ");
		visit(*(ast.cbegin()->get()));
	}

	void visit(const ast::Type& ast) override
	{
		std::string type_str;
		switch(ast.get_type())
		{
		case ast::Type::TypeEnum::ast_int:
			type_str = "int";
			break;
		case ast::Type::TypeEnum::ast_void:
			type_str = "void";
			break;
		default:
			std::println(stderr, "except Type in Type AST");
		}
		std::println("Type: <{}>", type_str);
	}

	void visit(const ast::Param& ast) override
	{
		std::println("Param: ");
		visit(*ast.get_type());
		visit(*ast.get_ident());
	}

	void visit(const ast::ParamList& ast) override
	{
		(void)ast;
	}

	void visit(const ast::FuncDef& ast) override
	{
		std::println("FuncDef:");
		visit(*ast.get_type());
		visit(*ast.get_ident());
		visit(*ast.get_paramlist());
		visit(*ast.get_block());
	}

	void visit(const ast::CompUnit& ast) override
	{
		std::println("CompUnit:");
		visit(*ast.get_func_def());
	}

};

