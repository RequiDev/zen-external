#pragma once
#include <base/auto_handle.hpp>

namespace remote
{
class process_t
{
public:
	bool attach(uint32_t process_id, uint32_t desired_access = PROCESS_ALL_ACCESS);

	template<typename T>
	bool read(uintptr_t address, T& val)
	{
		SIZE_T size_read;
		return !!::ReadProcessMemory(handle_, LPCVOID(address), &val, sizeof val, &size_read) && size_read > 0;
	}

	template<typename T>
	bool write(uintptr_t address, const T& val)
	{
		SIZE_T size_written;
		return !!::WriteProcessMemory(handle_, LPVOID(address), &val, sizeof val, &size_written) && size_written > 0;
	}


private:
	base::handle_t handle_;
};

} // namespace remote