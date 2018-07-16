#pragma once
#include <remote/module.hpp>
#include <native/peb_native.hpp>
#include <vector>

namespace remote
{
	class process_t;

	class peb_t
	{
	public:
		explicit peb_t(process_t* process);

		module_t* get_module(const char* module_name);
		bool reset();
	private:
		bool read();

		process_t* process_;
		PROCESS_BASIC_INFORMATION pbi_;
		PEB	peb_;
		PEB_LDR_DATA ldr_;
		RTL_USER_PROCESS_PARAMETERS process_parameters_;

		std::string command_line_;
		std::vector<module_t> modules_;
	};
} // namespace remote