%{
#include <cerrno>
#include <cstring> 	//strerror
#include <cstdlib>
#include "driver.hpp"
#include "bison_parser.hpp"
%}

%option noyywrap nounput noinput batch debug

blank	 		[ \t\r]+
LineComment		//.*\n
Ident			[a-zA-Z_][0-9a-zA-Z_]*
Number			[0-9]+

%%
%{
	//在%% %%中间插入的代码每次yylex执行时调用
	yy::location& loc = driver.get_location();
	//如果之前已经步进过，则此时不会改变
	loc.step();
%}

{blank}			loc.step();
\n+				{
					//这里的yyleng是\n数量，因为仅匹配\n
					loc.lines(yyleng);
					loc.step(); 
				}
{LineComment}	{
					loc.lines(1);
					loc.step();
				}
"int"			return yy::parser::make_KW_INT();
"void"			return yy::parser::make_KW_VOID();
"return"		return yy::parser::make_KW_RETURN();

{Ident}			{
					yylval->emplace(std::string{yytext});
					return yy::parser::make_IDENT();
				}
{Number}		{
					yylval->emplace(std::atoi(yytext));
					return yy::parser::make_INT_LITERAL();
				}
.				return yy::parser::make_YYerror(loc);
<<EOF>>			return yy::parser::make_YYEOF(loc);

%%

