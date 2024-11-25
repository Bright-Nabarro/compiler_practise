#pragma once

#include <string_view>
#include <memory>
#include "ast.hpp"
#include "bison_parser.hpp"
#include "file_manager.hpp"

#define YY_DECL \
	auto yylex(tinyc::Driver& driver) -> yy::parser::symbol_type

YY_DECL;

namespace tinyc
{

void report_error(yy::parser::location_type loc, const std::string& m);

class Driver
{
public:
	Driver();
	auto parse(std::string_view file_name) -> bool;
	auto get_location() -> yy::location&
	{ return m_location; }
	
	//CompUnit为根节点
	void set_ast(std::unique_ptr<CompUnit> ast)
	{ m_ast = std::move(ast); }
	auto get_ast() const -> const CompUnit&
	{ return *m_ast; }
	auto get_ast_ptr() -> CompUnit*
	{ return m_ast.get(); }

	//设置是否输出debug调用栈
	void set_trace(bool debug_trace)
	{ m_debug_trace = debug_trace; }
	auto get_trace() -> bool
	{ return m_debug_trace; }
	
	void flex_fatal_error(std::string_view msg);

private:
	void set_flex(std::FILE* handle);

private:
	std::shared_ptr<FileManager> m_file_manager;
	std::unique_ptr<CompUnit> m_ast;
	yy::location m_location;
	bool m_debug_trace;
};

}	//namespace tinyc


