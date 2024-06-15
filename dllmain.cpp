#include "sdk/main.h"
#include "script.h"

void OpenConsole() {
	AllocConsole();
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	std::cout << "persistent wheels loading...\n\n";
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason_for_call, LPVOID lpReserved)
{
	switch (reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//OpenConsole();
		scriptRegister(hModule, ScriptInit);
		break;
	case DLL_PROCESS_DETACH:
		CollectGarbage();
		//FreeConsole();
		scriptUnregister(hModule);
		break;
	}
	return TRUE;
}