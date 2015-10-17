#include "stdafx.h"
#include "util.h"
#include "RVBootstrap.h"

HMODULE ourModule = NULL;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call,LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		ourModule = hModule;
	}
	return TRUE;
}

bool runonce = false;
extern "C" __declspec(dllexport) int HookEvent(int code, WPARAM wParam, LPARAM lParam) {
	if (!runonce) {
		runonce = true;
		util::OpenConsole();
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RVBootstrap::Initialize, ourModule, 0, 0);
	}
	return(CallNextHookEx(NULL, code, wParam, lParam));
}