%{
#include <cerrno>
#include <cstring> 	//strerror
#include <cstdlib>
#include "driver.hpp"
#include "ast.hpp"
#include "bison_parser.hpp"

#define LOC_UPDATE_RET_ACTION(loc, action)                                     \
	do                                                                         \
	{                                                                          \
		return (action);                                                       \
	} while (0)

#define LOC_UPDATE_NORMAL(loc)                                                 \
	do                                                                         \
	{                                                                          \
		loc.step();                                                            \
	} while(0)

%}

%option noyywrap nounput noinput batch debug

blank	 		[ \t\r\n]+
LineComment		\/\/[^\n]*\n
LegacyComment 	\/\*.*\*\/
Ident			[a-zA-Z_][0-9a-zA-Z_]*
Number			[0-9]+
SignedInt		(signed\s+int)|(int)|(signed)
UnsignedInt		(unsigned\s+int)|(unsigned)
%%

%{
	//在%% %%中间插入的代码每次yylex执行时调用
	//如果之前已经步进过，则此时不会改变
	auto& loc = driver.get_location();
	loc.step();
%}

{blank}			LOC_UPDATE_NORMAL(loc);

{Ident}			LOC_UPDATE_RET_ACTION(loc, yy::parser::make_IDENT(std::string{yytext}, loc));
{Number}		LOC_UPDATE_RET_ACTION(loc, yy::parser::make_INT_LITERAL(std::atoi(yytext), loc));
.				{
					driver.get_parser().error(loc, "expect token");
					return yy::parser::make_YYerror(loc);
				}
				
<<EOF>>			return yy::parser::make_YYEOF(loc);

%%


namespace tinyc
{

void Driver::set_flex(const char* buffer, int buffer_size)
{
	yy_flex_debug = this->get_trace();
	yy_scan_bytes(buffer, buffer_size);
}

}	//namespace tinyc

