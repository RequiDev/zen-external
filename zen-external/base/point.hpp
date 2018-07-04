#pragma once
#include <windef.h>

namespace base
{
	class point_t : public POINT
	{
	public:
		point_t() = default;
		point_t(int x, int y);

		bool operator==(const POINT& rhs) const;
		bool operator!=(const POINT& rhs) const;
		point_t& operator=(const POINT& rhs);
	private:
	};
} // namespace base