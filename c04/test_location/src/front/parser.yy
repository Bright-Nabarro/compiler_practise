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
#include "llvm_location.hpp"
//前向声明
namespace tinyc { class Driver; }
}

%param { tinyc::Driver& driver }

%locations	//生成location定位
%define api.location.type { tinyc::LLVMLocation}	//使用自定义location类型
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
#include "ast.hpp"
#include "llvm_location.hpp"
}

%token <std::string> IDENT
%token <int> INT_LITERAL
//关键字
%token KW_RETURN
%token KW_SINT KW_UINT KW_VOID 
%nterm <std::unique_ptr<tinyc::CompUnit>>		CompUnit

%%

%start CompUnit;

CompUnit
	: INT_LITERAL 
	{
 		std::unique_ptr<tinyc::Location> location =
			std::make_unique<tinyc::LLVMLocation>(@$);
 
		auto comp_unit_ptr =
			std::make_unique<tinyc::CompUnit>(std::move(location));
	}
	| IDENT
	{
		std::unique_ptr<tinyc::Location> location =
			std::make_unique<tinyc::LLVMLocation>(@$);
 
		auto comp_unit_ptr =
			std::make_unique<tinyc::CompUnit>(std::move(location));

	};
%%

namespace yy
{

void parser::error(const location_type& loc, const std::string& m)
{
	(void)loc, (void)m;
	//loc.set_src_mgr(&(this->driver.get_src_mgr()));
	//loc.report(tinyc::Location::dk_error, m);
}

}	//namespace yy

