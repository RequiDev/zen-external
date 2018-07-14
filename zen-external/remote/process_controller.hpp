#pragma once
#include <cstdint>

namespace remote
{
	class process_controller_t
	{
	public:
		virtual ~process_controller_t() {}

		virtual bool read_memory(uintptr_t address, void* buffer, size_t size) = 0;
	};
} // namespace remote