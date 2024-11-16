#include "file_manager.hpp"
#include <cerrno>
#include <cstring>
#include <format>
#include <string>

namespace tinyc
{

auto FileManager::read_file_c(std::string_view file_name) -> std::expected<FILE*, std::string>
{
	if (file_name.empty())
		return stdin;
	auto ret = std::fopen(file_name.data(), "r");
	if (ret == nullptr)
		return std::unexpected(std::format("cannot open {}: {}", file_name.data(), std::strerror(errno)));

	return ret;
}

}	//namespace tinyc
