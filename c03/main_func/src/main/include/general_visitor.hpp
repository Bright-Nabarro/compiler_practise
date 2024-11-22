#pragma once

#include "ast.hpp"
#include <easylog.hpp>
#include <expected>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

namespace tinyc
{

class GeneralVisitor: public ASTVisitor
{
public:
	GeneralVisitor(llvm::LLVMContext& context, bool emit_llvm,
				   std::string_view output_file, llvm::TargetMachine* tm);
	/// @note 只支持从根节点翻译
	[[nodiscard]]
	auto visit(BaseAST* ast) -> bool override;

	/**
	 * @brief 将m_module转换为对应格式输出, 由程序的argc参数指定
	 * @note emit-llvm 生成llvm-ir
	 * @note filetype=obj 生成二进制文件
	 * @note filetype=asm && 没有指定 emit-llvm生成汇编
	 */
	[[nodiscard]]
	auto emit() -> bool;

private:
	void handle(const CompUnit& node);
	void handle(const FuncDef& node);
	auto handle(const Type& node) -> llvm::Type*;
	auto handle(const Ident& node) -> std::string;
	auto handle(const ParamList& node) -> std::vector<llvm::Type*>;
	auto handle(const Block& node, llvm::Function* func,
				std::string_view block_name) -> llvm::BasicBlock*;
	auto handle(const Param& node) -> llvm::Type*;
	void handle(const Stmt& node);
	auto handle(const Expr& expr) -> llvm::Value*;
	auto handle(const Number& num) -> int;


private:
	std::shared_ptr<llvm::Module> m_module;
	llvm::IRBuilder<> m_builder;
	llvm::Type* m_void_ty;
	llvm::Type* m_int32_ty;
	
	bool m_emit_llvm;
	std::string_view m_output_file;
	
	llvm::TargetMachine* m_target_machine;
};

}	//namespace tinyc

