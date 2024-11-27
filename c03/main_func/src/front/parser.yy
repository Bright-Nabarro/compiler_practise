%skeleton "lalr1.cc"	// use LALR-1, C++ language
%require "3.8.2"
%header
%define api.token.raw  			// 禁止在文法推导中使用字符字面量 
%define api.token.constructor 	// 将symbol作为一个类型
%define api.value.type variant 	// 使用variant代替union

%code requires {
#include <memory>
#include <string>
#include <string_view>
#include <iostream>
#include "ast.hpp"
#include "location_range.hpp"
//前向声明
namespace tinyc { class Driver; }
}

%param { tinyc::Driver& driver }

%locations	//生成location定位
%define api.location.type { tinyc::LocationRange }	//使用自定义location类型
//%define api.filename.type { std::string_view }
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
%token DELIM_LPAREN		"("
%token DELIM_RPAREN		")"
%token DELIM_LBRACE		"{"
%token DELIM_RBRACE		"}"
%token DELIM_COMMA 		","
%token DELIM_SEMICOLON	";"
// 操作符
%token OP_ADD	"+"
%token OP_SUB	"-"
%token OP_NOT	"!"
%token OP_MUL	"*"
%token OP_DIV	"/"
%token OP_MOD	"%"
%token OP_LT	"<"
%token OP_LE	"<="
%token OP_GT	">"
%token OP_GE	">="
%token OP_EQ	"=="
%token OP_NE	"!="
%token OP_LAND	"&&"
%token OP_LOR	"||"

%nterm <std::unique_ptr<tinyc::Number>>			Number
%nterm <std::unique_ptr<tinyc::Ident>>			Ident
%nterm <std::unique_ptr<tinyc::Expr>>			Expr
%nterm <std::unique_ptr<tinyc::Stmt>>			Stmt
%nterm <std::unique_ptr<tinyc::Block>>			Block
%nterm <std::unique_ptr<tinyc::Type>>			Type
%nterm <std::unique_ptr<tinyc::Param>>			Param
%nterm <std::unique_ptr<tinyc::ParamList>>		ParamList
%nterm <std::unique_ptr<tinyc::FuncDef>>		FuncDef
%nterm <std::unique_ptr<tinyc::CompUnit>>		CompUnit
%nterm <std::unique_ptr<tinyc::UnaryExpr>>		UnaryExpr
%nterm <std::unique_ptr<tinyc::UnaryOp>>		UnaryOp
%nterm <std::unique_ptr<tinyc::PrimaryExpr>>	PrimaryExpr
%nterm <std::unique_ptr<tinyc::L3Expr>> 		L3Expr
%nterm <std::unique_ptr<tinyc::L3Op>> 			L3Op
%nterm <std::unique_ptr<tinyc::L4Expr>>			L4Expr
%nterm <std::unique_ptr<tinyc::L4Op>>			L4Op
%nterm <std::unique_ptr<tinyc::L6Expr>>			L6Expr
%nterm <std::unique_ptr<tinyc::L6Op>>			L6Op
%nterm <std::unique_ptr<tinyc::L7Expr>>			L7Expr
%nterm <std::unique_ptr<tinyc::L7Op>>			L7Op
%nterm <std::unique_ptr<tinyc::LAndExpr>>		LAndExpr
%nterm <std::unique_ptr<tinyc::LAndOp>>			LAndOp
%nterm <std::unique_ptr<tinyc::LOrExpr>>		LOrExpr
%nterm <std::unique_ptr<tinyc::LOrOp>>			LOrOp


%%

%start CompUnit;

CompUnit:
	FuncDef 
	{
		//llvm::isa足够智能，能够区分裸指针和智能指针的情况
		assert_same_ptr(tinyc::FuncDef, $1);
		auto comp_unit_ptr = std::make_unique<tinyc::CompUnit>(@$, std::move($1));
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

		auto funcdef_ptr = std::make_unique<tinyc::FuncDef>( @$,
			std::move($1), std::move($2), std::move($4), std::move($6)
		);

		$$ = std::move(funcdef_ptr);
	};

