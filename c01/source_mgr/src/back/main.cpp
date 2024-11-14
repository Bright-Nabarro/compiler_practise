#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

int main() {
    llvm::SourceMgr sourceMgr;
    
    // 加载文件内容到 MemoryBuffer 中
    auto buffer = llvm::MemoryBuffer::getFile("example.txt");
    if (!buffer) {
        llvm::errs() << "Error reading file\n";
        return 1;
    }

    // 将 MemoryBuffer 添加到 SourceMgr
    unsigned bufferID = sourceMgr.AddNewSourceBuffer(std::move(*buffer), llvm::SMLoc());

    // 打印包含指定位置的诊断信息
    llvm::SMLoc loc = sourceMgr.FindLocForLineAndColumn(bufferID, 1, 1);  // 例如在第1行第1列位置
    sourceMgr.PrintMessage(loc, llvm::SourceMgr::DiagKind::DK_Error, "Example error message");

    return 0;
}

