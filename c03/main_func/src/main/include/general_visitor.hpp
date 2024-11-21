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
	/// 只支持根节点输出
	auto visit(BaseAST* ast) -> bool override;
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

	void generate_objectfile();

private:
	yq::logger<yq::log_level::error> m_error_log;
	yq::logger<yq::log_level::debug> m_debug_log;

	std::shared_ptr<llvm::Module> m_module;
	llvm::IRBuilder<> m_builder;
	llvm::Type* m_void_ty;
	llvm::Type* m_int32_ty;
	
	bool m_emit_llvm;
	std::string_view m_output_file;
	
	llvm::TargetMachine* m_target_machine;
};

}	//namespace tinyc

