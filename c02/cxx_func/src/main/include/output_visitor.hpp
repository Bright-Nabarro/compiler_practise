#pragma once

#include <easylog.hpp>
#include "ast.hpp"

namespace tinyc
{

class OutputVisitor: public ASTVisitor
{
public:
	OutputVisitor(bool enable_display);
	/// 只支持根节点输出
	auto visit(BaseAST* ast) -> bool;

private:
	void handle(const CompUnit& node);
	void handle(const FuncDef& node);
	void handle(const Type& node);
	void handle(const Ident& node);
	void handle(const ParamList& node);
	void handle(const Block& node);

private:
	yq::logger<yq::log_level::error> m_error_log;
	yq::logger<yq::log_level::debug> m_debug_log;
	
};

}	//namespace tinyc

