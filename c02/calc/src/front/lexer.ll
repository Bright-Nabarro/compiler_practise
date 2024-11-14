%{
#include <iostream>
#include <cerrno>
#include <cstring> 	//strerror
#include <cstdlib>
#include "driver.hpp"
#include "bison_parser.hpp"
%}

%option noyywrap nounput noinput batch debug

%{
	auto make_NUMBER (const std::string& s,
			const yy::parser::location_type& loc) ->
	yy::parser::symbol_type;
%}

id		[a-zA-Z][a-zA-Z_0-9]*
int		[0-9]+
blank	[ \t\r]+

%{
	// 每次匹配到一个模式时运行的代码。
	#define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
	//在%% %%中间插入的代码每次yylex执行时调用
	yy::location& loc = driver.get_location();
	//如果之前已经步进过，则此时不会改变
	loc.step();
%}

{blank}		loc.step();
\n+			loc.lines(yyleng); loc.step();
"-"			return yy::parser::make_MINUS(loc);
"+"			return yy::parser::make_PLUS(loc);
"*"			return yy::parser::make_STAR(loc);
"/"			return yy::parser::make_SLASH(loc);
"("			return yy::parser::make_LPAREN(loc);
")"			return yy::parser::make_RPAREN(loc);
":="		return yy::parser::make_ASSIGN(loc);
{int}		return make_NUMBER(yytext, loc);
{id}		return yy::parser::make_IDENTIFIER(yytext, loc);
.			{
				throw yy::parser::syntax_error
					(loc, "invalid character: "+std::string{yytext});
			}
<<EOF>>		return yy::parser::make_YYEOF(loc);
%%

auto make_NUMBER (const std::string& s, const yy::parser::location_type& loc) ->
	yy::parser::symbol_type
{
	int n = std::stoi(s);
	return yy::parser::make_NUMBER(n, loc);
}

void Driver::scan_begin()
{
	yy_flex_debug = get_trace();
	if (get_filename().empty() || get_filename() == "-")
	{
		yyin = stdin;
	}
	else
	{
		yyin = std::fopen(get_filename().c_str(), "r");
		if (!yyin)
		{
			std::cerr<<"cannot open "<<get_filename()<<": "<<strerror(errno)<<std::endl;
		}
	}
}

void Driver::scan_end()
{
	fclose(yyin);
}

