#include "cheat.hpp"
#include <thread>
#include <iostream>

namespace cheat
{
	cheat_t::cheat_t(const std::string& game, const std::string& class_name):
		renderer_(nullptr),
		game_(game),
		class_name_(class_name)
	{
	}

	int cheat_t::mainloop()
	{
		std::cout << "Initializing cheat for " << game_ << std::endl;
		HWND hwnd(::FindWindow(class_name_.data(), nullptr));
		if (!hwnd)
			return 1000;

		renderer_ = overlay_.create(hwnd);
		if (!renderer_)
			return 1001;

		DWORD process_id;
		::GetWindowThreadProcessId(hwnd, &process_id);

		if (!process_.attach(process_id))
			return 1002;

		return overlay_.mainloop();
	}
} // namespace cheat