#include "file_manager.hpp"
#include <cerrno>
#include <cstring>
#include <format>
#include <string>

namespace tinyc
{


FileManager::~FileManager()
{
	for (auto handle: m_file_handles)
	{
		std::fclose(handle);
	}
}

auto FileManager::read_file(std::string_view file_name) -> std::expected<FILE*, std::string>
{
	if (file_name.empty())
		return stdin;
	auto handle = std::fopen(file_name.data(), "r");
	if (handle == nullptr)
		return std::unexpected(std::format("cannot open {}: {}", file_name.data(), std::strerror(errno)));
	
	insert_handle(handle);
	return handle;
}

void FileManager::close_file(std::FILE* handle)
{
	if (erase_handle(handle))
		std::fclose(handle);
}

auto FileManager::insert_handle(std::FILE* handle) -> bool
{
	return m_file_handles.insert(handle).second;
}

auto FileManager::erase_handle(std::FILE* handle) -> bool
{
	return m_file_handles.erase(handle) > 0;
}

}	//namespace tinyc
