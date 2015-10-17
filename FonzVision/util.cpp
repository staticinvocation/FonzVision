#include "stdafx.h"
#include "util.h"
#include <stdio.h>

namespace util {

	void OpenConsole() {
		if (AllocConsole()) {
			FILE* pCout;
			freopen_s(&pCout, "CONIN$", "r+t", stdin);
			freopen_s(&pCout, "CONOUT$", "w+t", stdout);
			freopen_s(&pCout, "CONOUT$", "w+t", stderr);
			SetConsoleTitleA("");
		}
	}

}