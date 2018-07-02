#pragma once
#include <cstdint>
#include <array>

namespace csgo
{
	struct ent_info_t
	{
		uintptr_t entity;
		int	serial_number;
		uintptr_t previous_entity;  //CEntInfo *
		uintptr_t next_entity; //CEntInfo *
	}; //size=0x10

	// TODO: Make a proper class?
	using entity_list_t = std::array<ent_info_t, 4096>;
} // namespace csgo