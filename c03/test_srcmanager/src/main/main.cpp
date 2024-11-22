#include <llvm/Support/CommandLine.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/SMLoc.h>
#include <string_view>
#include <print>

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
	"emit-llvm", 
	llvm::cl::desc("Emit LLVM IR code instead of machine code"),
	llvm::cl::init(false)};

auto main(int argc, char* argv[]) -> int
{
	//llvm::InitLLVM X(argc, argv);
	// 解析命令行选项
	llvm::cl::ParseCommandLineOptions(argc, argv,
									  "Simple LLVM CommandLine Example\n");
	llvm::SourceMgr source_mgr;
	auto buffer_or_error = llvm::MemoryBuffer::getFile(input_filename);
	if (!buffer_or_error)
	{
		llvm::WithColor::error(llvm::errs(), input_filename)
			<< "Failed to open" << input_filename << '\n';
		return 1;
	}

	unsigned bufferid = source_mgr.AddNewSourceBuffer(std::move(*buffer_or_error), llvm::SMLoc());
	const char* buffer_start = source_mgr.getMemoryBuffer(bufferid)->getBufferStart();

	llvm::SMLoc error_start = llvm::SMLoc::getFromPointer(buffer_start);
	llvm::SMLoc error_end = llvm::SMLoc::getFromPointer(buffer_start + 5);
	llvm::SMRange range { error_start, error_end };

	std::string str_buf;
	llvm::raw_string_ostream os { str_buf }; 

	source_mgr.PrintMessage(os, error_start, llvm::SourceMgr::DK_Error, "hello error", range);
	std::println("{}", str_buf);
	//source_mgr.PrintMessage(error_start, llvm::SourceMgr::DK_Error, "hello error");

	return 0;
}

