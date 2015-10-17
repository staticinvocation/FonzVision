#include "stdafx.h"
#include "signature.h"

MODULEINFO GetModuleInfo(char *szModule) {
	MODULEINFO modinfo = { 0 };
	HMODULE hModule = GetModuleHandleA(szModule);
	if (hModule == 0) return modinfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
	return modinfo;
}