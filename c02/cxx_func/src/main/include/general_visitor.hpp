#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <easylog.hpp>
#include "ast.hpp"

namespace tinyc
{

class GeneralVisitor: public ASTVisitor
{
public:
	GeneralVisitor();
	/// 只支持根节点输出
	auto visit(BaseAST* ast) -> bool override;

private:
	void handle(const CompUnit& node);
	void handle(const FuncDef& node);
	void handle(const Type& node);
	void handle(const Ident& node);
	void handle(const ParamList& node);
	void handle(const Block& node);
	void handle(const Param& node);
	void handle(const Stmt& node);
	void handle(const Expr& expr);
	void handle(const Number& num);

private:
	yq::logger<yq::log_level::error> m_error_log;
	yq::logger<yq::log_level::debug> m_debug_log;
	llvm::Module* m_module;
	llvm::IRBuilder<> m_builder;
	
};

}	//namespace tinyc

