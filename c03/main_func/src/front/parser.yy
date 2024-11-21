%skeleton "lalr1.cc"	// use LALR-1, C++ language
%require "3.8.2"
%header
%define api.token.constructor 	// 将symbol作为一个类型
%define api.value.type variant 	// 使用variant代替union

%code requires {
#include <memory>
#include <string>
#include <string_view>
#include <iostream>
#include "ast.hpp"
//前向声明
namespace tinyc { class Driver; }
}

%param { tinyc::Driver& driver }

%locations	//生成location定位
%define api.filename.type { std::string_view }
%define parse.trace
%define parse.error detailed
%define parse.lac full	//启用lac前瞻

%code {
#include <memory>
#include <cassert>
#include <string>
#include <llvm/Support/Casting.h>
#include "driver.hpp"
#define assert_same_ptr(Type, ptr) \
	static_assert(std::is_same_v<Type, typename std::decay_t<decltype(ptr)>::element_type>)

}

%token KW_INT KW_VOID KW_RETURN
%token <std::string> IDENT
%token <int> INT_LITERAL
// 字面量标识分隔符
%token DELIM_LPAREN "("
%token DELIM_RPAREN ")"
%token DELIM_LBRACE "{"
%token DELIM_RBRACE "}"
%token DELIM_COMMA ","
%token DELIM_SEMICOLON ";"

%nterm <std::unique_ptr<tinyc::Number>> Number
%nterm <std::unique_ptr<tinyc::Ident>> Ident
%nterm <std::unique_ptr<tinyc::Expr>> Expr
%nterm <std::unique_ptr<tinyc::Stmt>> Stmt
%nterm <std::unique_ptr<tinyc::Block>> Block
%nterm <std::unique_ptr<tinyc::Type>> Type
%nterm <std::unique_ptr<tinyc::Param>> Param
%nterm <std::unique_ptr<tinyc::ParamList>> ParamList
%nterm <std::unique_ptr<tinyc::FuncDef>> FuncDef
%nterm <std::unique_ptr<tinyc::CompUnit>> CompUnit


%%

%start CompUnit;

CompUnit:
	FuncDef 
	{
		//llvm::isa足够智能，能够区分裸指针和智能指针的情况
		assert_same_ptr(tinyc::FuncDef, $1);
		auto comp_unit_ptr = std::make_unique<tinyc::CompUnit>(std::move($1));
		driver.set_ast(std::move(comp_unit_ptr));
	};

FuncDef :
//   1	   2 	3      4   	  5   6
	Type Ident "(" ParamList ")" Block
	{
		assert_same_ptr(tinyc::Type,$1);
		assert_same_ptr(tinyc::Ident, $2);
		assert_same_ptr(tinyc::ParamList, $4);
		assert_same_ptr(tinyc::Block, $6);

		auto funcdef_ptr = std::make_unique<tinyc::FuncDef>(
			std::move($1), std::move($2), std::move($4), std::move($6)
		);

		$$ = std::move(funcdef_ptr);
	};

ParamList :
	/* empty */
	{
		$$ = std::make_unique<tinyc::ParamList>();
	}
	| Param
	{
		assert_same_ptr(tinyc::Param, $1);
		auto param_list_ptr = std::make_unique<tinyc::ParamList>();
		param_list_ptr->add_param(std::move($1));
		$$ = std::move(param_list_ptr);
	}
	| ParamList "," Param
	{
		assert_same_ptr(tinyc::ParamList, $1);
		assert_same_ptr(tinyc::Param, $3);

		auto param_list_ptr = std::move($1);
		param_list_ptr->add_param(std::move($3));
		$$ = std::move(param_list_ptr);
	}

Param :
	Type Ident
	{
		assert_same_ptr(tinyc::Type, $1);
		assert_same_ptr(tinyc::Ident, $2);
		auto param_ptr = std::make_unique<tinyc::Param>(std::move($1), std::move($2));
		$$ = std::move(param_ptr);
	}

Type        
	: KW_INT {
		$$ = std::make_unique<tinyc::Type>(tinyc::Type::ty_int);
	}
	| KW_VOID {
		$$ = std::make_unique<tinyc::Type>(tinyc::Type::ty_void);
	};

Block
	: "{" Stmt "}"{
		assert_same_ptr(tinyc::Stmt, $2);
		//这里暂时只匹配单个表达式
		auto block_ptr = std::make_unique<tinyc::Block>();
		block_ptr->add_stmt(std::move($2));
		$$ = std::move(block_ptr);
	};

Stmt
	: KW_RETURN Expr ";" {
		assert_same_ptr(tinyc::Expr, $2);
		auto stmt_ptr = std::make_unique<tinyc::Stmt>(std::move($2));
		$$ = std::move(stmt_ptr);
	};

Expr
	: Number {
		assert_same_ptr(tinyc::Number, $1);
		auto expr_ptr = std::make_unique<tinyc::Expr>(std::move($1));
		$$ = std::move(expr_ptr);
	}
	| Ident {
		assert_same_ptr(tinyc::Ident, $1);
		auto expr_ptr = std::make_unique<tinyc::Expr>(std::move($1));
		$$ = std::move(expr_ptr);
	};

Number
	: INT_LITERAL{
		$$ = std::make_unique<tinyc::Number>($1);
	};

Ident
	: IDENT{
		$$ = std::make_unique<tinyc::Ident>($1);
	};

%%

namespace yy
{

void parser::error(const location_type& loc, const std::string& m)
{
	std::cerr<<loc<<": "<<m<<std::endl;
}

}	//namespace yy

