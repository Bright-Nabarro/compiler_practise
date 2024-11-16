#pragma once

#include <string_view>
#include <memory>
#include "bison_parser.hpp"

#define YY_DECL \
	auto yylex(tinyc::Driver& driver) -> yy::parser::symbol_type;

namespace tinyc
{

class Driver
{
public:
	Driver();
	auto parse(std::string_view file_name) -> bool;
	auto get_location() -> yy::location&;

private:
};

}	//namespace tinyc
