#pragma once
#include <expected>
#include <system_error>
#include <llvm/IR/Value.h>

namespace tinyc
{

namespace utils
{

enum class conversion_error
{
	none = 0,
#define CVT_KIND(kind, msg) \
	disable_##kind,
	#include "conversion.def"
#undef CVT_KIND
};


class conversion_category: public std::error_category
{
public:
	auto name() const noexcept -> const char*
	override
	{
		return "conversion_category";
	};

	auto message(int ev) const noexcept -> std::string
	override
	{
		switch(static_cast<conversion_error>(ev))
		{
		case conversion_error::none:
    	    return "No conversion error.";

#define CVT_KIND(kind, msg)                                                    \
	case conversion_error::disable_##kind:                                     \
		return msg "disabled";

#include "conversion.def"

#undef CVT_KIND

		default:
    	    return "Unknown conversion error.";
		};
	};
};

auto make_error_code(conversion_error e) -> std::error_code;

}	//namespace tinyc::utils

}	//namespace tinyc

namespace std
{

template <>
struct is_error_code_enum<tinyc::utils::conversion_error> : true_type
{
};

}	//namespace std

namespace tinyc
{

/**
 * @note 依据 https://en.cppreference.com/w/c/language/conversion
 * @brief 处理隐式转换
 */
class ImplicitConversionMgr
{
public:
	ImplicitConversionMgr();
	auto value_conversion(llvm::Value* right, llvm::Type* left_type) const
		-> std::expected<llvm::Value*, std::error_code>;

	auto arithmetic_conversion(llvm::Value* first, llvm::Value* second) const
		-> std::expected<llvm::Value*, std::error_code>;
	
	/// setter
#define CVT_KIND(kind, msg)                                                    \
	void set_##kind(bool enable)                                               \
	{                                                                          \
		m_enable_##kind = enable;                                              \
	}
#include "conversion.def"
#undef CVT_KIND
	/// getter
#define CVT_KIND(kind, msg)                                                    \
	auto get_##kind() const->bool                                              \
	{                                                                          \
		return m_enable_##kind;                                                \
	}
#include "conversion.def"
#undef CVT_KIND

private:
	auto arr2ptr(llvm::Value* value, llvm::Type* type) -> llvm::Value*;
	auto func2ptr(llvm::Value* value, llvm::Type* type) -> llvm::Value*;
	auto int_promotion(llvm::Value* value, llvm::Type* type) -> llvm::Value*;

private:
	/// 成员
#define CVT_KIND(kind, msg) \
	bool m_enable_##kind;
#include "conversion.def"
#undef CVT_KIND	

};

}	//tinyc
