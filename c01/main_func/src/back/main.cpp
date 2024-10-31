#include <print>
#include <memory>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include "ast.hpp"
#include "include/output_visitor.hpp"

extern auto yyparse(std::unique_ptr<ast::BaseAST>& ast) -> int;

extern FILE* yyin;

auto main(int argc, char* argv[]) -> int
{
	std::unique_ptr<ast::BaseAST> base;
	if (argc <= 1)
	{
		std::println(stderr, "Need a file name");
		return 1;
	}

	yyin = std::fopen(argv[1], "r");
	if (yyin == nullptr)
	{
		std::println(stderr, "fopen err {}", std::strerror(errno));
		return 1;
	}

	int ret = yyparse(base);
	OutputVisitor visiter;
	base->accept(visiter);
	if (ret)
	{
		std::println(stderr, "parse error");
		return 1;
	}
}

