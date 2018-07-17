#include "module.hpp"
#include <remote/process.hpp>

namespace remote
{
	module_t::module_t(process_t* process, _LDR_DATA_TABLE_ENTRY* entry):
		process_(process),
		base_(uintptr_t(entry->BaseAddress)),
		size_(entry->SizeOfImage),
		name_(process_->read_unicode_string(entry->BaseDllName)),
		module_bytes_(nullptr),
		nt_headers_(nullptr)
	{
	}

	module_t::~module_t()
	{
		if (module_bytes_)
			delete[] module_bytes_;
	}

	bool module_t::refresh()
	{
		if (module_bytes_)
			delete[] module_bytes_;
		module_bytes_ = new uint8_t[size_];
		if (!process_->read_memory(base_, module_bytes_, size_))
			return false;
		if (!load_exports())
			return false;
		return true;
	}

	bool module_t::operator==(const module_t& rhs) const
	{
		return base_ == rhs.base_ && size_ == rhs.size_ && name_ == rhs.name_;
	}

	const std::string& module_t::name() const
	{
		return name_;
	}

	bool module_t::load_nt_headers()
	{
		if (nt_headers_)
			return true;
		if (!module_bytes_)
			return false;

		IMAGE_DOS_HEADER* dos_hdr = reinterpret_cast<IMAGE_DOS_HEADER*>(module_bytes_);
		if (dos_hdr->e_magic != IMAGE_DOS_SIGNATURE)
			return false;
		nt_headers_ = reinterpret_cast<IMAGE_NT_HEADERS*>(uintptr_t(module_bytes_) + dos_hdr->e_lfanew);
		if (nt_headers_->Signature != IMAGE_NT_SIGNATURE)
			return false;

		return true;
	}

	bool module_t::load_exports()
	{
		if (!load_nt_headers())
			return false;

		IMAGE_DATA_DIRECTORY* export_data_dir = &nt_headers_->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		if (!export_data_dir->VirtualAddress || !export_data_dir->Size)
			return false;
		IMAGE_EXPORT_DIRECTORY* export_dir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(uintptr_t(module_bytes_) + export_data_dir->VirtualAddress);
		if (!export_dir || !export_dir->NumberOfFunctions)
			return false;

		uintptr_t* functions = reinterpret_cast<uintptr_t*>(uintptr_t(module_bytes_) + export_dir->AddressOfFunctions);
		uintptr_t* names = reinterpret_cast<uintptr_t*>(uintptr_t(module_bytes_) + export_dir->AddressOfNames);
		uint16_t* ordinals = reinterpret_cast<uint16_t*>(uintptr_t(module_bytes_) + export_dir->AddressOfNameOrdinals);

		for (size_t i = 0u; i < export_dir->NumberOfNames; ++i)
		{
			char* buffer = reinterpret_cast<char*>(uintptr_t(module_bytes_) + names[i]);
			exports_.emplace_back(std::make_pair(buffer, base_ + functions[ordinals[i]]));
		}
		return true;
	}

	uintptr_t module_t::get_proc_address(const char* export_name)
	{
		if (exports_.empty() && !refresh())
			return 0;

		for (const auto& exp : exports_)
		{
			if (exp.first == export_name)
				return exp.second;
		}

		return 0;
	}

	uintptr_t module_t::find_pattern(const char* pattern)
	{
		if (!module_bytes_ && !refresh())
			return 0;

		std::vector<int> bytes;
		char* start = const_cast<char*>(pattern);
		char* end = start + strlen(pattern);

		for (char* current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else
			{
				bytes.push_back(strtoul(current, &current, 16));
			}
		}

		size_t pattern_size = bytes.size();
		const int* bytes_data = bytes.data();

		for (size_t i = 0u; i < size_ - pattern_size; ++i)
		{
			bool found = true;
			for (size_t j = 0u; j < pattern_size; ++j)
			{
				if (module_bytes_[i + j] != bytes_data[j] && bytes_data[j] != -1)
				{
					found = false;
					break;
				}
			}
			if (found)
				return base_ + i;
		}

		return 0;
	}
} // namespace remote
