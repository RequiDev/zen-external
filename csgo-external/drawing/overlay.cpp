#include "overlay.hpp"
#include <base/point.hpp>
#include <chrono>
#include <thread>
#include <string>
#include <Dwmapi.h>
#include <comdef.h>

namespace drawing
{
	overlay_t* overlay_t::this_ = nullptr;

	overlay_t::overlay_t() noexcept:
		hwnd_(nullptr),
		target_hwnd_(nullptr)
	{
		this_ = this;
	}

	overlay_t::~overlay_t()
	{
		::UnregisterClass("csgo_external", nullptr);
	}

	renderer_t* overlay_t::create(HWND target_hwnd)
	{
		if (!::GetWindowRect(target_hwnd, &window_rect_))
			return nullptr;

		target_hwnd_ = target_hwnd;

		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0;
		wc.lpfnWndProc = wnd_proc_thunk;
		wc.hInstance = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszClassName = "csgo_external";

		::RegisterClassEx(&wc);

		hwnd_ = ::CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, wc.lpszClassName, "csgo_external", WS_POPUP | WS_VISIBLE,
		                        window_rect_.left, window_rect_.top, window_rect_.width(), window_rect_.height(), nullptr,
		                        nullptr, nullptr, nullptr);

		if (!hwnd_)
			return nullptr;

		if (!renderer_.create(hwnd_))
			return nullptr;

		::SetLayeredWindowAttributes(hwnd_, 0, 255, LWA_ALPHA);
		::UpdateWindow(hwnd_);

		extend_frame_into_client_area();

		return &renderer_;
	}

	int overlay_t::mainloop()
	{
		MSG msg = { nullptr };
		while (msg.message != WM_QUIT)
		{
			while (::PeekMessage(&msg, hwnd_, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);

				if (msg.message == WM_QUIT)
					break;

				if (!ensure_window_size())
					PostMessage(hwnd_, WM_QUIT, 0, 0);

				// not sure about this one
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		return EXIT_SUCCESS;
	}

	void overlay_t::extend_frame_into_client_area() const
	{
		MARGINS margins = { -1 };
		::DwmExtendFrameIntoClientArea(hwnd_, &margins);
	}

	bool overlay_t::ensure_window_size()
	{
		if (!is_target_alive())
			return false;

		RECT rc;
		if (!::GetWindowRect(target_hwnd_, &rc))
			return false;

		if (window_rect_ != rc)
		{
			window_rect_ = rc;
			::MoveWindow(hwnd_, rc.left, rc.top, window_rect_.width(), window_rect_.height(), true);
			extend_frame_into_client_area();
		}
		return true;
	}

	bool overlay_t::is_target_alive() const
	{
		return ::IsWindow(target_hwnd_);
	}

	LRESULT overlay_t::wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_SIZE:
				if (!renderer_.reset_device())
					throw std::runtime_error("Could not reset device.");
				break;
			case WM_DWMCOMPOSITIONCHANGED: // windows 7 support (thanks YatoDev)
				extend_frame_into_client_area();
				return 0;
			case WM_PAINT:
				renderer_.begin_rendering();
				renderer_.draw_text(std::to_string(renderer_.get_frame_rate()) + " FPS", { 10, 10 });
				renderer_.end_rendering();
				return 0;
			default:
				break;
		}

		return ::DefWindowProc(hwnd, message, wParam, lParam);
	}

	LRESULT CALLBACK overlay_t::wnd_proc_thunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!this_)
			return ::DefWindowProc(hwnd, message, wParam, lParam);
		return this_->wnd_proc(hwnd, message, wParam, lParam);
	}
} // namespace drawing