#include <windows.h>

static void(*g_copyTlsCallback)(void*);
static void* g_copyTlsContext;

BOOL WINAPI DllMain(void* dllHandle, DWORD reason, LPVOID)
{
	if (reason == DLL_THREAD_ATTACH)
	{
		if (g_copyTlsCallback)
		{
			g_copyTlsCallback(g_copyTlsContext);
		}
	}

	return TRUE;
}

extern "C" __declspec(dllexport) void SetTlsCallback(void(*cb)(void*), void* d)
{
	g_copyTlsCallback = cb;
	g_copyTlsContext = d;
}