#pragma once

#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/SMLoc.h>
#include <llvm/Support/MemoryBuffer.h>
#include <string_view>
#include <expected>
#include <memory>
#include "ast.hpp"
#include "bison_parser.hpp"
//#include "file_manager.hpp"

#define YY_DECL \
	auto yylex(tinyc::Driver& driver) -> yy::parser::symbol_type

YY_DECL;

namespace tinyc
{

class Driver
{
	friend class DriverFactory;
private:
	Driver(llvm::SourceMgr& src_mgr);

public:
	auto construct(std::string_view file_name)
		-> std::expected<void, std::string>;
	auto parse() -> bool;
	
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
	
	void report_error(const char* buf_pos);

private:
	auto get_buffer() -> const char*;

private:
	std::unique_ptr<CompUnit> m_ast;
	llvm::SourceMgr& m_src_mgr;
	unsigned m_bufferid;
	bool m_debug_trace;
	std::unique_ptr<yy::parser> m_parser;
};


class DriverFactory
{
public:
	DriverFactory(llvm::SourceMgr& src_mgr):
		m_src_mgr { src_mgr }
	{}

	auto produce_driver(std::string_view file_name)
		-> std::expected<Driver, std::string>;

private:
	llvm::SourceMgr& m_src_mgr;
};


}	//namespace tinyc


