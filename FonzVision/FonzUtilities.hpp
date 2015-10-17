#include "stdafx.h"
#pragma once
#ifndef FONZUTILS_HPP
#define FONZUTILS_HPP
namespace FonzUtils {

	// This is used to deobfuscate strings using an XOR key.  Strings can be stored in the string table in an obfuscated state
	// then deobfuscated at run time. This stops really dumb people from looking at our strings, but will not stop a competant 
	// person.

	char* decloakString( _Out_ char* buffer, _In_ int bufferSize, _In_ const char* obfuscatedString, _In_ char* xorKey, _In_ int keyLength ) ;

	// This is a copy/paste of learn_more's sig scan posted at http://www.unknowncheats.me/forum/c-and-c/77419-findpattern.html
	// Shameless copy pasta, but it's a good utility and I can't think of a reason to reinvent the wheel.
	#define INRANGE(x,a,b)    (x >= a && x <= b) 
	#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
	#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))


	void* findText( void* rangeStart, void* rangeEnd, const char* text );

	void getModuleBoundaries( const char* moduleName, HINSTANCE* ourModule, unsigned int* lastModuleByte );

	DWORD findPattern( DWORD rangeStart, DWORD rangeEnd, const char* pattern );

	DWORD findPatternPlusBytes( DWORD rangeStart, DWORD rangeEnd, const char* pattern, int addBytes );

	DWORD findNthPattern(DWORD rangeStart, DWORD rangeEnd, const char* pattern, int n );

	DWORD findNthPatternPlusBytes(DWORD rangeStart, DWORD rangeEnd, const char* pattern, int addBytes, int n );

	
}

#endif