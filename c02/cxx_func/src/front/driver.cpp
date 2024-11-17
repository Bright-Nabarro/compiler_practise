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
	auto handle = m_file_manager->read_file(file_name);
	if (!handle)
	{
		std::println(stderr, "{}", handle.error());
		return false;
	}
	m_location.initialize(&file_name);
	set_flex(handle.value());

	yy::parser parse(*this);
	parse.set_debug_level(this->get_trace());
	int parse_ret = parse();

	m_file_manager->close_file(handle.value());

	return parse_ret == 0;
}

}	//namespace tinyc

