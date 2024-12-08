#pragma once
#include <expected>
#include <llvm/IR/Value.h>

namespace tinyc
{

/**
 * @note 依据 https://en.cppreference.com/w/c/language/conversion
 * @brief 处理隐式转换
 */
class CImplicitConversionMgr
{
public:
	CImplicitConversionMgr();
	auto conversion(llvm::Value* value, llvm::Value* desire)
		-> std::expected<llvm::Value*, std::string>;
	
	///  左值转换
	void set_lval_cvt(bool enable);
	///  数组到指针转换
	void set_arr2ptr_cvt(bool enable);
	///  函数到指针转换
	void set_func2ptr_cvt(bool enable);
	///  兼容类型
	void set_compatible_types_cvt(bool enable);
	///  整数提升
	void set_int_promotion(bool enable);
	///  布尔转换
	void set_bool_cvt(bool enable);
	/// 整数转换
	void set_int_cvt(bool enable);
	/// 浮点整数转换
	void set_float2int_cvt(bool enable);
	///  浮点数转换
	void set_float_cvt(bool enable);
	///  指针转换
	void set_voidpointer_cvt(bool enable);
	

private:
	bool m_enable_lval_cvt;
	bool m_enable_arr2ptr_cvt;
	bool m_enable_func2ptr_cvt;
	bool m_enable_compatible_types_cvt;
	bool m_enable_int_promotion;
	bool m_enable_bool_cvt;
	bool m_enable_int_cvt;
	bool m_enable_float2int_cvt;
	bool m_enable_float_cvt;
	bool m_enable_voidpointer_cvt;

};

}	//tinyc
