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
#include <llvm/Support/SourceMgr.h>

namespace tinyc
{

class CodeGenVisitor: public ASTVisitor
{
public:
	CodeGenVisitor(llvm::LLVMContext& context, llvm::SourceMgr& src_mgr,
				   llvm::TargetMachine* tm);
	/// @note 只支持从根节点翻译
	[[nodiscard]]
	auto visit(BaseAST* ast) -> std::expected<void, std::string> override;

	/// @brief 在语法解析后获取结果
	[[nodiscard]]
	auto get_module() -> std::unique_ptr<llvm::Module>
	{ return std::move(m_module); }

private:
	using SymbolTable = std::unordered_map<std::string_view, llvm::Value*>;

	void handle(const CompUnit& node);
	void handle(const FuncDef& node);

	auto handle(const BuiltinType& node) -> llvm::Type*;
	auto handle(const ScalarType& node) -> llvm::Type*;

	auto handle(const ParamList& node) -> std::vector<llvm::Type*>;

	auto handle(const Block& node, llvm::Function* func,
				std::string_view block_name) -> llvm::BasicBlock*;
	
	auto handle(const Param& node) -> llvm::Type*;
	void handle(const Stmt& node);
	auto handle(const Expr& expr) -> llvm::Value*;
	auto handle(const PrimaryExpr& node) -> llvm::Value*;
	auto handle(const UnaryExpr& node) -> llvm::Value*;

	auto handle(const Number& num) -> llvm::Value*;
	auto handle(const Ident& node) -> std::string_view;

	auto handle(const Decl& node) -> llvm::Value*;
	auto handle(const ConstDecl& node) -> std::vector<llvm::Value*>; 
	auto handle(const ConstDef& node, llvm::Type* type) -> llvm::Value*;
	auto handle(const ConstDefList& node, llvm::Type* type)
		-> std::vector<llvm::Value*>;
	auto handle(const ConstInitVal& node) -> llvm::Value*;
	auto handle(const ConstExpr& node) -> llvm::Value*;
	auto handle(const LVal& node) -> llvm::Value*;

	/// @note 在上层会传入所有的BinaryExpr, 无需在实现文件中显式实例化声明
	template<typename BinaryExpr>
	auto handle(const BinaryExpr& node) -> llvm::Value*;

	/// @brief 一元运算符处理
	auto unary_operate(const UnaryOp& op, llvm::Value* operand) -> llvm::Value*;
	/// @brief 二元运算符通用处理函数
	auto binary_operate(llvm::Value* left, const Operator& op,
						llvm::Value* right) -> llvm::Value*;

private:
	std::unique_ptr<llvm::Module> m_module;
	llvm::IRBuilder<> m_builder;
	std::shared_ptr<CTypeManager> m_type_mgr;
	llvm::SourceMgr& m_src_mgr;

	llvm::TargetMachine* m_target_machine;
};

}	//namespace tinyc

