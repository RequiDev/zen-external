#include "overlay.hpp"
#include "overlay_controller.hpp"
#include <base/point.hpp>
#include <chrono>
#include <thread>
#include <string>
#include <Dwmapi.h>
#include <comdef.h>

namespace drawing
{
	overlay_t* overlay_t::this_ = nullptr;

	overlay_t::overlay_t(const std::string& class_name, overlay_controller_t* ctrl) noexcept :
		class_name_(class_name),
		ovrly_ctrl_(ctrl),
		hwnd_(nullptr),
		target_hwnd_(nullptr)
	{
		this_ = this;
	}

	overlay_t::~overlay_t()
	{
		::DestroyWindow(hwnd_);
		::UnregisterClass(class_name_.c_str(), nullptr);
	}

	bool overlay_t::create(HWND target_hwnd)
	{
		target_hwnd_ = target_hwnd;
		if (!::GetWindowRect(target_hwnd_, &window_rect_))
			return false;

		int len = ::GetWindowTextLength(target_hwnd_);

		std::string window_name(len, 0);
		if (!::GetWindowText(target_hwnd_, window_name.data(), window_name.length()))
			return false;

		window_name.append(" Overlay");

		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof WNDCLASSEX;
		wc.lpfnWndProc = wnd_proc_thunk;
		wc.lpszClassName = class_name_.c_str();
		::RegisterClassEx(&wc);

		hwnd_ = ::CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, wc.lpszClassName, window_name.c_str(), WS_POPUP | WS_VISIBLE,
		                        window_rect_.left, window_rect_.top, window_rect_.width(), window_rect_.height(), nullptr,
		                        nullptr, nullptr, nullptr);

		if (!hwnd_)
			return false;

		if (!renderer_.create(hwnd_, renderer_t::device_type_t::d3d))
			return false;

		::SetLayeredWindowAttributes(hwnd_, 0, 255, LWA_ALPHA);
		::UpdateWindow(hwnd_);

		extend_frame_into_client_area();

		if (ovrly_ctrl_)
			ovrly_ctrl_->set_renderer(&renderer_);

		return true;
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