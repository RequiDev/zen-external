#pragma once
#include <base/auto_handle.hpp>
#include <winternl.h>
#include <vector>

namespace remote
{
	class process_t;

	struct module_t
	{
		uintptr_t base;
		std::string name;
	};

	class peb_t
	{
	public:
		explicit peb_t(process_t* process);

		bool read();

		PEB* operator&();
		operator PEB*();
		void reset();
	private:
		std::string read_unicode_string(const UNICODE_STRING& value) const;

		process_t* process_;
		PROCESS_BASIC_INFORMATION pbi_;
		PEB	peb_;

		std::string command_line_;
		std::vector<module_t> loader_entries_;
	};
} // namespace remote