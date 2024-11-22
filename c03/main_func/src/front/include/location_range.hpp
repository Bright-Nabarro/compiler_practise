#pragma once
#include <llvm/Support/SMLoc.h>
#include <llvm/Support/SourceMgr.h>
#include <ostream>
#include <unordered_map>

#define YYLLOC_DEFAULT(Cur, Rhs, N)                                            \
	do                                                                         \
	{                                                                          \
		if (N)                                                                 \
		{                                                                      \
			Cur.begin = YYRHSLOC(Rhs, 1).begin;                                \
			Cur.end = YYRHSLOC(Rhs, N).end;                                    \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			Cur.begin = Cur.end = YYRHSLOC(Rhs, 0).end;                        \
		}                                                                      \
	} while (0)


namespace tinyc
{

/**
 * @brief location trace
 * @note 在flex, 不要将yytext赋值给begin和end \\
 * 使用yy_scan_bytes, yytext指向原始缓冲区副本
 * @note yy::parse要求必须有默认构造函数
 */
class LocationRange
{
public:
	explicit LocationRange();
	llvm::SMLoc begin;
	llvm::SMLoc end;

	/**
	 * @note 需要在初始化时调用
	 * @param buf llvm::SourceMgr的字符指针
	 */
	void set_begin(const char* buf);

	/**
	 * @note 需要在初始化时调用
	 * @param buf llvm::SourceMgr的字符指针
	 */
	void set_end(const char* buf);
	
	auto get_range() const -> llvm::SMRange;
	
	/**
	 * @param src_mgr begin和end对应的SourceMgr
	 * @param dk 错误级别
	 */
	void report(const llvm::SourceMgr& src_mgr,
					   llvm::SourceMgr::DiagKind dk,
					   std::string_view msg) const;
	/// @brief set begin to end
	void step();

	/// @brief end后移len字节
	void update(std::size_t len);

	/// @brief 查询某个级别输出信息的次数
	static
	auto search_counter(llvm::SourceMgr::DiagKind dk) -> std::size_t;

private:
	static inline
	std::unordered_map<llvm::SourceMgr::DiagKind, std::size_t> trace_counter;
	
	static
	void count(llvm::SourceMgr::DiagKind dk);
};

/// @brief 提供给flex trace使用
auto operator<< (std::ostream& o, const LocationRange& loc) -> std::ostream&;

}	//namespace tinyc

