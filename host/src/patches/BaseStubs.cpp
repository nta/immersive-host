#include "StdInc.h"

#include <delayimp.h>

#include <MinHook.h>

#include <PackageIdentity.h>

#include <strsafe.h>

#define __pfnDliFailureHook2 MEOW
#include <delayloadhandler.h>
#undef __pfnDliFailureHook2

static DWORD(WINAPI* g_origGetModuleFileNameW)(HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
static ULONG(WINAPI* g_origNtStatusToDosError)(NTSTATUS);
static LONG(NTAPI* g_origWaitOnAddress)(void* address, void* wait, size_t size);
static VOID(NTAPI* g_origWakeAddressAll)(void* address);
static BOOL(WINAPI* g_origGetTokenInformation)(HANDLE TokenHandle, TOKEN_INFORMATION_CLASS TokenInformationClass, LPVOID TokenInformation, DWORD TokenInformationLength, PDWORD ReturnLength);
static NTSTATUS(WINAPI* g_origNtQueryInformationToken)(HANDLE TokenHandle, TOKEN_INFORMATION_CLASS TokenInformationClass, PVOID TokenInformation, ULONG TokenInformationLength, PULONG ReturnLength);
static PVOID(WINAPI* g_origResolveDelayLoadedAPI)(PVOID ParentModuleBase, PCIMAGE_DELAYLOAD_DESCRIPTOR DelayloadDescriptor, PDELAYLOAD_FAILURE_DLL_CALLBACK FailureDllHook, void* FailureSystemHook, PIMAGE_THUNK_DATA ThunkAddress, ULONG Flags);

LPCWSTR WINAPI GetCommandLineW_Custom()
{
	//return L"-ServerName:DirectXApplication.App.AppXss2a6f7wvvcx2xdatdznjbwwpz57czj3.mca";
	//return L"-ServerName:App.AppXq5rsf19nejqvvn2dnm8y3jhz8dfg09ta.mca";
	//return L"-ServerName:App.AppXz3sdjmp9qbjsw9sn2xasnj0pcbks50j2.mca"; // TR
	return L"-ServerName:App.AppX92m5m1wwhhzwrbzhvdt7whj6b4qqpaqa.mca"; // Delta
}

DWORD WINAPI GetModuleFileNameW_Custom(HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
	// special case for 'us'
	if (hModule == nullptr || hModule == NtCurrentPeb()->ImageBaseAddress)
	{
		std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();

		if (identity.get())
		{
			std::wstring concatStr = identity->GetApplicationRoot() + L"\\" + identity->GetApplicationFileName();

			StringCchCopy(lpFilename, nSize, concatStr.c_str());

			return wcslen(lpFilename);
		}
	}

	return g_origGetModuleFileNameW(hModule, lpFilename, nSize);
}

DWORD GetLastError_Custom()
{
	DWORD error = RTNtCurrentTeb()->LastErrorValue;

	return error;
}

ULONG WINAPI RtlNtStatusToDosError_Custom(NTSTATUS status)
{
	ULONG error = g_origNtStatusToDosError(status);

	if (error == ERROR_NOT_FOUND)
	{
		// laa-dii-daaaaa
		return 0;
	}

	return error;
}

static DWORD g_stateCookie;

DWORD* WINAPI OpenState_Custom()
{
	return &g_stateCookie;
}

BOOL GetStateFolder_Custom(DWORD* state, int folder, PWSTR path, LPDWORD size)
{
	std::wstring appDataPath = ImHost_GetAppDataPath();

	if (path)
	{
		wcscpy(path, appDataPath.c_str());
		*size = wcslen(path) + 1;
	}
	else
	{
		*size = wcslen(appDataPath.c_str()) + 1;

		SetLastError(ERROR_INSUFFICIENT_BUFFER);

		return FALSE;
	}

	return TRUE;
}

void WINAPI CloseState_Custom(DWORD* cookie)
{

}

BOOL WINAPI CapabilityCheck_Custom(HANDLE hUnknown, PCWSTR capabilityName, BOOL* hasCapability)
{
	*hasCapability = TRUE;

	return TRUE;
}

LONG NTAPI RtlWaitOnAddress_Custom(void* address, void* wait, size_t size)
{
	if (IsModule<TWinAPIModule>(_ReturnAddress()))
	{
		*(char*)address = 1;

		return 0;
	}

	return g_origWaitOnAddress(address, wait, size);
}

DWORD g_activationUnlock;
DWORD g_activationLock;

VOID NTAPI RtlWakeAddressAll_Custom(PVOID address)
{
	if (IsModule<TWinAPIModule>(_ReturnAddress()))
	{
		if (g_activationUnlock == 0)
		{
			++g_activationUnlock;

			g_origWakeAddressAll(&g_activationUnlock);

			DWORD oldLock = g_activationLock;
			g_origWaitOnAddress(&g_activationLock, &oldLock, sizeof(g_activationLock));
		}
	}

	return g_origWakeAddressAll(address);
}


BOOL WINAPI GetTokenInformation_Custom(HANDLE TokenHandle, TOKEN_INFORMATION_CLASS TokenInformationClass, LPVOID TokenInformation, DWORD TokenInformationLength, PDWORD ReturnLength)
{
	BOOL retval = g_origGetTokenInformation(TokenHandle, TokenInformationClass, TokenInformation, TokenInformationLength, ReturnLength);

	if (TokenInformationClass == TokenIsAppContainer)
	{
		if (retval)
		{
			if (IsModule<MrmCoreModule>(_ReturnAddress()))
			{
				*(BOOL*)TokenInformation = TRUE;
			}
		}
	}

	return retval;
}

NTSTATUS WINAPI NtQueryInformationToken_Custom(HANDLE TokenHandle, TOKEN_INFORMATION_CLASS TokenInformationClass, PVOID TokenInformation, ULONG TokenInformationLength, PULONG ReturnLength)
{
	NTSTATUS status;

	if (g_origNtQueryInformationToken)
	{
		status = g_origNtQueryInformationToken(TokenHandle, TokenInformationClass, TokenInformation, TokenInformationLength, ReturnLength);
	}

	// add app ID/package names
	if (TokenInformationClass == TokenSecurityAttributes)
	{
		std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();
		std::wstring familyName = identity->GetFamilyName();
		std::wstring fullName = identity->GetFullName();
		std::wstring appId = identity->GetPrimaryAppId();

		*ReturnLength = sizeof(TOKEN_SECURITY_ATTRIBUTES_INFORMATION) + (sizeof(TOKEN_SECURITY_ATTRIBUTE_V1) * 1) + (sizeof(UNICODE_STRING) * 4) +
			(appId.length() * sizeof(WCHAR)) + (fullName.length() * sizeof(WCHAR)) +
			(familyName.length() * sizeof(WCHAR));

		if (TokenInformationLength == 0)
		{
			return STATUS_BUFFER_TOO_SMALL;
		}

		memset(TokenInformation, 0, *ReturnLength);

		TOKEN_SECURITY_ATTRIBUTES_INFORMATION* attributes = reinterpret_cast<TOKEN_SECURITY_ATTRIBUTES_INFORMATION*>(TokenInformation);

		TOKEN_SECURITY_ATTRIBUTE_V1* attributeArray = reinterpret_cast<TOKEN_SECURITY_ATTRIBUTE_V1*>(attributes + 1);
		attributes->AttributeCount = 1;
		attributes->Version = 1;
		attributes->Attribute.pAttributeV1 = attributeArray;

		PUNICODE_STRING unicodeStrings = reinterpret_cast<PUNICODE_STRING>(attributeArray + 1);

		attributeArray->ValueCount = 3;
		attributeArray->ValueType = TOKEN_SECURITY_ATTRIBUTE_TYPE_STRING;
		attributeArray->Values.pString = unicodeStrings;

		wchar_t* charBuffers = reinterpret_cast<wchar_t*>(unicodeStrings + 4);
		wcsncpy(charBuffers, fullName.c_str(), fullName.length());

		unicodeStrings[0].Buffer = charBuffers;
		unicodeStrings[0].Length = fullName.length() * 2;
		unicodeStrings[0].MaximumLength = fullName.length() * 2;

		charBuffers += fullName.length();

		wcsncpy(charBuffers, appId.c_str(), appId.length());

		unicodeStrings[1].Buffer = charBuffers;
		unicodeStrings[1].Length = appId.length() * 2;
		unicodeStrings[1].MaximumLength = appId.length() * 2;

		charBuffers += appId.length();

		wcsncpy(charBuffers, familyName.c_str(), familyName.length());

		unicodeStrings[2].Buffer = charBuffers;
		unicodeStrings[2].Length = familyName.length() * 2;
		unicodeStrings[2].MaximumLength = familyName.length() * 2;

		RtlInitUnicodeString(&attributeArray[0].Name, L"WIN://SYSAPPID");

		return STATUS_SUCCESS;
	}

	return status;
}

extern "C"
FARPROC WINAPI
LocalDelayLoadHelper2(
	PVOID				imageBase,
	PCImgDelayDescr     pidd,
	FARPROC *           ppfnIATEntry
);

PVOID WINAPI ResolveDelayLoadedAPI_Custom(PVOID ParentModuleBase, PCIMAGE_DELAYLOAD_DESCRIPTOR DelayloadDescriptor,
										  PDELAYLOAD_FAILURE_DLL_CALLBACK FailureDllHook, void* FailureSystemHook, PIMAGE_THUNK_DATA ThunkAddress, ULONG Flags)
{
	void* retval = g_origResolveDelayLoadedAPI(ParentModuleBase, DelayloadDescriptor, FailureDllHook, FailureSystemHook, ThunkAddress, Flags);

	if (!retval)
	{
		if (g_activationLock < 1)
		{
			DWORD oldLock = g_activationLock;
			g_origWaitOnAddress(&g_activationLock, &oldLock, sizeof(g_activationLock));
		}

		retval = LocalDelayLoadHelper2(ParentModuleBase, (PCImgDelayDescr)DelayloadDescriptor, (FARPROC*)ThunkAddress);
	}

	return retval;
}


void InitializeBaseStubs()
{
	LoadLibrary(L"sechost.dll");

	auto peb = NtCurrentPeb();
	wcscpy(peb->ProcessParameters->CommandLine.Buffer, L"-ServerName:App.AppX92m5m1wwhhzwrbzhvdt7whj6b4qqpaqa.mca");// L"-ServerName:OpusReleaseFinal.AppXhz3m1thxb88yczd2ax0k426vzefwnnmc.mca");
	auto a = GetCommandLineW();

	//MH_CreateHookApi(L"kernelbase.dll", "GetCommandLineW", GetCommandLineW_Custom, nullptr);
	//MH_CreateHookApi(L"kernelbase.dll", "GetLastError", GetLastError_Custom, nullptr);
	//MH_CreateHookApi(L"ntdll.dll", "RtlNtStatusToDosError", RtlNtStatusToDosError_Custom, (void**)&g_origNtStatusToDosError);
	MH_CreateHookApi(L"kernelbase.dll", "OpenState", OpenState_Custom, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "CloseState", CloseState_Custom, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "GetStateFolder", GetStateFolder_Custom, nullptr);
	MH_CreateHookApi(L"sechost.dll", "CapabilityCheck", CapabilityCheck_Custom, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "GetTokenInformation", GetTokenInformation_Custom, (void**)&g_origGetTokenInformation);
	MH_CreateHookApi(L"ntdll.dll", "RtlWaitOnAddress", RtlWaitOnAddress_Custom, (void**)&g_origWaitOnAddress);
	MH_CreateHookApi(L"ntdll.dll", "RtlWakeAddressAll", RtlWakeAddressAll_Custom, (void**)&g_origWakeAddressAll);
	MH_EnableHook(MH_ALL_HOOKS);
}

void PrepareActivation();

void PostInitializeBaseStubs()
{
	MH_CreateHookApi(L"ntdll.dll", "NtQueryInformationToken", NtQueryInformationToken_Custom, (void**)&g_origNtQueryInformationToken);
	MH_CreateHookApi(L"ntdll.dll", "LdrResolveDelayLoadedAPI", ResolveDelayLoadedAPI_Custom, (void**)&g_origResolveDelayLoadedAPI);
	MH_EnableHook(MH_ALL_HOOKS);

	RoInitialize(RO_INIT_MULTITHREADED);

	PrepareActivation();

	//MH_CreateHookApi(L"kernelbase.dll", "GetModuleFileNameW", GetModuleFileNameW_Custom, (void**)&g_origGetModuleFileNameW);
	MH_EnableHook(MH_ALL_HOOKS);
}