#pragma once

namespace base
{
	class no_copy_t
	{
	public:
		no_copy_t() = default;

		no_copy_t(const no_copy_t&) = delete; // non construction-copyable
		no_copy_t& operator=(const no_copy_t&) = delete; // non copyable

		no_copy_t(no_copy_t&&) = delete;
		no_copy_t& operator=(no_copy_t&&) = delete;
	};
}
