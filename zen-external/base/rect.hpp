#pragma once
#include <windef.h>

namespace base
{
	class point_t;

	class rect_t : public RECT
	{
	public:
		rect_t() = default;
		rect_t(const RECT& rc);
		explicit rect_t(const point_t& pt);

		int width() const;
		int height() const;

		bool operator==(const RECT& rhs) const;
		bool operator!=(const RECT& rhs) const;
	private:
	};
} // namespace base