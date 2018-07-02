#include "renderer.hpp"
#include <ctime>
#include <base/rect.hpp>
#include <drawing/device/dx9_device.hpp>
#include <winuser.h>

namespace drawing
{
	renderer_t::renderer_t() noexcept:
		hwnd_(nullptr),
		fps_data_(),
		frame_rate_(0)
	{
	}

	renderer_t::~renderer_t()
	{
		if (device_)
			device_->release();
	}

	bool renderer_t::create(HWND owner)
	{
		hwnd_ = owner;
		base::rect_t rc;
		if (!::GetWindowRect(hwnd_, &rc))
			return false;

		device_.reset(new dx9_device_t(hwnd_));

		return true;
	}

	void renderer_t::draw_text(const std::string& text, const base::point_t& point) const
	{
		device_->draw_text(text, point);
	}

	void renderer_t::begin_rendering()
	{
		device_->begin_scene();
	}

	void renderer_t::end_rendering()
	{
		++fps_data_.frames;
		float tick_count(clock() * 0.001f);
		if (tick_count - fps_data_.tick_count >= 1.0f)
		{
			fps_data_.tick_count = tick_count;
			fps_data_.fps = fps_data_.frames;
			fps_data_.frames = 0;
		}
		device_->end_scene();
	}

	int renderer_t::get_frame_rate() const
	{
		return fps_data_.fps;
	}

	bool renderer_t::reset_device()
	{
		// In case we get WM_SIZE but the renderer isn't even created yet
		if (!device_)
			return true;

		if (!device_->reset())
			return false;

		return true;
	}
} // namespace drawing