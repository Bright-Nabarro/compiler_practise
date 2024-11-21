#include "driver.hpp"

#include <format>
#include <llvm/Support/WithColor.h>
#include "file_manager.hpp"

namespace tinyc
{

Driver::Driver(llvm::SourceMgr& src_mgr):
	m_ast {},
	m_src_mgr { src_mgr },
	m_bufferid {},
	m_debug_trace { false },
	m_parser {}
{
	
}

auto Driver::construct(std::string_view file_name)
	-> std::expected<void, std::string>
{
	auto buffer_or_error = llvm::MemoryBuffer::getFile(file_name);
	if (!buffer_or_error)
	{
		return std::unexpected{std::format("Failed to open {} \n", file_name)};
	}
	m_bufferid = m_src_mgr.AddNewSourceBuffer(std::move(*buffer_or_error), llvm::SMLoc());

	m_parser = std::make_unique<yy::parser>(*this);

	return {};
}

auto Driver::get_buffer() -> const char*
{
	return m_src_mgr.getMemoryBuffer(m_bufferid)->getBufferStart();
}

//auto Driver::parse(std::string_view file_name) -> bool
//{
//	auto handle = m_file_manager->read_file(file_name);
//	if (!handle)
//	{
//		std::println(stderr, "{}", handle.error());
//		return false;
//	}
//	m_location.initialize(&file_name);
//	set_flex(handle.value());
//
//	yy::parser parse(*this);
//	parse.set_debug_level(this->get_trace());
//	int parse_ret = parse();
//
//	m_file_manager->close_file(handle.value());
//
//	if (parse_ret != 0)
//	{
//		return false;
//	}
//
//	return true;
//}

auto Driver::parse() -> bool
{
	m_parser->set_debug_level(this->get_trace());
	int parse_ret = (*m_parser)();

	return parse_ret == 0;
	
}

}	//namespace tinyc