ParamList :
	/* empty */
	{
		$$ = std::make_unique<tinyc::ParamList>(@$);
	}
	| Param
	{
		assert_same_ptr(tinyc::Param, $1);
		auto param_list_ptr = std::make_unique<tinyc::ParamList>(@$);
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
		auto param_ptr = std::make_unique<tinyc::Param>(@$, std::move($1), std::move($2));
		$$ = std::move(param_ptr);
	}

Type        
	: KW_INT {
		$$ = std::make_unique<tinyc::Type>(@$, tinyc::Type::ty_int);
	}
	| KW_VOID {
		$$ = std::make_unique<tinyc::Type>(@$, tinyc::Type::ty_void);
	};

Block
	: "{" Stmt "}"{
		assert_same_ptr(tinyc::Stmt, $2);
		//这里暂时只匹配单个表达式
		auto block_ptr = std::make_unique<tinyc::Block>(@$);
		block_ptr->add_stmt(std::move($2));
		$$ = std::move(block_ptr);
	};

Stmt
	: KW_RETURN Expr ";" {
		assert_same_ptr(tinyc::Expr, $2);
		auto stmt_ptr = std::make_unique<tinyc::Stmt>(@$, std::move($2));
		$$ = std::move(stmt_ptr);
	};

Expr
	: LOrExpr {
		assert_same_ptr(tinyc::LOrExpr, $1);
		$$ = std::make_unique<tinyc::Expr>(@$, std::move($1));
	};

PrimaryExpr
	: "(" Expr ")" {
		assert_same_ptr(tinyc::Expr, $2);
		$$ = std::make_unique<tinyc::PrimaryExpr>(@$, std::move($2));
	}
	| Number {
		assert_same_ptr(tinyc::Number, $1);
		$$ = std::make_unique<tinyc::PrimaryExpr>(@$, std::move($1));
	}
	| Ident {
		assert_same_ptr(tinyc::Ident, $1);
		$$ = std::make_unique<tinyc::PrimaryExpr>(@$, std::move($1));
	};

UnaryExpr
	: PrimaryExpr {
		assert_same_ptr(tinyc::PrimaryExpr, $1);
		$$ = std::make_unique<tinyc::UnaryExpr>(@$, std::move($1));
	}
	| UnaryOp UnaryExpr {
		assert_same_ptr(tinyc::UnaryOp, $1);
		assert_same_ptr(tinyc::UnaryExpr, $2);
		$$ = std::make_unique<tinyc::UnaryExpr>(@$, std::move($1), std::move($2));
	};

UnaryOp
	: "+" {
		$$ = std::make_unique<tinyc::UnaryOp>(@$, tinyc::UnaryOp::op_add);
	}
	| "-" {
		$$ = std::make_unique<tinyc::UnaryOp>(@$, tinyc::UnaryOp::op_sub);
	} 
	| "!" {
		$$ = std::make_unique<tinyc::UnaryOp>(@$, tinyc::UnaryOp::op_not);
	};

L3Expr
	: UnaryExpr {
		assert_same_ptr(tinyc::UnaryExpr, $1);
		$$ = std::make_unique<tinyc::L3Expr>(@$, std::move($1));
	}
	| L3Expr L3Op UnaryExpr {
		assert_same_ptr(tinyc::L3Expr, $1);
		assert_same_ptr(tinyc::L3Op, $2);
		assert_same_ptr(tinyc::UnaryExpr, $3);
		$$ = std::make_unique<tinyc::L3Expr>(@$, std::move($1), std::move($2), std::move($3));
	};

L3Op
	: "*"  {
		$$ = std::make_unique<tinyc::L3Op>(@$, tinyc::Operation::op_mul);
	}
	| "/"  {
		$$ = std::make_unique<tinyc::L3Op>(@$, tinyc::Operation::op_div);
	}
	| "%" {
		$$ = std::make_unique<tinyc::L3Op>(@$, tinyc::Operation::op_mod);
	};

L4Expr
	: L3Expr {
		assert_same_ptr(tinyc::L3Expr, $1);
		$$ = std::make_unique<tinyc::L4Expr>(@$, std::move($1));
	}
	| L4Expr L4Op L3Expr {
		assert_same_ptr(tinyc::L4Expr, $1);
		assert_same_ptr(tinyc::L4Op, $2);
		assert_same_ptr(tinyc::L3Expr, $3);
		$$ = std::make_unique<tinyc::L4Expr>(@$, std::move($1), std::move($2), std::move($3));
	};

