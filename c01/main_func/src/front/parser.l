%option noyywrap
%option nounput

%{

#include <string>
#include "parser.bison.hpp"

%}

WhiteSpace 		[ \t\n\r]*
LineComment		"//".*
Ident			[a-zA-Z_][0-9a-zA-Z_]*
Number			[0-9]+

%%

{WhiteSpace}	{}
{LineComment}	{}
"int"			{ return KW_INT; }
"void"			{ return KW_VOID; }
"return"		{ return KW_RETURN; }

{Ident}			{
					yylval.str_val = new std::string{yytext};
					return IDENT;
				}
{Number}		{
					yylval.int_val = std::atoi(yytext);
					return INT_LITERAL;
				}
.				{ return yytext[0]; }
%%

