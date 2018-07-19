#pragma once

namespace base
{
	class no_copy_t
	{
	public:
		no_copy_t() = default;

		no_copy_t(const no_copy_t&) = delete;
		no_copy_t& operator=(const no_copy_t&) = delete;
	};
} // namespace base
