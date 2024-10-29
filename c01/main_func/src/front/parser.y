%code requires {
#include <memory>
#include <string>
}

%{

#include <iostream>
#include <memory>
#include <string>

int yylex();
void yyerror(std::unique_ptr<ast::BaseAST>& ast, const char* s);

template<typename Drived>
auto unique_ptr_dynamic_cast(ast::BaseAST* base_ptr)
-> std::unique_ptr<Drived>
{
	if (auto derived_ptr = dynamic_cast<Drived*>(base_ptr);
		derived_ptr != nullptr)
	{
		return std::unique_ptr<Drived> { derived_ptr };
	}

	throw std::runtime_error { "except dynamic_cast" };
}

%}

%parse-param { std::unique_ptr<ast::BaseAST>& ast }

%union {
	std::string* str_val;
	int int_val;
	ast::BaseAST* ast_val;
}

%token KW_INT KW_VOID KW_RETURN
%token <str_val> IDENT
%token <int_val> INT_LITERAL

%type <ast_val> FuncDef ParamList Param Type Block Stmt Expr Number Ident

%%

CompUnit
	: FuncDef {
		auto func_def_uptr = unique_ptr_dynamic_cast<ast::FuncDef>($1);
		ast = std::make_unique<ast::CompUnit>(std::move(func_def_uptr));
	};

FuncDef
	: Type Ident '(' ParamList ')' Block  {
	/* 1 	2	  3		4		5    6 */	
		auto type_uptr = unique_ptr_dynamic_cast<ast::Type>($1);
		auto ident_uptr =unique_ptr_dynamic_cast<ast::Ident>($2);
		auto param_list_uptr =
			unique_ptr_dynamic_cast<ast::ParamList>($4);
		auto block_uptr = unique_ptr_dynamic_cast<ast::Block>($6);

		$$ = new ast::FuncDef {
			std::move(type_uptr),
			std::move(ident_uptr),
			std::move(param_list_uptr),
			std::move(block_uptr)
		}
	};

ParamList   
	: /* empty */ {
		$$ = new ast::ParamList{};
	}
	| Param {
		ast::ParamList::Vector vec;
		auto param_uptr = unique_ptr_dynamic_cast<ast::Param>($1);
		vec.push_back(std::move(param_uptr));
		$$ = new ast::ParamList { std::move(vec) };
	}
	| ParamList ',' Param {
		auto param_list_ptr = $1;
		auto param_uptr = unique_ptr_dynamic_cast<ast::Param>($3);
		param_list_ptr->add_param(std::move(param_uptr));
		$$ = param_list_ptr;
	};

Param
	:Type Ident {
		auto type_uptr = unique_ptr_dynamic_cast<ast::Type>($1);
		auto ident_uptr = unique_ptr_dynamic_cast<ast::Ident>($2);
		$$ = new ast::Param { std::move(type_uptr), std::move(ident_uptr) };
	}; 

Type        
	: KW_INT {
		$$ = new ast::Type { ast::Type::ast_int };
	}
	| KW_VOID {
		$$ = new ast::Type { ast::Type::ast_void };
	};

Block
	: '{' Stmt '}'{
		//这里暂时之匹配单个表达式
		auto stmt_uptr = std::unique_ptr_dynamic_cast<ast::Stmt>($2);
		ast::Block::Vector vec;
		vec.push_back(std::move(stmt_uptr));
		$$ = new ast::Block { std::move(vec) };
	};

Stmt
	: KW_RETURN Expr ';' {
		auto expr_uptr = std::unique_ptr_dynamic_cast<ast::Expr>($2);
		$$ = new ast::Stmt { std::move(expr_uptr) };
	};
Expr
	: Number {
		auto number_uptr = std::unique_ptr_dynamic_cast<ast::Number>($1);
		$$ = new ast::Expr { std::move(number_uptr) };
	}
	| Ident {
		auto ident_uptr = std::unique_ptr_dynamic_cast<ast::Ident>($1);
		$$ = new ast::Expr { std::move(number_uptr) };

	};
Number
	: INT_LITERAL{
		$$ = new ast::Number { $1 };
	};

Ident
	: IDENT{
		$$ = new ast::Ident { $1 };
	};

%%

void yyerror(std::unique_ptr<ast::BaseAST>& ast, const char* s)
{
	std::cerr<<s<<std::endl;
}




