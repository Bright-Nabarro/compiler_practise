#pragma once
#include <string_view>
#include <expected>
#include <cstdio>

namespace tinyc
{

class FileManager
{
public:
	virtual ~FileManager() = default;
	auto read_file_c(std::string_view file_name) -> std::expected<FILE*, std::string>;
};

}	//namespace tinyc 
	
