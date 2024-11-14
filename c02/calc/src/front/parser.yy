%skeleton "lalr1.cc"	// use LALR-1, C++ language
%require "3.8.2"
%header
%define api.token.raw  			// 禁止在文法推导中使用字符字面量 
%define api.token.constructor 	// 将symbol作为一个类型
%define api.value.type variant 	// 使用variant代替union
%define parse.assert			// 辅助检查不变量，无法在禁用了rtti的情况下使用

%code requires {
#include <string>
class Driver;
}

%param { Driver& drive }		// 影响 yylex 和 yyparse

%locations
%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {
#include "driver.hpp"
}

%define api.token.prefix {TOK_}
%token
	ASSIGN 	":="
	MINUS 	"-"
	PLUS 	"+"
	STAR	"*"
	SLASH	"/"
	LPAREN	"("
	RPAREN	")"
;
%token <std::string> IDENTIFIER "identifier"
%token <int> NUMBER "number"

%nterm <int> exp

%printer { yyo << $$; } <*>;

%%
%start unit;
unit: assignments exp {
		drive.set_result($2);
	};

assignments:
	%empty {}
	| assignments assignment {};

assignment:
	"identifier" ":=" exp
	{ drive.insert($1, $3); };

%left "+" "-";
%left "*" "/";

exp:
   "number"
   | "identifier" { $$ = drive.find($1); }
   | exp "+" exp  { $$ = $1 + $3; }
   | exp "-" exp  { $$ = $1 - $3; }
   | exp "*" exp  { $$ = $1 * $3; }
   | exp "/" exp  { $$ = $1 / $3; }
   | "(" exp ")"  { $$ = $2; }

%%

void yy::parser::error(const location_type& loc, const std::string& m)
{
	std::cerr<<loc<<": "<<m<<std::endl;
}

