#pragma once
#include <string_view>
#include <expected>
#include <cstdio>
#include <unordered_set>

namespace tinyc
{

class FileManager
{
public:
	virtual
	~FileManager();

	[[nodiscard]]
	virtual 
	auto read_file(std::string_view file_name) -> std::expected<std::FILE*, std::string>;

	virtual
	void close_file(std::FILE* handle);

protected:
	/// 记录已经打开的句柄
	virtual
	auto insert_handle(std::FILE* handle) -> bool;

	/// 删除句柄记录
	virtual
	auto erase_handle(std::FILE* handle) -> bool;

private:
	std::unordered_set<std::FILE*> m_file_handles;
};

}	//namespace tinyc 
	
