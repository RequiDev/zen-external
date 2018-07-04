#pragma once
#include <string>
#include <windef.h>

namespace base
{
	class point_t;
} // namespace base

namespace drawing
{
	class device_t
	{
	public:
		virtual ~device_t() {}

		virtual bool create(HWND owner) = 0;

		virtual void release() = 0;
		virtual void begin_scene() = 0;
		virtual void end_scene() = 0;
		virtual bool reset() = 0;

		virtual void draw_text(const std::string& text, const base::point_t& pt) {}
	private:
	};
} // namespace drawing