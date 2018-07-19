#pragma once
#include <string>

namespace base
{
	extern std::string wide_to_narrow(const std::wstring& str);
	extern std::wstring narrow_to_wide(const std::string& str);
} // namespace base
