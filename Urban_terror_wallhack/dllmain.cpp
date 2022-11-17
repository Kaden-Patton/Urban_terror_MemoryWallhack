// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

DWORD ret_address;

void(__stdcall* glDepthFunc)(unsigned int) = NULL;
void(__stdcall* glDepthRange)(double, double) = NULL;

DWORD count = 0;
__declspec(naked) void codecave() {
	__asm {
		pushad
		mov eax, dword ptr ds : [esp + 0x10]
		mov count, eax
		popad
		pushad
	}

	if (count > 900) {
		(*glDepthRange)(0.0, 0.0);
		(*glDepthFunc)(0x207);
	}
	else {
		(*glDepthRange)(0.0, 1.0);
		(*glDepthFunc)(0x203);
	}

	__asm {
		popad
		mov esi, dword ptr ds : [esi + 0xA18]
		jmp ret_address
	}
}

HMODULE openGLHandle = NULL;
unsigned char* hookLocation;

void injected_thread() {
	while (true) {
		if (openGLHandle == NULL) {
			openGLHandle = GetModuleHandle(L"opengl32.dll");
		}
		if (openGLHandle != NULL && glDepthFunc == NULL) {
			glDepthFunc = (void(__stdcall*)(unsigned int))GetProcAddress(openGLHandle, "glDepthFunc");
			glDepthRange = (void(__stdcall*)(double, double))GetProcAddress(openGLHandle, "glDepthRange");

			hookLocation = (unsigned char*)GetProcAddress(openGLHandle, "glDrawElements");
			hookLocation += 0x16;

			DWORD oldProtect;
			VirtualProtect((void*)hookLocation, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
			*hookLocation = 0xE9;
			*(DWORD*)(hookLocation + 1) = (DWORD)&codecave - ((DWORD)hookLocation + 5);
			*(hookLocation + 5) = 0x90;

			ret_address = (DWORD)(hookLocation + 0x6);
		}

		Sleep(1);
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
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)injected_thread, NULL, 0, NULL);

		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}