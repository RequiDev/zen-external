#pragma once
#include <base/rect.hpp>
#include <drawing/renderer.hpp>

namespace drawing
{
	class overlay_controller_t;

	class overlay_t
	{
	public:
		explicit overlay_t(const std::string& class_name,overlay_controller_t* ctrl = nullptr) noexcept;
		~overlay_t();

		bool create(HWND target_hwnd);
		int mainloop();
	private:
		void extend_frame_into_client_area() const;
		bool ensure_window_size();
		bool is_target_alive() const;

		LRESULT wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK wnd_proc_thunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
		static overlay_t* this_;

		std::string class_name_;
		overlay_controller_t* ovrly_ctrl_;

		renderer_t renderer_;
		HWND hwnd_; // TODO: Make an abstract window class
		HWND target_hwnd_;
		base::rect_t window_rect_;

	};
} // namespace drawing