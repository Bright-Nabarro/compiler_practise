#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <easylog.hpp>
#include <expected>
#include "ast.hpp"

namespace tinyc
{

class GeneralVisitor: public ASTVisitor
{
public:
	GeneralVisitor(llvm::LLVMContext& context);
	/// 只支持根节点输出
	auto visit(BaseAST* ast) -> bool override;

private:
	void handle(const CompUnit& node);
	void handle(const FuncDef& node);
	auto handle(const Type& node) -> llvm::Type*;
	auto handle(const Ident& node) -> std::string;
	auto handle(const ParamList& node) -> std::vector<llvm::Type*>;
	void handle(const Block& node);
	auto handle(const Param& node) -> llvm::Type*;
	void handle(const Stmt& node);
	void handle(const Expr& expr);
	void handle(const Number& num);

private:
	yq::logger<yq::log_level::error> m_error_log;
	yq::logger<yq::log_level::debug> m_debug_log;

	llvm::LLVMContext& m_llvm_context;
	std::shared_ptr<llvm::Module> m_module;
	llvm::IRBuilder<> m_builder;
	llvm::Type* m_void_ty;
	llvm::Type* m_int32_ty;
};

}	//namespace tinyc

