// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

DWORD ret_address = 0x0052D303;

__declspec(naked) void codecave() {
	__asm {
		pushad
		mov dword ptr ds : [ebx + 4] , 0xD
		popad
		mov dword ptr ds : [0x102AE98] , ebx

		jmp ret_address
	}
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		DWORD oldProtect;
		unsigned char* hookLocation = (unsigned char*)0x0052D2FD;

		VirtualProtect((void*)hookLocation, 6, PAGE_EXECUTE_READWRITE, &oldProtect);
		*hookLocation = 0xE9;
		*(DWORD*)(hookLocation + 1) = (DWORD)&codecave - ((DWORD)hookLocation + 5);
		*(hookLocation + 5) = 0x90;
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}