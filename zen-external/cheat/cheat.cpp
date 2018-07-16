#include "cheat.hpp"
#include <thread>
#include <iostream>
#include <winuser.h>

namespace cheat
{
	enum
	{
		WINDOW_NOT_FOUND = 1000,
		PROCESS_NOT_ATTACHED,
		OVERLAY_NOT_CREATED
	};

	cheat_t::cheat_t(const std::string& game, const std::string& class_name):
		overlay_(this),
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
			return WINDOW_NOT_FOUND;

		DWORD process_id;
		::GetWindowThreadProcessId(hwnd, &process_id);

		if (!process_.attach(process_id))
			return PROCESS_NOT_ATTACHED;

		// only create overlay, if we for sure initialized correctly.
		if (!overlay_.create(hwnd))
			return OVERLAY_NOT_CREATED;

		return overlay_.mainloop();
	}

	void cheat_t::set_renderer(drawing::renderer_t* renderer)
	{
		renderer_ = renderer;
	}

	bool cheat_t::read_memory(uintptr_t address, void* buffer, size_t size)
	{
		return process_.read_memory(address, buffer, size);
	}
} // namespace cheat