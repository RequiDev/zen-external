#include "peb.hpp"
#include <remote/process.hpp>
#include <native/peb_native.hpp>
#include <base/auto_alloc.hpp>

using namespace native;

namespace remote
{
	uintptr_t module_t::get_proc_address(const char* export_name)
	{
		for (const auto& exp : exports)
		{
			if (exp.first == export_name)
				return exp.second;
		}

		return 0;
	}

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

		uintptr_t first_link = uintptr_t(ldr.InLoadOrderModuleList.ForwardLink);
		uintptr_t forward_link = first_link;
		do 
		{
			ldr_data_table_entry_t entry;
			process_->read(forward_link, entry);
			forward_link = uintptr_t(entry.InLoadOrderModuleList.ForwardLink);
			
			if (!entry.BaseAddress)
				continue;
			
			module_t mod;
			mod.name = read_unicode_string(entry.BaseDllName);
			mod.base = uintptr_t(entry.BaseAddress);
			// read nt shit
			IMAGE_DOS_HEADER dos_hdr;
			if (!process_->read(mod.base, dos_hdr) || dos_hdr.e_magic != IMAGE_DOS_SIGNATURE)
				continue;
			if (!process_->read(mod.base + dos_hdr.e_lfanew, mod.nt_headers) || mod.nt_headers.Signature != IMAGE_NT_SIGNATURE)
				continue;

			IMAGE_DATA_DIRECTORY export_data_dir = mod.nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			if (!export_data_dir.VirtualAddress || !export_data_dir.Size)
				continue;
			IMAGE_EXPORT_DIRECTORY export_dir;
			if (!process_->read(mod.base + export_data_dir.VirtualAddress, export_dir) || !export_dir.NumberOfFunctions)
				continue;

			base::auto_alloc_t<uintptr_t> functions(export_dir.NumberOfFunctions);
			base::auto_alloc_t<uintptr_t> names(export_dir.NumberOfNames);
			base::auto_alloc_t<uint16_t> ordinals(export_dir.NumberOfNames);

			process_->read_memory(mod.base + export_dir.AddressOfFunctions, functions, functions.size());
			process_->read_memory(mod.base + export_dir.AddressOfNames, names, names.size());
			process_->read_memory(mod.base + export_dir.AddressOfNameOrdinals, ordinals, ordinals.size());

			for (size_t i = 0u; i < export_dir.NumberOfNames; ++i)
			{
				char buffer[128];
				process_->read_memory(mod.base + names[i], buffer, 128);

				mod.exports.emplace_back(std::make_pair(buffer, mod.base + functions[ordinals[i]]));
			}

			modules_.push_back(mod);
		} while (forward_link && forward_link != first_link);

		return true;
	}

	module_t* peb_t::get_module(const char* module_name)
	{
		for (auto& module : modules_)
		{
			if (module.name == module_name)
				return &module;
		}
		return nullptr;
	}

	bool peb_t::reset()
	{
		::memset(&peb_, 0, sizeof peb_);
		return read();
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
