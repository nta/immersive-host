#pragma once

#include <roapi.h>

void ImHost_AddActivationFactoryWithResult(const std::wstring& classId, const std::function<HRESULT(HSTRING, IActivationFactory**, void*)>& factory);
void ImHost_AddActivationCallback(const std::function<void()>& function);
void ImHost_AddActivationProxy(const std::wstring& id, const std::function<HRESULT(IUnknown*, IActivationFactory**)>& function);
HRESULT ImHost_GetActivationFactory(HSTRING activatableClassId, REFIID iid, void** factory); // original function
const std::wstring& ImHost_GetAppDataPath();
const std::wstring& ImHost_GetOriginalCommandLine();
HRESULT ImHost_GetAppScopedActivationFactory(HSTRING activatableClassId, REFIID iid, void** factory);
bool ImHost_IsActivationProxy();
HANDLE ImHost_GetActivationProxyProcessHandle();
HWND ImHost_GetCoreWindowHandle();
DWORD ImHost_GetParentProcessId();

#define DEFINE_MODULE_CHECK(key, name) \
	struct key##Module { static inline const wchar_t* GetName() { return L##name; }	};

DEFINE_MODULE_CHECK(TokenBroker, "TokenBroker.dll");
DEFINE_MODULE_CHECK(WebCore, "windows.security.authentication.web.core.dll");
DEFINE_MODULE_CHECK(Store, "windows.applicationmodel.store.dll");
DEFINE_MODULE_CHECK(TWinAPI, "twinapi.appcore.dll");
DEFINE_MODULE_CHECK(MrmCore, "MrmCoreR.dll");

struct NullModule { static inline const wchar_t* GetName() { return nullptr; } };

template<typename TModule>
bool IsModule(void* address)
{
	static char* g_module;
	static char* g_moduleEnd;

	if (!g_module)
	{
		g_module = (char*)GetModuleHandle(TModule::GetName());

		if (g_module)
		{
			PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)g_module;
			PIMAGE_NT_HEADERS ntHeader = (PIMAGE_NT_HEADERS)(g_module + dosHeader->e_lfanew);

			g_moduleEnd = g_module + ntHeader->OptionalHeader.SizeOfImage;
		}
	}

	return (address >= g_module && address <= g_moduleEnd);
}

inline void ImHost_AddActivationFactory(const std::wstring& classId, PFNGETACTIVATIONFACTORY factory)
{
	ImHost_AddActivationFactoryWithResult(classId, std::bind(factory, std::placeholders::_1, std::placeholders::_2));
}