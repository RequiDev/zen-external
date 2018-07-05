#pragma once
#include "device.hpp"
#include <windef.h>

struct ID2D1HwndRenderTarget;
struct IDWriteFactory;
struct ID2D1Factory;
struct ID2D1SolidColorBrush;
struct IDWriteTextFormat;

namespace drawing
{
	class d2d_device_t:
		public device_t
	{
	public:
		explicit d2d_device_t();
		virtual ~d2d_device_t();

		virtual bool create(HWND owner);

		virtual void release();
		virtual void begin_scene();
		virtual void end_scene();
		virtual bool reset();

		virtual void draw_text(const std::string& text, const base::point_t& pt);
	private:
		bool create_device_objects();
		void destroy_device_objects();

		HWND owner_;

		ID2D1Factory* factory_;
		ID2D1HwndRenderTarget* render_target_;
		IDWriteFactory* write_factory_;
		IDWriteTextFormat* font_;
		ID2D1SolidColorBrush* color_brush_;
	};
} // namespace drawing