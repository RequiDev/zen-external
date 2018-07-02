#include "renderer.hpp"
#include <ctime>
#include <base/rect.hpp>
#include <drawing/device/dx9_device.hpp>

namespace drawing
{
	renderer_t::renderer_t() noexcept:
		fps_data_(),
		frame_rate_(0)
	{
	}

	renderer_t::~renderer_t()
	{
		if (device_tmp_)
			device_tmp_->release();
	}

	bool renderer_t::create(HWND owner)
	{
		base::rect_t rc;
		if (!::GetWindowRect(owner, &rc))
			return false;

		device_tmp_.reset(new dx9_device_t(owner));

		return true;
	}

	void renderer_t::draw_text(const std::string& text, const base::point_t& point) const
	{
		device_tmp_->draw_text(text, point);
	}

	void renderer_t::begin_rendering() const
	{
		device_tmp_->begin_scene();
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
		device_tmp_->end_scene();
	}

	int renderer_t::get_frame_rate() const
	{
		return fps_data_.fps;
	}

	bool renderer_t::reset_device()
	{
		// In case we get WM_SIZE but the renderer isn't even created yet
		if (!device_tmp_)
			return true;

		if (!device_tmp_->reset())
			return false;

		return true;
	}
} // namespace drawing