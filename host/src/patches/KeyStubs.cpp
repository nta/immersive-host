#include "StdInc.h"
#include <MinHook.h>

static HANDLE g_rootHandle;
static std::wstring g_perAppKeyName;

HRESULT ImHost_GetAppScopedActivationFactory(HSTRING activatableClassId, _In_ REFIID iid, _COM_Outptr_ void ** factory)
{
	HKEY hKey = (HKEY)g_rootHandle;

	std::wstring classId = WindowsGetStringRawBuffer(activatableClassId, nullptr);

	HKEY subkey;

	if (RegOpenKeyEx(hKey, (L"ActivatableClassId\\" + classId).c_str(), 0, KEY_READ, &subkey) == ERROR_SUCCESS)
	{
		DWORD size = 0;
		DWORD type = REG_SZ;
		RegQueryValueEx(subkey, L"DllPath", nullptr, &type, nullptr, &size);

		std::vector<uint8_t> buffer(size);
		RegQueryValueEx(subkey, L"DllPath", nullptr, &type, &buffer[0], &size);

		HMODULE module = LoadLibrary(reinterpret_cast<wchar_t*>(&buffer[0]));
		assert(module);

		auto getActivationFactory = (PFNGETACTIVATIONFACTORY)GetProcAddress(module, "DllGetActivationFactory");

		ComPtr<IActivationFactory> factoryPtr;
		getActivationFactory(activatableClassId, factoryPtr.ReleaseAndGetAddressOf());

		factoryPtr.CopyTo(iid, factory);

		return S_OK;
	}

	return E_INVALIDARG;
}

static NTSTATUS(NTAPI* g_origLdrLoadDll)(IN PWSTR pwszSearchPathOrFlags OPTIONAL, IN PULONG pfFlags OPTIONAL, IN PCUNICODE_STRING pName, OUT PHANDLE phMod);
static NTSTATUS(NTAPI* g_origNtOpenKey)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES);

NTSTATUS NTAPI LdrLoadDll_Custom(IN PWSTR pwszSearchPathOrFlags OPTIONAL, IN PULONG pfFlags OPTIONAL, IN PCUNICODE_STRING pName, OUT PHANDLE phMod)
{
	HANDLE hModule;
	NTSTATUS status = g_origLdrLoadDll(pwszSearchPathOrFlags, pfFlags, pName, phMod);

	if (status == STATUS_NOT_APPCONTAINER)
	{
		// NOTE: this may break a *lot* of stuff
		NtCurrentPeb()->Diff0.W81.IsAppContainer = 1;

		status = g_origLdrLoadDll(pwszSearchPathOrFlags, pfFlags, pName, phMod);

		NtCurrentPeb()->Diff0.W81.IsAppContainer = 0;
	}

	return status;
}

NTSTATUS NTAPI NtOpenKey_Custom(PHANDLE handle, ACCESS_MASK mask, POBJECT_ATTRIBUTES key)
{
	NTSTATUS status = g_origNtOpenKey(handle, mask, key);
	std::wstring fullKeyName;

	if (key->RootDirectory)
	{
		ULONG length = 0;
		NtQueryKey(key->RootDirectory, KeyNameInformation, nullptr, 0, &length);

		std::vector<uint8_t> data(length);
		NtQueryKey(key->RootDirectory, KeyNameInformation, &data[0], length, &length);

		auto nameInfo = reinterpret_cast<KEY_NAME_INFORMATION*>(&data[0]);
		fullKeyName = std::wstring(nameInfo->Name, nameInfo->NameLength / 2) + L"\\";
	}

	fullKeyName += std::wstring(key->ObjectName->Buffer, key->ObjectName->Length / 2);

	if (g_perAppKeyName.empty() && _wcsnicmp(fullKeyName.c_str(), L"\\Registry\\A", 11) == 0)
	{
		//NtOpenKey(&g_hKeyPerApp, KEY_READ, targetKey);
		g_perAppKeyName = fullKeyName;

		int firstBackslash = g_perAppKeyName.find_first_of(L'\\');
		int secondBackslash = g_perAppKeyName.find_first_of(L'\\', firstBackslash + 1);
		int thirdBackslash = g_perAppKeyName.find_first_of(L'\\', secondBackslash + 1);
		int lastBackslash = g_perAppKeyName.find_first_of(L'\\', thirdBackslash + 1);

		g_perAppKeyName = g_perAppKeyName.substr(0, lastBackslash);

		UNICODE_STRING str;
		RtlInitUnicodeString(&str, L"");

		OBJECT_ATTRIBUTES attributes;
		InitializeObjectAttributes(&attributes, &str, OBJ_CASE_INSENSITIVE, key->RootDirectory, nullptr);

		NTSTATUS statusB = NtOpenKey(&g_rootHandle, mask, &attributes);

		assert(NT_SUCCESS(statusB));
	}

	return status;
}

// TODO: check for x86!
NTSTATUS WINAPI PsmRegisterAppStateChangeNotification_Custom(void* a1, void* a2, void* a3, void* a4)
{
#if _M_IX86
#error Not checked for i386 :(
#endif

	return STATUS_SUCCESS;
}

HMODULE WINAPI LoadPackagedLibrary_Custom(
	_In_       LPCWSTR lpwLibFileName,
	_Reserved_ DWORD   Reserved
)
{
	return LoadLibraryW(lpwLibFileName);
}

void InitializeKeyStubs()
{
	LoadLibrary(L"twinapi.appcore.dll");

	MH_CreateHookApi(L"ntdll.dll", "NtOpenKey", NtOpenKey_Custom, (void**)&g_origNtOpenKey);
	MH_CreateHookApi(L"twinapi.appcore.dll", "PsmRegisterAppStateChangeNotification", PsmRegisterAppStateChangeNotification_Custom, nullptr);
	MH_CreateHookApi(L"ntdll.dll", "LdrLoadDll", LdrLoadDll_Custom, (void**)&g_origLdrLoadDll);
	MH_CreateHookApi(L"kernelbase.dll", "LoadPackagedLibrary", LoadPackagedLibrary_Custom, nullptr);
	MH_EnableHook(MH_ALL_HOOKS);
}