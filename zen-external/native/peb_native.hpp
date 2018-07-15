#pragma once
#include <cstdint>

namespace native
{
	struct list_entry_t
	{
		list_entry_t* ForwardLink;
		list_entry_t* BackwardLink;
	};

	struct peb_ldr_data_t
	{
		uint32_t Length;
		int Initialized;

		void* SSHandle;

		list_entry_t InLoadOrderModuleList;
		list_entry_t InMemoryOrderModuleList;
		list_entry_t InInitializationOrderModuleList;

		void* EntryInProgress;
	};

	struct ldr_data_table_entry_t
	{
		list_entry_t InLoadOrderModuleList;
		list_entry_t InMemoryOrderModuleList;
		list_entry_t InInitializationOrderModuleList;

		void* BaseAddress;
		void* EntryPoint;

		uint32_t SizeOfImage;

		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;

		uint32_t Flags;

		short LoadCount;
		short TlsIndex;

		list_entry_t HashTableEntry;

		uint32_t TimeDateStamp;
	};

}
