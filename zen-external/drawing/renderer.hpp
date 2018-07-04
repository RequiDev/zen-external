#pragma once
#include <windef.h>
#include <string>
#include <drawing/device/device.hpp>

namespace base
{
	class point_t;
} // namespace base

namespace drawing
{
	class renderer_t
	{
		struct fps_data_t
		{
			int frames;
			int fps;
			float tick_count;
		};
	public:
		// TODO: Add multiple device types
		enum device_type_t
		{
			d2d,
			d3d
		};

		renderer_t() noexcept;
		~renderer_t();

		bool create(HWND owner);
		void draw_text(const std::string& text, const base::point_t& point) const;
		void begin_rendering();
		void end_rendering();

		int get_frame_rate() const;
		bool reset_device();
	private:

		std::unique_ptr<device_t> device_;
		HWND hwnd_;
		fps_data_t fps_data_;
		int frame_rate_;
	};
} // namespace drawing