#include "overlay.hpp"
#include <chrono>
#include <thread>
#include <base/point.hpp>

namespace drawing
{
	overlay_t* overlay_t::this_ = nullptr;

	overlay_t::overlay_t() noexcept:
		hwnd_(nullptr),
		target_hwnd_(nullptr),
		margins_()
	{
		this_ = this;
	}

	overlay_t::~overlay_t()
	{
		::UnregisterClass("csgo_external", GetModuleHandle(nullptr));
	}

	renderer_t* overlay_t::create(HWND child)
	{
		if (!::GetWindowRect(child, &window_rect_))
			return nullptr;

		target_hwnd_ = child;
		margins_ = { 0, 0, window_rect_.width(), window_rect_.height() };

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

		::SetLayeredWindowAttributes(hwnd_, 0, 255, 0x2);
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
			}

			RECT rc;
			if (!::GetWindowRect(target_hwnd_, &rc))
				break;

			if (window_rect_ != rc)
			{
				window_rect_ = rc;
				margins_ = { 0, 0, window_rect_.width(), window_rect_.height() };
				::MoveWindow(hwnd_, rc.left, rc.top, window_rect_.width(), window_rect_.height(), true);
				extend_frame_into_client_area();
			}

			renderer_.begin_rendering();

			renderer_.draw_text(std::to_string(renderer_.get_frame_rate()) + " FPS", { 10, 10 });

			renderer_.end_rendering();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		return EXIT_SUCCESS;
	}

	void overlay_t::extend_frame_into_client_area() const
	{
		MARGINS margins = { -1 };
		DwmExtendFrameIntoClientArea(hwnd_, &margins);
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
			default:
				break;
		}

		return::DefWindowProc(hwnd, message, wParam, lParam);
	}

	LRESULT CALLBACK overlay_t::wnd_proc_thunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!this_)
			return 1;
		return this_->wnd_proc(hwnd, message, wParam, lParam);
	}
} // namespace drawing