L4Op
	: "+" {
		$$ = std::make_unique<tinyc::L4Op>(@$, tinyc::Operation::op_add);
	}
	| "-" {
		$$ = std::make_unique<tinyc::L4Op>(@$, tinyc::Operation::op_sub);
	};

L6Expr
	: L4Expr {
		assert_same_ptr(tinyc::L4Expr, $1);
		$$ = std::make_unique<tinyc::L6Expr>(@$, std::move($1));
	}
	| L6Expr L6Op L4Expr {
		assert_same_ptr(tinyc::L6Expr, $1);
		assert_same_ptr(tinyc::L6Op, $2);
		assert_same_ptr(tinyc::L4Expr, $3);
		$$ = std::make_unique<tinyc::L6Expr>(@$, std::move($1), std::move($2), std::move($3));
	};

L6Op
	: "<" {
		$$ = std::make_unique<tinyc::L6Op>(@$, tinyc::Operation::op_lt);
	}
	| ">" {
		$$ = std::make_unique<tinyc::L6Op>(@$, tinyc::Operation::op_gt);
	}
	| "<=" {
		$$ = std::make_unique<tinyc::L6Op>(@$, tinyc::Operation::op_le);
	}
	| ">=" {
		$$ = std::make_unique<tinyc::L6Op>(@$, tinyc::Operation::op_ge);
	};

L7Expr      
	: L6Expr {
		assert_same_ptr(tinyc::L6Expr, $1);
		$$ = std::make_unique<tinyc::L7Expr>(@$, std::move($1));
	}
	| L7Expr L7Op L6Expr {
		assert_same_ptr(tinyc::L7Expr, $1);
		assert_same_ptr(tinyc::L7Op, $2);
		assert_same_ptr(tinyc::L6Expr, $3);
		$$ = std::make_unique<tinyc::L7Expr>(@$, std::move($1), std::move($2), std::move($3));
	};

L7Op		
	: "==" {
		$$ = std::make_unique<tinyc::L7Op>(@$, tinyc::Operation::op_eq);
	}
	| "!=" {
		$$ = std::make_unique<tinyc::L7Op>(@$, tinyc::Operation::op_ne);
	};

LAndExpr	
	: L7Expr {
		assert_same_ptr(tinyc::L7Expr, $1);
		$$ = std::make_unique<tinyc::LAndExpr>(@$, std::move($1));
	}
	| LAndExpr LAndOp L7Expr{
		assert_same_ptr(tinyc::LAndExpr, $1);
		assert_same_ptr(tinyc::LAndOp, $2);
		assert_same_ptr(tinyc::L7Expr, $3);
		$$ = std::make_unique<tinyc::LAndExpr>(@$, std::move($1), std::move($2), std::move($3));
	};

LAndOp		
	: "&&" {
		$$ = std::make_unique<tinyc::LAndOp>(@$, tinyc::Operation::op_land);
	}

LOrExpr
	: LAndExpr {
		assert_same_ptr(tinyc::LAndExpr, $1);
		$$ = std::make_unique<tinyc::LOrExpr>(@$, std::move($1));
	}
	| LOrExpr LOrOp LAndExpr {
		assert_same_ptr(tinyc::LOrExpr, $1);
		assert_same_ptr(tinyc::LOrOp, $2);
		assert_same_ptr(tinyc::LAndExpr, $3);
		$$ = std::make_unique<tinyc::LOrExpr>(@$, std::move($1), std::move($2), std::move($3));
	};

LOrOp	
	: "||" {
		$$ = std::make_unique<tinyc::LOrOp>(@$, tinyc::Operation::op_lor);
	};

Number
	: INT_LITERAL{
		$$ = std::make_unique<tinyc::Number>(@$, $1);
	};

Ident
	: IDENT{
		$$ = std::make_unique<tinyc::Ident>(@$, $1);
	};

%%

namespace yy
{

void parser::error(const location_type& loc, const std::string& m)
{
	loc.report(this->driver.get_src_mgr(),
		llvm::SourceMgr::DK_Error, m);
}

}	//namespace yy

