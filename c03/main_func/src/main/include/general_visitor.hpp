#pragma once

#include "ast.hpp"
#include "c_type_manager.hpp"
#include <easylog.hpp>
#include <memory>
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
	auto handle(const Ident& node) -> std::pair<llvm::Value*, std::string>;
	auto handle(const ParamList& node) -> std::vector<llvm::Type*>;
	auto handle(const Block& node, llvm::Function* func,
				std::string_view block_name) -> llvm::BasicBlock*;
	auto handle(const Param& node) -> llvm::Type*;
	void handle(const Stmt& node);
	auto handle(const Expr& expr) -> llvm::Value*;
	auto handle(const PrimaryExpr& node) -> llvm::Value*;
	auto handle(const UnaryExpr& node) -> llvm::Value*;
	auto handle(const UnaryOp& op, llvm::Value* operand) -> llvm::Value*;
	auto handle(const Number& num) -> llvm::Value*;

	/// @note 在上层会传入所有的BinaryExpr, 无需在实现文件中显式实例化声明
	template<typename BinaryExpr>
	auto handle(const BinaryExpr& node) -> llvm::Value*;

	/// @brief 二元运算符通用处理函数
	auto binary_operate(llvm::Value* left, const Operation& op,
						llvm::Value* right) -> llvm::Value*;

private:
	std::unique_ptr<llvm::Module> m_module;
	llvm::IRBuilder<> m_builder;
	std::shared_ptr<CTypeManager> m_type_mgr;
	
	bool m_emit_llvm;
	std::string_view m_output_file;
	
	llvm::TargetMachine* m_target_machine;
};

}	//namespace tinyc

