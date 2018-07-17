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
}
