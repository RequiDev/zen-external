#include "rect.hpp"
#include <base/point.hpp>
#include <cstdlib>

namespace base
{
	rect_t::rect_t(const RECT& rc):
		RECT(rc)
	{
	}

	rect_t::rect_t(const point_t& pt)
	{
		left = pt.x;
		top = pt.y;
		right = 0;
		bottom = 0;
	}

	int rect_t::width() const
	{
		return abs(right - left);
	}

	int rect_t::height() const
	{
		return abs(bottom - top);
	}

	bool rect_t::operator==(const RECT& rhs) const
	{
		return left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom;
	}

	bool rect_t::operator!=(const RECT& rhs) const
	{
		return !(*this == rhs);
	}
} // namespace base