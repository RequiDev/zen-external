#pragma once
#include <remote/process.hpp>
#include <drawing/overlay.hpp>
#include <drawing/overlay_controller.hpp>
#include <remote/process_controller.hpp>

namespace cheat
{
	class cheat_t:
		public drawing::overlay_controller_t,
		public remote::process_controller_t
	{
	public:
		cheat_t(const std::string& game, const std::string& class_name);
		virtual int mainloop();

	private:
		// overlay_controller_t
		virtual void set_renderer(drawing::renderer_t* renderer);

		// process_controller_t
		virtual bool read_memory(uintptr_t address, void* buffer, size_t size);

		remote::process_t process_;
		drawing::overlay_t overlay_;
		drawing::renderer_t* renderer_;

		std::string game_;
		std::string class_name_;
	};
} // namespace cheat