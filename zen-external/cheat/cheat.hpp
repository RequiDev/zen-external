#pragma once
#include <remote\process.hpp>
#include <drawing\overlay.hpp>
#include <string>

namespace cheat
{
	class cheat_t
	{
	public:
		cheat_t(const std::string& game, const std::string& class_name);
		int mainloop();
	private:
		remote::process_t process_;
		drawing::overlay_t overlay_;
		drawing::renderer_t* renderer_;

		std::string game_;
		std::string class_name_;
	};
} // namespace cheat