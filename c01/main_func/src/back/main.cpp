#include <print>
#include <memory>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <string>
#include "ast.hpp"
#include "include/output_visitor.hpp"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"

extern auto yyparse(std::unique_ptr<ast::BaseAST>& ast) -> int;

extern FILE* yyin;

static llvm::cl::opt<std::string> input_filename {
	llvm::cl::Positional,
	llvm::cl::desc("<input file>"),
};

static llvm::cl::opt<std::string> output_filename {
	"o",
	llvm::cl::desc { "Specify output filename" },
	llvm::cl::value_desc { "filename" },
	llvm::cl::init("a.out")
};

static llvm::cl::opt<bool> emit_llvm {
	"emit-llvm",
	llvm::cl::desc("Emit llvm ir instead of machine code"),
	llvm::cl::init(false)
};

static std::unique_ptr<ast::BaseAST> base;

bool parse()
{
	yyin = std::fopen(input_filename.c_str(), "r");
	if (yyin == nullptr)
	{
		std::println(stderr, "fopen err {}", std::strerror(errno));
		return false;
	}
	int ret = yyparse(base);
	OutputVisitor visiter;
	base->accept(visiter);
	if (ret != 0)
	{
		std::println(stderr, "parse error");
		return false;
	}
	return true;
}

auto main(int argc, char* argv[]) -> int
{
	llvm::InitLLVM X { argc, argv };
	llvm::cl::ParseCommandLineOptions(argc, argv, "main func compiler");
	

	if (parse())
		return 1;
}

