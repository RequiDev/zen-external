#include "window.hpp"
#include <winuser.h>
#include <base/rect.hpp>

namespace gui
{
	window_t::window_t(const std::string& class_name):
		class_name_(class_name),
		hwnd_(nullptr)
	{
	}

	window_t::~window_t()
	{
		destroy();
	}

	bool window_t::create(window_t* parent, const std::string& window_name, const base::rect_t& size, uint32_t style, uint32_t ex_style)
	{
		hwnd_ = CreateWindowEx(ex_style, class_name_.c_str(), window_name.c_str(), style, size.left, size.top, size.width(), size.height(), *parent, nullptr, nullptr, nullptr);
		if (!hwnd_)
			return false;
		return true;
	}

	void window_t::destroy()
	{
		DestroyWindow(hwnd_);
	}

	void window_t::move(const base::rect_t& rect)
	{
		MoveWindow(hwnd_, rect.left, rect.top, rect.height(), rect.width(), false);
	}

	void window_t::invalidate(bool repaint)
	{
		RECT rc;
		InvalidateRect(hwnd_, &rc, repaint);
	}

	window_t::operator HWND() const
	{
		return hwnd_;
	}
} // namespace gui
