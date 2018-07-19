#include "string_util.hpp"
#include <winnls.h>

namespace base
{
	std::string wide_to_narrow(const std::wstring& str)
	{
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, str.data(), str.length(), nullptr, 0, nullptr, nullptr);
		std::string str_to(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), str.length(), str_to.data(), size_needed, nullptr, nullptr);
		return str_to;
	}

	std::wstring narrow_to_wide(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), nullptr, 0);
		std::wstring wstr_to(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), wstr_to.data(), size_needed);
		return wstr_to;
	}
} // namespace base
