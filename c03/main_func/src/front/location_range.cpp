#include <llvm/Support/raw_ostream.h>
#include "location_range.hpp"

namespace tinyc
{

LocationRange::LocationRange()
{ }

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

void LocationRange::report(const llvm::SourceMgr& src_mgr,
					   llvm::SourceMgr::DiagKind dk,
					   std::string_view msg) const
{
	assert(begin.isValid());
	assert(src_mgr.getNumBuffers() > 0);
	auto range = get_range();
	count(dk);
	src_mgr.PrintMessage(begin, dk, msg, range);
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

auto LocationRange::search_counter(llvm::SourceMgr::DiagKind dk) -> std::size_t
{
	return LocationRange::trace_counter[dk];
}

void LocationRange::count(llvm::SourceMgr::DiagKind dk)
{
	++trace_counter[dk];
}

} 	//namespace tinyc

