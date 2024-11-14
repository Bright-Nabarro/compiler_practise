%{

#include <string>
#include "parser.bison.hpp"

%}

%option noyywrap nounput batch debug noinput

WhiteSpace 		[ \t\n\r]*
LineComment		"//".*
Ident			[a-zA-Z_][0-9a-zA-Z_]*
Number			[0-9]+

%%

{WhiteSpace}	{}
{LineComment}	{}
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
.				{ return yytext[0]; }


%%

