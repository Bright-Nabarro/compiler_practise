#include "driver.hpp"

Driver::Driver():
	m_trace_parsing { false },
	m_trace_scanning { false }
{
}

auto Driver::parse(const std::string& filename) -> int
{
	m_filename = filename;
	m_location.initialize(&filename);
	scan_begin();
	yy::parser parse(*this);
	parse.set_debug_level(get_trace());
	int ret = parse();
	scan_end();
	return ret;
}

void Driver::insert(std::string key, int value)
{
	m_variables[std::move(key)] = value;
}


