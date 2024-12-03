#include <llvm/CodeGen/CommandFlags.h>
#include "emit_target.hpp"

namespace tinyc
{

auto EmitTarget::operator()(std::unique_ptr<llvm::Module> module)
	-> std::expected<void, std::string>
{
}

auto EmitTarget::get_target_type() -> TargetType
{
	auto file_type = llvm::codegen::getFileType();
	switch(file_type)
	{
	case llvm::CodeGenFileType::AssemblyFile:
		if (m_emit_llvm)
			return llvm_ir;
		return assembly;
	case llvm::CodeGenFileType::ObjectFile:
		if (m_emit_llvm)
			return llvm_bin;
		return object;
	case llvm::CodeGenFileType::Null:
		if (m_emit_llvm)
			return llvm_ir;
		return object;
	}
}

auto EmitTarget::erase_file_postfix(std::string_view file_name)
	-> std::string_view
{
	std::size_t pos = file_name.rfind('.');
	if (pos == std::string_view::npos)
		return file_name;
	
	return file_name.substr(0, pos);
}

auto EmitTarget::get_target_name() -> std::string
{
	if (m_target_name.has_value())
		return std::string { m_target_name.value() };

	std::string result { erase_file_postfix(m_inputfile_name) };

	switch(get_target_type())
	{
	case llvm_ir:
		result += ".ll";
		break;
	case llvm_bin:
		result += ".bc";
		break;
	case assembly:
		result += ".s";
		break;
	case object:
		result += ".o";
		break;
	}

	return result;
}

}	//namespace tinyc
