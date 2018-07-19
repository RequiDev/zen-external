#include "cheat.hpp"
#include <thread>
#include <iostream>
#include <winuser.h>
#include <sstream>

namespace cheat
{
	std::string error_codes[] =
	{
		"Could not find game window.",
		"Could not attach to game process.",
		"Could not create overlay."
	};


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

		int return_code =  overlay_.mainloop();

		if (return_code != EXIT_SUCCESS)
		{
			std::string error_message;

			int last_error(base::get_last_error());
			HRESULT last_hresult(base::get_last_hresult());
			NTSTATUS last_ntstatus(base::get_last_ntstatus());
			long relevant_code(0);
			if (last_error)
			{
				error_message = base::get_error_message(last_error);
				relevant_code = last_error;
			}
			else if (last_hresult)
			{
				error_message = base::get_error_message(last_hresult);
				relevant_code = last_hresult;
			}
			else if (last_ntstatus)
			{
				error_message = base::get_nt_message(last_ntstatus);
				relevant_code = last_ntstatus;
			}
			std::stringstream ss;
			if (!error_message.empty())
			{
				ss << "\n(";
				ss << "0x" << std::hex << relevant_code;
				ss << ") ";
			}

			std::cout << "Something went wrong: " << error_codes[return_code - 1000] << ss.str() << error_message << std::endl;
			std::cin.get();
		}

		return return_code;
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