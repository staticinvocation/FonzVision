#include "stdafx.h"
#include <Psapi.h>
#include <stdint.h>
#include <string>
#include <sstream> 
#include <vector>

MODULEINFO GetModuleInfo(char *szModule);

#define INRANGE(x,a,b) (x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

class Signature {
private:
	DWORD start_offset;
	DWORD size;
	char* pattern;

public:
	Signature(DWORD start_offset, DWORD size, char* pattern) {
		this->start_offset = start_offset;
		this->size = size;
		this->pattern = pattern;
	}

	Signature(HANDLE start_offset, DWORD size, char* pattern) {
		this->start_offset = (DWORD)start_offset;
		this->size = size;
		this->pattern = pattern;
	}

	Signature(size_t size, char* pattern) {
		this->start_offset = (DWORD)0x400000;
		this->size = size;
		this->pattern = pattern;
	}


	Signature(char* module, char* pattern) {
		MODULEINFO mod = GetModuleInfo(module);
		this->start_offset = (DWORD)mod.lpBaseOfDll;
		this->size = mod.SizeOfImage;
		this->pattern = pattern;
	}

	inline bool IsMatch(const PBYTE addr, const PBYTE pat, const PBYTE msk) {
		size_t n = 0;
		while (addr[n] == pat[n] || msk[n] == (BYTE)'?') {
			if (!msk[++n]) {
				return true;
			}
		}
		return false;
	}

	PBYTE LearnMoreFindPattern(const PBYTE rangeStart, DWORD len, const char* pattern) {
		PBYTE rangeEnd = (PBYTE)rangeStart + len;
		const unsigned char* pat = reinterpret_cast<const unsigned char*>(pattern);
		PBYTE firstMatch = 0;
		for (PBYTE pCur = rangeStart; pCur < rangeEnd; ++pCur) {
			if (*(PBYTE)pat == (BYTE)'\?' || *pCur == getByte(pat)) {
				if (!firstMatch) {
					firstMatch = pCur;
				}
				pat += (*(PWORD)pat == (WORD)'\?\?' || *(PBYTE)pat != (BYTE)'\?') ? 3 : 2;
				if (!*pat) {
					return firstMatch;
				}
			}
			else if (firstMatch) {
				pCur = firstMatch;
				pat = reinterpret_cast<const unsigned char*>(pattern);
				firstMatch = 0;
			}
		}
		return NULL;
	}

	inline LPVOID FindSignature() {
		return LearnMoreFindPattern((PBYTE)start_offset, size, pattern);
	}

};


template<class T> T* CreateInterface(void* addr) {
	return reinterpret_cast<T*>(addr);
}

template<class T> T* CreateInterfaceSig(Signature signature) {
	return CreateInterface<T>(signature.FindSignature());
}

template<class T> T* CreateInterfaceSigOffset(Signature signature, DWORD offset) {
	DWORD found = (DWORD)signature.FindSignature();
	found += offset;
	return CreateInterface<T>((void*)found);
}

template<class T> T* CreateInterfaceSigOpcode(Signature signature, DWORD offset) {
	DWORD found = (DWORD)signature.FindSignature();
	found += offset;

	void* ptrToEdianVal = reinterpret_cast<void*>(found);
	DWORD out;
	memcpy(&out, ptrToEdianVal, 4);

	return CreateInterface<T>((void*)out);
}

template<class T> T* CreateInterfaceSigOpcodeOffset(Signature signature, DWORD offset, DWORD offset2) {
	DWORD found = (DWORD)signature.FindSignature();
	found += offset;

	void* ptrToEdianVal = reinterpret_cast<void*>(found);
	DWORD out;
	memcpy(&out, ptrToEdianVal, 4);
	out += offset2;

	return CreateInterface<T>((void*)out);
}