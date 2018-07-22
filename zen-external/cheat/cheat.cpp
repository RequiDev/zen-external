#include "cheat.hpp"
#include <thread>
#include <iostream>
#include <winuser.h>
#include <sstream>

namespace cheat
{
	enum
	{
		WINDOW_NOT_FOUND = 1000,
		PROCESS_NOT_ATTACHED,
		OVERLAY_NOT_CREATED
	};

	cheat_t::cheat_t(const std::string& game, const std::string& class_name, const std::string& overlay_class_name) :
		last_error_(0),
		overlay_(overlay_class_name, this),
		renderer_(nullptr),
		game_(game),
		class_name_(class_name)
	{
	}

	bool cheat_t::init()
	{
		std::cout << "Initializing cheat for " << game_ << std::endl;
		HWND hwnd(::FindWindow(class_name_.data(), nullptr));
		if (!hwnd)
		{
			last_error_ = WINDOW_NOT_FOUND;
			return false;
		}

		DWORD process_id;
		::GetWindowThreadProcessId(hwnd, &process_id);

		if (!process_.attach(process_id))
		{
			last_error_ = PROCESS_NOT_ATTACHED;
			return false;
		}

		// only create overlay, if we for sure initialized correctly.
		if (!overlay_.create(hwnd))
		{
			last_error_ = OVERLAY_NOT_CREATED;
			return false;
		}

		return true;
	}

	int cheat_t::mainloop()
	{
		return overlay_.mainloop();
	}

	int cheat_t::operator()()
	{
		if (!init())
			return last_error_;
		return mainloop();
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