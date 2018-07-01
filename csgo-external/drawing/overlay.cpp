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
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = wnd_proc_thunk;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = RGB(0, 0, 0);
		wc.lpszClassName = "csgo_external";
		::RegisterClassEx(&wc);

		hwnd_ = ::CreateWindowExA(WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, "csgo_external", WS_POPUP | WS_VISIBLE,
		                        window_rect_.left, window_rect_.top, window_rect_.width(), window_rect_.height(), nullptr,
		                        nullptr, wc.hInstance, nullptr);

		::SetLayeredWindowAttributes(hwnd_, RGB(0, 0, 0), 0, ULW_COLORKEY);
		::SetLayeredWindowAttributes(hwnd_, 0, 255, LWA_ALPHA);

		if (!hwnd_)
			return nullptr;

		if (!renderer_.create(hwnd_))
			return nullptr;

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
				::DwmExtendFrameIntoClientArea(hwnd_, &margins_);
			}

			// keep our window between game and everything else
			::SetWindowPos(target_hwnd_, hwnd_, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

			renderer_.begin_rendering();

			renderer_.draw_text(std::to_string(renderer_.get_frame_rate()) + " FPS", { 10, 10 });

			renderer_.end_rendering();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		return EXIT_SUCCESS;
	}

	LRESULT overlay_t::wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_CREATE:
				::DwmExtendFrameIntoClientArea(hwnd, &margins_);
				break;
			case WM_SIZE:
				if (!renderer_.reset_device())
					throw std::runtime_error("Could not reset device.");
				break;
			default:
				return ::DefWindowProc(hwnd, message, wParam, lParam);
		}

		return 1;
	}

	LRESULT CALLBACK overlay_t::wnd_proc_thunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!this_)
			return 1;
		return this_->wnd_proc(hwnd, message, wParam, lParam);
	}
} // namespace drawing