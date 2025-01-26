
#include "json.hpp"

#include "Main.h"

#include <fstream>

#include "Hooks.h"

#pragma comment(lib, "ArkApi.lib")

#define PLUGIN_NAME "WShopBDFix"

namespace WShopBDFix
{


	void Load()
	{
		Log::Get().Init("WShopBDFix");

		Hooks::Init();
	}

	void Unload()
	{
		Hooks::Unload();
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		WShopBDFix::Load();
		break;
	case DLL_PROCESS_DETACH:
		WShopBDFix::Unload();
		break;
	}
	return TRUE;
}