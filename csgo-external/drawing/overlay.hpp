#pragma once
#include <windows.h>
#include <base/rect.hpp>
#include <drawing/renderer.hpp>
#include <Dwmapi.h>

namespace drawing
{
	class overlay_t
	{
	public:
		overlay_t() noexcept;
		~overlay_t();

		renderer_t* create(HWND child);
		int mainloop();
	private:
		LRESULT wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK wnd_proc_thunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static overlay_t* this_;

		renderer_t renderer_;
		HWND hwnd_; // TODO: Make an abstract window class
		HWND target_hwnd_;
		base::rect_t window_rect_;

		MARGINS margins_;
	};
} // namespace drawing