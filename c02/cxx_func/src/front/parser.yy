%skeleton "lalr1.cc"	// use LALR-1, C++ language
%require "3.8.2"
%header
%define api.token.constructor 	// 将symbol作为一个类型
%define api.value.type variant 	// 使用variant代替union

%code requires {
#include <memory>
#include <string>
#include <string_view>
//前向声明
namespace tinyc { class Driver; }
}

%param { tinyc::Driver& drive }

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
	static_assert(std::is_same_v<Type, typename decltype(ptr)::element_type>)

}

%token KW_INT KW_VOID KW_RETURN
%token <std::string> IDENT
%token <int> INT_LITERAL

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
	};

FuncDef :
//   1	   2 	3      4   	  5   6
	Type Ident '(' ParamList ')' Block
	{
		assert_same_ptr(tinyc::Type,$1);
		assert_same_ptr(tinyc::Ident, $2);
		assert_same_ptr(tinyc::ParamList, $4);
		assert_same_ptr(tinyc::Block, $6);

		auto funcdef_ptr = std::make_unique<tinyc::FuncDef>(
			std::move($1), std::move($2), std::move($4), std::move($6)
		);

		$$ = std::move($1);
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
	| ParamList ',' Param
	{
		assert_same_ptr(tinyc::ParamList, $1);
		assert_same_ptr(tinyc::Param, $3);

		auto param_list_ptr = std::move($1);
		param_list_ptr->add_param($3);
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
		$$ = std::make_unique<tinyc::Type>(tinyc::Type::ast_int);
	}
	| KW_VOID {
		$$ = std::make_unique<tinyc::Type>(tinyc::Type::ast_void);
	};

Block
	: '{' Stmt '}'{
		assert_same_ptr(tinyc::Stmt, $2);
		//这里暂时只匹配单个表达式
		$$ = std::make_unique<tinyc::Block>();
	};

Stmt
	: KW_RETURN Expr ';' {
		assert_same_ptr(tinyc::Expr, $2);
		auto stmt_ptr = std::make_unique<tinyc::Stmt>(std::move($2));
		$$ = std::move(stmt_ptr);
	};

Expr
	: Number {
		assert_same_ptr(tinyc::Number $1);
		auto number_uptr = std::make_unique<tinyc::Number>($1);
		$$ = std::move(number_uptr);
	}
	| Ident {
		assert_same_ptr(tinyc::Number $1);
		auto ident_uptr = std::make_unique<tinyc::Ident>($1);
		$$ = std::move(ident_uptr);
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

