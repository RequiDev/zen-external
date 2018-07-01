#include "process.hpp"

namespace remote
{
	bool process_t::attach(uint32_t process_id, uint32_t desired_access)
	{
		handle_.reset(::OpenProcess(desired_access, 0, process_id));
		return handle_;
	}
} // namespace remote