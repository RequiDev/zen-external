#pragma once

namespace base
{
	class no_move_t
	{
	public:
		no_move_t() = default;

		no_move_t(no_move_t&&) = delete;
		no_move_t& operator=(no_move_t&&) = delete;
	};
} // namespace base
