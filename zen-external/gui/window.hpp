#pragma once
#include <base/no_copy.hpp>
#include <string>
#include <cstdint>
#include <windef.h>

namespace base {
	class rect_t;
}

namespace gui
{
	class window_t:
		base::no_copy_t
	{
	public:
		explicit window_t(const std::string& class_name);
		~window_t();
		bool create(window_t* parent, const std::string& window_name, const base::rect_t& size, uint32_t style, uint32_t ex_style = 0);
		void destroy();

		void move(const base::rect_t& point);

		void invalidate(bool repaint = false);

		operator HWND() const;
	private:
		std::string class_name_;
		HWND hwnd_;
	};
} // namespace gui
