#include <llvm/Support/raw_ostream.h>
#include <easylog.hpp>
#include "location_range.hpp"

namespace tinyc
{

LocationRange::LocationRange():
	Location{},
	begin {},
	end {},
	m_src_mgr { nullptr }
{ }

LocationRange::LocationRange(const LocationRange& rhs):
	Location {},
	begin { rhs.begin },
	end { rhs.end },
	m_src_mgr { rhs.m_src_mgr }
{
}

auto LocationRange::operator=(const LocationRange& rhs)
	-> LocationRange&
{
	auto tmp = rhs;
	std::swap(*this, tmp);
	return *this;
}

void LocationRange::set_begin(const char* buf)
{
	begin = llvm::SMLoc::getFromPointer(buf);
}

void LocationRange::set_end(const char* buf)
{
	end = llvm::SMLoc::getFromPointer(buf);
}

auto LocationRange::get_range() const -> llvm::SMRange
{
	return llvm::SMRange { begin, end };
}

void LocationRange::set_src_mgr(const llvm::SourceMgr* src_mgr)
{
	m_src_mgr = src_mgr;
}

void LocationRange::report(Location::DiagKind kind,
					   std::string_view msg) const
{
	assert(begin.isValid());
	assert(m_src_mgr != nullptr && "m_src_mgr uninitialized");
	assert(m_src_mgr->getNumBuffers() > 0);
	auto range = get_range();
	count(kind);
	m_src_mgr->PrintMessage(begin, cvt_kind_to_llvm(kind), msg, range);
}

void LocationRange::step()
{
	begin = end;
}

void LocationRange::update(std::size_t len)
{
	end = llvm::SMLoc::getFromPointer(end.getPointer() + len);
}

auto operator<< (std::ostream& os, const LocationRange& loc) -> std::ostream&
{
	const char* beg_str = loc.begin.getPointer();
	const char* end_str = loc.end.getPointer();

	assert(end_str - beg_str >= 0);

	os.write(beg_str, end_str - beg_str + 1);
	return os;
}

auto LocationRange::search_counter(Location::DiagKind kind) -> std::size_t
{
	return LocationRange::trace_counter[kind];
}

void LocationRange::count(Location::DiagKind kind)
{
	++trace_counter[kind];
}

constexpr
auto LocationRange::cvt_kind_to_llvm(Location::DiagKind kind)
	-> llvm::SourceMgr::DiagKind
{
	switch(kind)
	{	
	case Location::dk_error:
		return llvm::SourceMgr::DK_Error;
	case Location::dk_warning:
		return llvm::SourceMgr::DK_Warning;
	case Location::dk_remark:
		return llvm::SourceMgr::DK_Remark;
	case Location::dk_note:
		return llvm::SourceMgr::DK_Note;
	default:
		yq::error(yq::loc(), "unkown DiagKind");
	}
}

} 	//namespace tinyc

