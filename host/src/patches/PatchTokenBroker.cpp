#include "StdInc.h"
#include "Hooking.Patterns.h"

void PatchTokenBroker()
{
	HMODULE tb = LoadLibrary(L"tokenbroker.dll");
	auto matches = hook::module_pattern(tb, "88 44 24 38 84 C0 0F 85 0B 01 00 00");

	if (matches.size() > 0)
	{
		uint8_t* patchAddr = matches.get(0).get<uint8_t>(0);

		DWORD oldProtect;
		VirtualProtect(patchAddr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);

		*(uint16_t*)(patchAddr) = 0x1B0;
		*(uint32_t*)(patchAddr + 2) = 0x38244488;
		*(uint16_t*)(patchAddr + 6) = 0xE990;
	}
	else
	{
		auto matches = hook::module_pattern(tb, "44 0F B6 F8 84 C0 0F 85");

		if (matches.size() > 0)
		{
			uint8_t* patchAddr = matches.get(0).get<uint8_t>(0);

			DWORD oldProtect;
			VirtualProtect(patchAddr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);

			*(uint16_t*)(patchAddr) = 0x1B0;
			*(uint32_t*)(patchAddr + 2) = 0xF8B60F44;
			*(uint16_t*)(patchAddr + 6) = 0xE990;
		}
		else
		{
			// 14936
			auto matches = hook::module_pattern(tb, "44 8A F0 84 C0 0F 85");

			assert(matches.size() == 1);

			uint8_t* patchAddr = matches.get(0).get<uint8_t>(0);

			DWORD oldProtect;
			VirtualProtect(patchAddr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);

			*(uint16_t*)(patchAddr) = 0x1B0;
			*(uint32_t*)(patchAddr + 2) = 0x90F08A44;
			*( uint8_t*)(patchAddr + 6) = 0xE9;
		}
	}
}