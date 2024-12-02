#pragma once

#include "ast.hpp"

namespace tinyc
{

class LLVMLocation: public Location
{
	void report(Location::DiagKind kind, std::string_view msg) const override
	{
		(void)kind;
		(void)msg;
		std::println("LLVMLocation");
	};
};

static
auto operator<< (std::ostream& o, const LLVMLocation& loc) -> std::ostream&
{
	(void)loc;
	return o;
}

}	//namespace tinyc

