#include <llvm/IR/Type.h>
#include "conversion.hpp"

namespace tinyc
{

namespace utils
{

auto make_error_code(conversion_error e) -> std::error_code
{
	static conversion_category instance;
	return { static_cast<int>(e), instance };
};

}	//namespace tinyc::utils

#define CVT_KIND(kind, msg) \
	m_enable_##kind { true },

#define CVT_KIND_END(kind, msg) \
	m_enable_##kind { true }

ImplicitConversionMgr::ImplicitConversionMgr():
#include "conversion.def"
{
}
#undef CVT_KIND
#undef CVT_KIND_END

auto ImplicitConversionMgr::value_conversion(llvm::Value* right,
											 llvm::Type* left_type) const
	-> std::expected<llvm::Value*, std::error_code>
{
	
}

auto ImplicitConversionMgr::arithmetic_conversion(llvm::Value* first,
												  llvm::Value* second) const
	-> std::expected<llvm::Value*, std::error_code>
{
}

auto ImplicitConversionMgr::int_promotion(llvm::Value* value, llvm::Type* type)
	-> llvm::Value*
{
	llvm::Type* value_type = value->getType();
	unsigned value_width = value_type->getIntegerBitWidth();
	unsigned target_width = type->getIntegerBitWidth();
}

}	//namespace tinyc
