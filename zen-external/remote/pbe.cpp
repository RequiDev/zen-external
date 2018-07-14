#include "pbe.hpp"
#include <remote/process.hpp>
#include <native/peb_native.hpp>

using namespace native;

namespace remote
{
	peb_t::peb_t(process_t* process):
		process_(process),
		pbi_(),
		peb_()
	{
	}

	bool peb_t::read()
	{
		if (!pbi_.PebBaseAddress)
		{
			if (!process_->query_basic_information(pbi_))
				return false;
		}

		if (!process_->read(uintptr_t(pbi_.PebBaseAddress), peb_))
			return false;

		peb_ldr_data_t ldr;
		if (!process_->read(uintptr_t(peb_.Ldr), ldr))
			return false;
		memcpy(peb_.Ldr, &ldr, sizeof ldr);

		RTL_USER_PROCESS_PARAMETERS process_parameters;
		if (!process_->read(uintptr_t(peb_.ProcessParameters), process_parameters))
			return false;
		memcpy(&peb_.ProcessParameters, &process_parameters, sizeof process_parameters);

		command_line_ = read_unicode_string(process_parameters.CommandLine);

		list_entry_t* forward_link = ldr.InLoadOrderModuleList.ForwardLink;

		while (forward_link)
		{
			ldr_data_table_entry_t entry;
			process_->read(uintptr_t(forward_link), entry);
			
			if (entry.BaseAddress)
			{
				module_t mod;
				mod.name = read_unicode_string(entry.BaseDllName);
				mod.base = uintptr_t(entry.BaseAddress);

				loader_entries_.push_back(mod);
			}
			forward_link = entry.InLoadOrderModuleList.ForwardLink;
			if (forward_link == ldr.InLoadOrderModuleList.ForwardLink)
				break;
		}

		return true;
	}

	PEB* peb_t::operator&()
	{
		return &peb_;
	}

	peb_t::operator PEB*()
	{
		return &peb_;
	}

	void peb_t::reset()
	{
		::memset(&peb_, 0, sizeof peb_);
	}

	std::string peb_t::read_unicode_string(const UNICODE_STRING& value) const
	{
		decltype(&::RtlInitUnicodeString) rtl_init_unicode_string = reinterpret_cast<decltype(&::RtlInitUnicodeString)>(::GetProcAddress(::GetModuleHandle("ntdll.dll"), "RtlInitUnicodeString"));
		std::unique_ptr<wchar_t[]> buffer(std::make_unique<wchar_t[]>(value.Length));
		if (!process_->read_memory(uintptr_t(value.Buffer), buffer.get(), value.Length))
			return "";

		std::wstring tmp(buffer.get());
		return std::string(tmp.begin(), tmp.end());
	}
} // namespace remote
