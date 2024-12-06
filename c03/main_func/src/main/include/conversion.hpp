#pragma once
#include <llvm/IR/Value.h>

namespace tinyc
{

class CImplicitConversionMgr
{
public:
	auto conversion(llvm::Value* value) -> llvm::Value*;

	//  左值转换
	//  数组到指针转换
	//  函数到指针转换
	//  兼容类型
	//  整数提升
	//  布尔转换
	// 	整数转换
	// 	浮点整数转换
	//  浮点数转换
	//  指针转换

private:
	bool m_enable_lval_cvt;
	bool m_enable_arr2ptr_cvt;
	bool m_enable_func2ptr_cvt;
	static constexpr bool enable_compatible_types_cvt = true;
};

}	//tinyc
