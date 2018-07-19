#include "point.hpp"

namespace base
{
	point_t::point_t(const POINT& pt):
		POINT(pt)
	{
	}

	point_t::point_t(int x, int y)
	{
		this->x = x;
		this->y = y;
	}

	bool point_t::operator==(const POINT& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	bool point_t::operator!=(const POINT& rhs) const
	{
		return !(*this == rhs);
	}
} // namespace base