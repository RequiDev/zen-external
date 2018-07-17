#include "peb.hpp"
#include <remote/process.hpp>

namespace remote
{
	peb_t::peb_t(process_t* process):
		process_(process),
		pbi_(),
		peb_(),
		ldr_(),
		process_parameters_()
	{
	}

	bool peb_t::read()
	{
		if (!pbi_.PebBaseAddress)
		{
			if (!process_->query_information(pbi_, ProcessBasicInformation))
				return false;
		}

		if (!process_->read(uintptr_t(pbi_.PebBaseAddress), peb_))
			return false;

		if (!process_->read(uintptr_t(peb_.Ldr), ldr_))
			return false;
		peb_.Ldr = &ldr_;

		
		if (!process_->read(uintptr_t(peb_.ProcessParameters), process_parameters_))
			return false;
		peb_.ProcessParameters = &process_parameters_;

		command_line_ = process_->read_unicode_string(process_parameters_.CommandLine);

		uintptr_t first_link = uintptr_t(ldr_.InLoadOrderModuleList.Flink);
		uintptr_t forward_link = first_link;
		do
		{
			LDR_DATA_TABLE_ENTRY entry;
			process_->read(forward_link, entry);
			forward_link = uintptr_t(entry.InLoadOrderModuleList.Flink);
			
			if (!entry.BaseAddress)
				continue;
			
			modules_.emplace_back(module_t(process_, &entry));
		} while (forward_link && forward_link != first_link);

		return true;
	}

	bool peb_t::get_module(const char* module_name, module_t& module_out)
	{
		for (auto& module : modules_)
		{
			if (module.name() != module_name)
				continue;

			module_out = module;
			return true;
		}
		return false;
	}

	bool peb_t::reset()
	{
		::memset(&peb_, 0, sizeof peb_);
		::memset(&ldr_, 0, sizeof ldr_);
		::memset(&pbi_, 0, sizeof pbi_);
		::memset(&process_parameters_, 0, sizeof process_parameters_);
		command_line_ = "";
		modules_.clear();
		return read();
	}
} // namespace remote
