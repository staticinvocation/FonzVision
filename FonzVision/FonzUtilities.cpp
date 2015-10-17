#include "stdafx.h"
#include "windows.h"
#include <Psapi.h>
#include <cstdio>
#include "FonzUtilities.hpp"
namespace FonzUtils {

	// This is used to deobfuscate strings using an XOR key.  Strings can be stored in the string table in an obfuscated state
	// then deobfuscated at run time. This stops really dumb people from looking at our strings, but will not stop a competant 
	// person.

	char* decloakString( _Out_ char* buffer, _In_ int bufferSize, _In_ const char* obfuscatedString, _In_ char* xorKey, _In_ int keyLength ) {
		char c;
		for (int i = 0; i < bufferSize; i++) {
			c = obfuscatedString[i] ^ (unsigned char)xorKey[i % keyLength];			
			buffer[i] = c;
			if (c == '\0')
				return buffer;
		}
		return buffer;
	}

	// This is a copy/paste of learn_more's sig scan posted at http://www.unknowncheats.me/forum/c-and-c/77419-findpattern.html
	// Shameless copy pasta, but it's a good utility and I can't think of a reason to reinvent the wheel.
	#define INRANGE(x,a,b)    (x >= a && x <= b) 
	#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
	#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))


	void* findText( void* rangeStart, void* rangeEnd, const char* text ) {
		void* match = 0;
		bool curMatch = 0;
		int len = strlen(text);
		//printf("Finding text %s with length %d\n", text, len);
		for( int i = (int)rangeStart; i < (int)rangeEnd; i++ ) {
			char* ind = (char*)i;
			char sPos;
			for (sPos = 0; sPos < len; sPos++) {
				if (*(ind + sPos) != *(text + sPos))
					break;
				if (sPos == len - 1 && text != ind) 
					return ind;
			}
		}
		return 0;
	}

	void getModuleBoundaries( const char* moduleName, HINSTANCE* ourModule, unsigned int* lastModuleByte ) {		
		MEMORY_BASIC_INFORMATION lbuffer;
		MODULEINFO lpmodinfo;
		int ourModuleSize = 0;
		int totalModuleSize = 0;	

		while (!(*ourModule = GetModuleHandleA( moduleName ), *ourModule)) { Sleep(500);}

		GetModuleInformation(GetCurrentProcess(), *ourModule, &lpmodinfo, sizeof(lpmodinfo));
		VirtualQuery(ourModule, &lbuffer, sizeof(lbuffer));
		ourModuleSize = lbuffer.RegionSize;
		totalModuleSize = lpmodinfo.SizeOfImage;
		*lastModuleByte = (unsigned int)ourModule + totalModuleSize;			
	}

	DWORD findPattern( DWORD rangeStart, DWORD rangeEnd, const char* pattern )
	{
		const char* pat = pattern;
		DWORD firstMatch = 0;
		for( DWORD pCur = rangeStart; pCur < rangeEnd; pCur++ )
		{
			if( !*pat ) return firstMatch;
			if( *(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte( pat ) ) {
				if( !firstMatch ) firstMatch = pCur;
				if( !pat[2] ) return firstMatch;
				if( *(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?' ) pat += 3;
				else pat += 2;    //one ?
			} else {
				pat = pattern;
				firstMatch = 0;
			}
		}
		printf("Signature not found\n");
		return NULL;
	}  

	DWORD findNthPatternPlusBytes(DWORD rangeStart, DWORD rangeEnd, const char* pattern, int addBytes, int n ) {		
		DWORD result;
		for(int i = 0; i < n; i++) {
			result = findPattern( rangeStart, rangeEnd, pattern );
			rangeStart = result + 1;
		}
		result = (DWORD)((char*)result + addBytes);
		return *(DWORD*)result;
	}

	DWORD findNthPattern(DWORD rangeStart, DWORD rangeEnd, const char* pattern, int n ) {		
		DWORD result;
		for(int i = 0; i < n; i++) {
			result = findPattern( rangeStart, rangeEnd, pattern );
			rangeStart = result + 1;
		}
		
		return result;
	}

	DWORD findPatternPlusBytes( DWORD rangeStart, DWORD rangeEnd, const char* pattern, int addBytes ) {
		DWORD result = findPattern( rangeStart, rangeEnd, pattern );
		result = (DWORD)((char*)result + addBytes);
		return *(DWORD*)result;
	}
}
