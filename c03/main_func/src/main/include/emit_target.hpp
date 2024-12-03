#pragma once
#include <llvm/IR/Module.h>
#include <expected>
#include <string>

namespace tinyc
{

class EmitTarget
{
public:
	enum TargetType
	{
		llvm_ir,
		llvm_bin,
		assembly,
		object,
	};

	EmitTarget();
	[[nodiscard]]
	auto operator()(std::unique_ptr<llvm::Module> module)
		-> std::expected<void, std::string>;
	[[nodiscard]]
	auto get_target_type() -> TargetType;
	
private:
	[[nodiscard]] static
	auto erase_file_postfix(std::string_view file_name) -> std::string_view;

	[[nodiscard]]
	auto get_target_name() -> std::string;

private:
	std::string_view m_inputfile_name;
	std::optional<std::string_view> m_target_name;
	bool m_emit_llvm;
};

}	//namespace tinyc
