#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "ast.hpp"
#include <print>

extern auto yyparse(std::unique_ptr<tinyc::BaseAST>& ast) -> int;

extern FILE* yyin;

// 定义命令行选项
static llvm::cl::opt<std::string> input_filename{
	llvm::cl::Positional, // 位置参数，无需用 "--" 指定
	llvm::cl::desc("<input file>"),
	//llvm::cl::init("-") // 默认值为 "-"
};

static llvm::cl::opt<std::string> output_filename{
	"o", // 使用 -o 指定
	llvm::cl::desc("Specify output filename"), llvm::cl::value_desc("filename"),
	llvm::cl::init("output.txt")};

static llvm::cl::opt<bool> emit_llvm{
	"emit-llvm", // 使用 --emit-llvm 启用
	llvm::cl::desc("Emit LLVM IR code instead of machine code"),
	llvm::cl::init(false)};

auto main(int argc, char* argv[]) -> int
{
	llvm::InitLLVM X(argc, argv);
	// 解析命令行选项
	llvm::cl::ParseCommandLineOptions(argc, argv,
									  "Simple LLVM CommandLine Example\n");

	// 打印解析后的参数值
	std::println("Input file: {}", input_filename.getValue());
	std::println("Output file: {}", output_filename.getValue());
	std::println("Emit LLVM IR: {}", emit_llvm.getValue());

	return 0;
}

