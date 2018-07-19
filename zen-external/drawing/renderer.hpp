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
		friend class overlay_t;

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

		void draw_text(const std::string& text, const base::point_t& point) const;

		int get_frame_rate() const;
	private:
		bool create(HWND owner, device_type_t device_type);
		void begin_rendering();
		void end_rendering();
		bool reset_device();

		std::unique_ptr<device_t> device_;
		HWND hwnd_;
		fps_data_t fps_data_;
		int frame_rate_;
	};
} // namespace drawing