#include "driver.hpp"

#include <print>
#include "file_manager.hpp"

namespace tinyc
{

Driver::Driver():
	m_file_manager { std::make_shared<FileManager>() },
	m_ast { nullptr },
	m_location {},
	m_debug_trace { false }
{}

auto Driver::parse(std::string_view file_name) -> bool
{
	auto ret = m_file_manager->read_file_c(file_name);
	if (!ret)
	{
		std::println(stderr, "{}", ret.error());
		return false;
	}
	m_location.initialize(&file_name);
	scan_begin();
	yy::parser parse(*this);
	parse.set_debug_level(this->get_trace());
	int parse_ret = parse();
	scan_end();

	return parse_ret == 0;
}

}	//namespace tinyc
