#include "process.hpp"
#include <memoryapi.h>
#include <processthreadsapi.h>
#include <winnls.h>

namespace remote
{
	class module_t;

	process_t::process_t():
		peb_(this)
	{
	}

	process_t::process_t(HANDLE handle):
		handle_(handle),
		peb_(this)
	{
	}

	bool process_t::attach(uint32_t process_id, uint32_t desired_access)
	{
		handle_.reset(::OpenProcess(desired_access, 0, process_id));
		if (!handle_)
			return false;

		if (!peb_.reset())
			return false;
		
		return true;
	}

	bool process_t::read_memory(uintptr_t address, LPVOID buffer, SIZE_T size) const
	{
		SIZE_T size_read;
		return !!::ReadProcessMemory(handle_, LPCVOID(address), buffer, size, &size_read) && size_read > 0;
	}

	bool process_t::write_memory(uintptr_t address, LPCVOID buffer, SIZE_T size) const
	{
		SIZE_T size_written;
		return !!::WriteProcessMemory(handle_, LPVOID(address), buffer, size, &size_written) && size_written > 0;
	}

	std::string process_t::read_unicode_string(const UNICODE_STRING& unicode) const
	{
		std::unique_ptr<wchar_t[]> buffer(std::make_unique<wchar_t[]>(unicode.Length));
		if (!read_memory(uintptr_t(unicode.Buffer), buffer.get(), unicode.Length))
			return "";

		int size = WideCharToMultiByte(CP_UTF8, 0, buffer.get(), unicode.Length, nullptr, 0, nullptr, nullptr);
		std::unique_ptr<char[]> ret(std::make_unique<char[]>(size));
		WideCharToMultiByte(CP_UTF8, 0, buffer.get(), unicode.Length, ret.get(), size, nullptr, nullptr);

		return ret.get();
	}

	uint32_t process_t::granted_access() const
	{
		OBJECT_BASIC_INFORMATION info;
		if (!query_object(info, ObjectBasicInformation))
			return 0;

		return info.DesiredAccess;
	}

	bool process_t::get_module(const char* str, module_t& module_out)
	{
		return peb_.get_module(str, module_out);
	}
} // namespace remote