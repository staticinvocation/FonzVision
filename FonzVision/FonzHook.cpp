#include "FonzHook.h"
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>



namespace FonzUtils
{
	/**
	** FonzHook constructor, takes target function to hook, and landing function to redirect to
	**/
	FonzHook::FonzHook(void* target, void* landing)
	{
		pTarget = (unsigned char*)target;
		pLanding = (char*)landing;
		
		pTCode = NULL;
		returnCall = NULL;
		prepare();
	}

	bool FonzHook::prepare()
	{
		if (!prepTramp()){
			printf("Could not prepare trampoline\n");
			return false;
		}		
		return true;
	}

	bool FonzHook::install()
	{
		// All we have to do is overwrite the first 6 bytes of the hooked function with push (function address) ret
		DWORD dwOldProtect, dwNewProtect;
		VirtualProtect(pTarget, HOOK_SIZE, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		*pTarget = 0x68;
		(*(int*)&pTarget[1]) = (int)pLanding;
		pTarget[5] = 0xC3;
		VirtualProtect(pTarget, HOOK_SIZE, dwOldProtect, &dwNewProtect);
		FlushInstructionCache(GetCurrentProcess(), pTarget, HOOK_SIZE);
		return true;
	}

	bool FonzHook::remove()
	{
		// Just replace the first 6 bytes of the hooked function with the first 6 bytes of the trampoline
		DWORD dwOldProtect, dwNewProtect;
		VirtualProtect(pTarget, HOOK_SIZE, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		for (int i = 0; i < iCSize; i++)
		{
			pTarget[i] = pTCode[i];
		}

		VirtualProtect(pTarget, HOOK_SIZE, dwOldProtect, &dwNewProtect);
		FlushInstructionCache(GetCurrentProcess(), pTarget, HOOK_SIZE);

		free(pTCode);
		return true;
	}

	bool FonzHook::check()
	{
		bool isHooked = 0;
		if (*pTarget == 0x68 && *(char**)&pTarget[1] == pLanding)
			isHooked = 1;
		return isHooked;
	}

	unsigned int FonzHook::extraBytes(unsigned char a, unsigned char b) {
		unsigned char lower = (a & 0x0f);
		if (a >= 0xc0) {
			return 0;
		}
		else if (a >= 0x80) {
			if ((lower == 4) || (lower == 12))
				return 5;
			else
				return 4;
		}
		else if (a >= 0x40) {
			if ((lower == 4) || (lower == 12))
				return 2;
			else
				return 1;

		}
		else {
			if ((lower == 4) || (lower == 12))
			{
				if ((b & 0x07) == 0x05)
					return 5;
				else
					return 1;
			}
			else if ((lower == 5) || (lower == 13))
			{
				return 4;
			}
			return 0;
		}
	}

	bool FonzHook::prepTramp()
	{
		// We need to build the trampoline here.  To do this, we need to determine how many bytes we're going to overwrite
		// The trampoline will be the overwritten bytes, followed by pushad, pushfd, jump to landing function, popfd, popad, jump back to hooked function
		// The size of the trampoline function will be overwritten bytes + 24(6*4 for pushad/pushfd) + 5 (jmp to landing function) + 5 (jump to hooked fu.nction) = overwrittenBytes + 34d
		unsigned char*  pOCode = pTarget;
		unsigned char* pOrig = 0;
		DWORD dwOldProtect;
				
		// Why didn't hte Fonz just virtualalloc here?  We may never know.
		pTCode = (char*)malloc(64);
		VirtualProtect(pTCode, 64, MEM_COMMIT | MEM_RESERVE | PAGE_EXECUTE_READWRITE, &dwOldProtect);

		DWORD oProtect;
		if (!VirtualProtect(pOCode, 16, PAGE_EXECUTE_READ, &oProtect)) {
			printf("Could not set permissions of target function\n");
			//return 0; // We have to be able to read the target function to build our trampoline
		}

		while (*pOCode == 0xe9) { // JMP
			unsigned char *tmp = pOCode;
			int iptr = *reinterpret_cast<int *>(pOCode + 1);
			pOCode += iptr + 5;

			printf("Skipping jump from %p to %p offset %X\n", tmp, pOCode, iptr);

			// Assume jump took us out of our read enabled zone, get rights for the new one
			DWORD tempProtect;
			VirtualProtect(tmp, 16, oProtect, &tempProtect);
			if (!VirtualProtect(pOCode, 16, PAGE_EXECUTE_READ, &oProtect)) {
				printf("Trampline build failed to make jump target code read and executable");
				return NULL;
			}
		}

		int iTSize = 0;
		unsigned int index = 0;
		do {
			unsigned char opcode = pOCode[index];
			unsigned char a = pOCode[index + 1];
			unsigned char b = pOCode[index + 2];
			unsigned int extra = 0;

			switch (opcode) {
			case 0x00: // ADD
				extra = 1;
				break;
			case 0x33: // XOR
				extra = extraBytes(a, b) + 1;
				break;
			case 0x4F: // decrement, added for tommy's shitty win8
				extra = extraBytes(a, b) + 1;
				break;
			case 0x50: // PUSH
			case 0x51:
			case 0x52:
			case 0x53:
			case 0x54:
			case 0x55:
			case 0x56:
			case 0x57:
			case 0x58: // POP
			case 0x59:
			case 0x5a:
			case 0x5b:
			case 0x5c:
			case 0x5d:
			case 0x5e:
			case 0x5f:
				break;
			case 0x6a: // PUSH immediate
				extra = 1;
				break;
			case 0x68: // PUSH immediate
				extra = 4;
				break;
			case 0x81: // CMP immediate
				extra = extraBytes(a, b) + 5;
				break;
			case 0x83:	// CMP
				extra = extraBytes(a, b) + 2;
				break;
			case 0x8b:	// MOV
				extra = extraBytes(a, b) + 1;
				break;
			case 0x94: // 	Load Status Flags into AH Register, added for tommy's shitty win8					
				break;
			case 0xb8:
				extra = 4;
				break;
			case 0xc3:
				extra = 4;
				break;
			default: {
				int rmop = ((a >> 3) & 7);
				if (opcode == 0xff && rmop == 6) { // PUSH memory
					extra = extraBytes(a, b) + 1;
					break;
				}

				DWORD tempProtect;
				VirtualProtect(pOCode, HOOK_SIZE, oProtect, &tempProtect);
				printf("Unrecognized opcode %02x in %x\n", opcode, pOCode);
				return false;
				break;
			}
			}

			pTCode[index] = opcode;
			++index;

			for (unsigned int i = 0; i < extra; ++i)
				pTCode[index + i] = pOCode[index + i];
			index += extra;

		} while (index < HOOK_SIZE);

		iCSize = index;

		pTCode[index++] = 0xe9;
		int* retPtr = (int*)&pTCode[index];
		int offset = pOCode - (unsigned char*)pTCode - 5;
		*retPtr = offset;
		index += 4;
		call = pTCode;
		pTarget = pOCode; // If we are skipping jumps, we hook the function at the end
		if (!VirtualProtect(pTCode, 64, PAGE_EXECUTE_READWRITE, &dwOldProtect)){
			printf("Virtual protect failed\n");
			return false;
		}
		printf("Function %X hooked, trampoline at %X trampoline size %d\n", pTarget, pTCode, index);
		return true;
	}
}