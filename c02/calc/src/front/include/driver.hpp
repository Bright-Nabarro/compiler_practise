#pragma once

#include <string>
#include <unordered_map>
#include "bison_parser.hpp"

// yylex signature
#define YY_DECL \
	auto yylex(Driver& driver) -> yy::parser::symbol_type

//declare yylex
YY_DECL;

class Driver
{
public:
	Driver();
	auto parse(const std::string& filename) -> int;
	void insert(std::string key, int value);
	auto find(const std::string& key) -> int
	{ return m_variables[key]; }

	auto get_location() -> yy::location&
	{ return m_location; }

	void scan_begin();
	void scan_end();

	void set_trace(bool value)
	{ m_trace_parsing = value; }

	auto get_trace() -> bool
	{ return m_trace_parsing; }

	void set_result(int value)
	{ m_result = value; }

	auto get_result() -> int
	{ return m_result; }

	auto get_filename() const -> std::string
	{ return m_filename; }


private:
	std::unordered_map<std::string, int> m_variables;
	int m_result;
	std::string m_filename;
	yy::location m_location;
	bool m_trace_parsing;
	bool m_trace_scanning;
};



