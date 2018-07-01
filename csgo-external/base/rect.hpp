#pragma once
#include <windows.h>

namespace base
{
	class point_t;

	class rect_t : public RECT
	{
	public:
		rect_t() = default;
		explicit rect_t(const point_t& pt);

		int width() const;
		int height() const;

		bool operator==(const RECT& rhs) const;
		bool operator!=(const RECT& rhs) const;
		rect_t& operator=(const RECT& rc);
	private:
	};
} // namespace base