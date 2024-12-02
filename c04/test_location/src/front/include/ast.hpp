#pragma once
#include <memory>
#include <cassert>
#include <print>

namespace tinyc
{

/// @brief BaseAST依赖的位置抽象基类
class Location
{
public:
	int begin = 0;
	int end = 0;
	
	void step(){}
	void set_begin(const char* buf){(void)buf;}
	void set_end(const char* buf){(void)buf;}

	enum DiagKind
	{
		dk_error,
		dk_warning,
		dk_remark,
		dk_note,
	};

	virtual
	~Location() = default;
	
	virtual
	void report(Location::DiagKind kind, std::string_view msg) const = 0;
};


static
auto operator<< (std::ostream& o, const Location& loc) -> std::ostream&
{
	(void)loc;
	return o;
}

class BaseAST
{
};

class CompUnit: public BaseAST
{
public:
	CompUnit(std::unique_ptr<Location> location):
		m_location { std::move(location) }
	{}

	void report()
	{
		m_location->report(Location::dk_note, "");
	}

private:
	std::unique_ptr<Location> m_location;
};

} //namespace tinyc

