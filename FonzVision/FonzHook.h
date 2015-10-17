/*
 ** FonzHook.h 
 ** Douglas 'Douggem' Confere 2014
 ** dougconfere@gmail.com
 */

//#include "stdafx.h"
#define HOOK_SIZE 6
namespace FonzUtils
{
	class FonzHook
	{		
		// Pointer to the code to be hooked
		unsigned char* pTarget;
		// Pointer to code to be redirected to
		char* pLanding;
		// Copy of the overwritten bytes from the hooked function
		unsigned char* oCode;		
		// Pointer to the trampoline code for returning to the original function after our code
		char* pTCode;
		// Number of bytes that need to be overwritten in the target function
		int iCSize;
		// Pointer to our return point in the hooked function
		void* returnCall;

		// Prepare everything for the hook without installing it
		bool prepare();
			
		
	public:
		// Construct the object with a pointer to the target function to be hooked and a pointer to the landing function
		FonzHook(void* target, void* landing);
		FonzHook(void* target, void* landing, void* bounce);
		static unsigned int extraBytes(unsigned char a, unsigned char b);
		// Remove the hook and restore original functionality in the target function
		bool remove();
		// Pointer to the trampoline, which will execute the functionality of the hooked function
		void* call;
		// Check that the hook is still installed
		bool check();
		// Install the prepared hook
		bool install();	

	private:
		bool prepTramp();
		
	};
}