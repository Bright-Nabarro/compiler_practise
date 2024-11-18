#include <llvm/Support/CommandLine.h>
#include <llvm/Support/InitLLVM.h>
#include <print>
#include "driver.hpp"
#include "general_visitor.hpp"

// 定义命令行选项
static llvm::cl::opt<std::string> input_file {
	llvm::cl::Positional, // 位置参数，无需用 "--" 指定
	llvm::cl::desc("<input file>"),
	llvm::cl::init("") // 默认值为 empty
};

static llvm::cl::opt<std::string> output_file{
	"o", // 使用 -o 指定
	llvm::cl::desc("Specify output filename"), llvm::cl::value_desc("filename"),
	llvm::cl::init("output.txt")
};

static llvm::cl::opt<bool> emit_llvm{
	"emit-llvm", 
	llvm::cl::desc("Emit LLVM IR code instead of machine code"),
	llvm::cl::init(false)
};

/// 开启debug追踪
static llvm::cl::opt<bool> debug_trace{
	"trace",
	llvm::cl::desc("Enable flex debug trace"),
	llvm::cl::init(false)
};

auto main(int argc, char* argv[]) -> int
{
	llvm::InitLLVM X(argc, argv);
	// 解析命令行选项
	llvm::cl::ParseCommandLineOptions(argc, argv,
									  "Simple LLVM CommandLine Example\n");
	tinyc::Driver driver;
	auto file = input_file.getValue();
	driver.parse(file);
	
	tinyc::GeneralVisitor visitor;
	visitor.visit(driver.get_ast_ptr());

	return 0;
}

