#define NTDDI_VERSION NTDDI_WINBLUE
#include <windows.h>

#define IPRT_NT_USE_WINTERNL

#include <string>
#include <nt.h>

#include "../../build/elevation_rpc_h.h"

void RunRpcServer()
{
	RpcServerUseProtseqEp(reinterpret_cast<RPC_WSTR>(L"ncacn_np"), RPC_C_LISTEN_MAX_CALLS_DEFAULT, reinterpret_cast<RPC_WSTR>(L"\\pipe\\ImmersiveElevationServer"), nullptr);
	RpcServerRegisterIf(elevation_rpc_v1_0_s_ifspec, nullptr, nullptr);
	RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, FALSE);
}

void StopRpcServer()
{
	RpcMgmtStopServerListening(nullptr);
	RpcServerUnregisterIf(nullptr, nullptr, FALSE);
}

#define TOKEN_SECURITY_ATTRIBUTE_TYPE_INVALID 0x00
#define TOKEN_SECURITY_ATTRIBUTE_TYPE_INT64 0x01
#define TOKEN_SECURITY_ATTRIBUTE_TYPE_UINT64 0x02
#define TOKEN_SECURITY_ATTRIBUTE_TYPE_STRING 0x03
#define TOKEN_SECURITY_ATTRIBUTE_TYPE_FQBN 0x04
#define TOKEN_SECURITY_ATTRIBUTE_TYPE_SID 0x05
#define TOKEN_SECURITY_ATTRIBUTE_TYPE_BOOLEAN 0x06
#define TOKEN_SECURITY_ATTRIBUTE_TYPE_OCTET_STRING 0x10

// Flags

#define TOKEN_SECURITY_ATTRIBUTE_NON_INHERITABLE 0x0001
#define TOKEN_SECURITY_ATTRIBUTE_VALUE_CASE_SENSITIVE 0x0002
#define TOKEN_SECURITY_ATTRIBUTE_USE_FOR_DENY_ONLY 0x0004
#define TOKEN_SECURITY_ATTRIBUTE_DISABLED_BY_DEFAULT 0x0008
#define TOKEN_SECURITY_ATTRIBUTE_DISABLED 0x0010
#define TOKEN_SECURITY_ATTRIBUTE_MANDATORY 0x0020

#define TOKEN_SECURITY_ATTRIBUTE_VALID_FLAGS ( \
    TOKEN_SECURITY_ATTRIBUTE_NON_INHERITABLE | \
    TOKEN_SECURITY_ATTRIBUTE_VALUE_CASE_SENSITIVE | \
    TOKEN_SECURITY_ATTRIBUTE_USE_FOR_DENY_ONLY | \
    TOKEN_SECURITY_ATTRIBUTE_DISABLED_BY_DEFAULT | \
    TOKEN_SECURITY_ATTRIBUTE_DISABLED | \
    TOKEN_SECURITY_ATTRIBUTE_MANDATORY)

#define TOKEN_SECURITY_ATTRIBUTE_CUSTOM_FLAGS 0xffff0000

// end_rev

// private
typedef struct _TOKEN_SECURITY_ATTRIBUTE_FQBN_VALUE
{
	ULONG64 Version;
	UNICODE_STRING Name;
} TOKEN_SECURITY_ATTRIBUTE_FQBN_VALUE, *PTOKEN_SECURITY_ATTRIBUTE_FQBN_VALUE;

// private
typedef struct _TOKEN_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE
{
	PVOID pValue;
	ULONG ValueLength;
} TOKEN_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE, *PTOKEN_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE;

// private
typedef struct _TOKEN_SECURITY_ATTRIBUTE_V1
{
	UNICODE_STRING Name;
	USHORT ValueType;
	USHORT Reserved;
	ULONG Flags;
	ULONG ValueCount;
	union
	{
		PLONG64 pInt64;
		PULONG64 pUint64;
		PUNICODE_STRING pString;
		PTOKEN_SECURITY_ATTRIBUTE_FQBN_VALUE pFqbn;
		PTOKEN_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE pOctetString;
	} Values;
} TOKEN_SECURITY_ATTRIBUTE_V1, *PTOKEN_SECURITY_ATTRIBUTE_V1;

// rev
#define TOKEN_SECURITY_ATTRIBUTES_INFORMATION_VERSION_V1 1
// rev
#define TOKEN_SECURITY_ATTRIBUTES_INFORMATION_VERSION TOKEN_SECURITY_ATTRIBUTES_INFORMATION_VERSION_V1

// private
typedef struct _TOKEN_SECURITY_ATTRIBUTES_INFORMATION
{
	USHORT Version;
	USHORT Reserved;
	ULONG AttributeCount;
	union
	{
		PTOKEN_SECURITY_ATTRIBUTE_V1 pAttributeV1;
	} Attribute;
} TOKEN_SECURITY_ATTRIBUTES_INFORMATION, *PTOKEN_SECURITY_ATTRIBUTES_INFORMATION;

struct TOKEN_SECURITY_OPERATIONS_AND_ATTRIBUTES
{
	TOKEN_SECURITY_ATTRIBUTES_INFORMATION* attributes;
	DWORD* operations;
};

extern "C" NTSTATUS NTAPI NtSetInformationToken(HANDLE, TOKEN_INFORMATION_CLASS, PVOID, ULONG);

#pragma comment(lib, "ntdll.lib")

error_status_t set_token_appid(
	/* [in] */ handle_t rpc_handle,
	/* [in] */ unsigned long process_id,
	/* [string][in] */ const wchar_t *full_name,
	/* [string][in] */ const wchar_t *app_id,
	/* [string][in] */ const wchar_t *family_name)
{
	std::wstring g_appId = app_id;
	std::wstring g_fullName = full_name;
	std::wstring familyName = family_name;

	// TODO: don't use this weird allocation method it's useless

	size_t returnLength = sizeof(TOKEN_SECURITY_ATTRIBUTES_INFORMATION) + (sizeof(TOKEN_SECURITY_ATTRIBUTE_V1) * 2) + (sizeof(UNICODE_STRING) * 4) +
		(g_appId.length() * sizeof(WCHAR)) + (g_fullName.length() * sizeof(WCHAR)) +
		(familyName.length() * sizeof(WCHAR)) + (wcslen(L"WIN://SYSAPPID") * sizeof(WCHAR));

	VOID* TokenInformation = malloc(returnLength);
	memset(TokenInformation, 0, returnLength);

	TOKEN_SECURITY_ATTRIBUTES_INFORMATION* attributes = reinterpret_cast<TOKEN_SECURITY_ATTRIBUTES_INFORMATION*>(TokenInformation);

	TOKEN_SECURITY_ATTRIBUTE_V1* attributeArray = reinterpret_cast<TOKEN_SECURITY_ATTRIBUTE_V1*>(attributes + 1);
	attributes->AttributeCount = 2;
	attributes->Version = 1;
	attributes->Attribute.pAttributeV1 = attributeArray;

	PUNICODE_STRING unicodeStrings = reinterpret_cast<PUNICODE_STRING>(attributeArray + 2);

	attributeArray[0].ValueCount = 3;
	attributeArray[0].ValueType = TOKEN_SECURITY_ATTRIBUTE_TYPE_STRING;
	attributeArray[0].Values.pString = unicodeStrings;

	uint64_t yes = 1;

	attributeArray[1].ValueCount = 1;
	attributeArray[1].ValueType = TOKEN_SECURITY_ATTRIBUTE_TYPE_UINT64;
	attributeArray[1].Values.pUint64 = &yes;

	wchar_t* charBuffers = reinterpret_cast<wchar_t*>(unicodeStrings + 4);
	wcsncpy(charBuffers, g_fullName.c_str(), g_fullName.length());

	unicodeStrings[0].Buffer = charBuffers;
	unicodeStrings[0].Length = g_fullName.length() * 2;
	unicodeStrings[0].MaximumLength = g_fullName.length() * 2;

	charBuffers += g_fullName.length();

	wcsncpy(charBuffers, g_appId.c_str(), g_appId.length());

	unicodeStrings[1].Buffer = charBuffers;
	unicodeStrings[1].Length = g_appId.length() * 2;
	unicodeStrings[1].MaximumLength = g_appId.length() * 2;

	charBuffers += g_appId.length();

	wcsncpy(charBuffers, familyName.c_str(), familyName.length());

	unicodeStrings[2].Buffer = charBuffers;
	unicodeStrings[2].Length = familyName.length() * 2;
	unicodeStrings[2].MaximumLength = familyName.length() * 2;

	charBuffers += familyName.length();

	wcsncpy(charBuffers, L"WIN://SYSAPPID", 14);

	attributeArray[0].Name.Buffer = charBuffers;
	attributeArray[0].Name.Length = 14 * 2;
	attributeArray[0].Name.MaximumLength = 14 * 2;

	RtlInitUnicodeString(&attributeArray[1].Name, L"WIN://PKG");

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process_id);

	if (!hProcess)
	{
		return GetLastError();
	}

	HANDLE hToken;
	NTSTATUS status = NtOpenProcessToken(hProcess, 136, &hToken);

	if (!NT_SUCCESS(status))
	{
		CloseHandle(hProcess);
		return RtlNtStatusToDosError(status);
	}

	DWORD isAppContainer = 0;
	DWORD size = sizeof(isAppContainer);
	GetTokenInformation(hToken, TokenIsAppContainer, &isAppContainer, sizeof(isAppContainer), &size);

	if (!isAppContainer)
	{
		CloseHandle(hToken);
		CloseHandle(hProcess);

		return ERROR_APPCONTAINER_REQUIRED;
	}

	DWORD operations[] = { 4, 4 };

	TOKEN_SECURITY_OPERATIONS_AND_ATTRIBUTES tiAA;
	tiAA.operations = operations;
	tiAA.attributes = attributes;

	status = NtSetInformationToken(hToken, TokenSecurityAttributes, &tiAA, sizeof(tiAA));

	CloseHandle(hToken);
	CloseHandle(hProcess);

	if (!NT_SUCCESS(status))
	{
		return RtlNtStatusToDosError(status);
	}

	return 0;
}

#include <netfw.h>
#include <wrl.h>
#include <psapi.h>
#include <userenv.h>
#include <sddl.h>
#include <mutex>
#include <MinHook.h>

#pragma comment(lib, "userenv.lib")

using namespace Microsoft::WRL;

template<typename T>
struct LocalAlloc_delete
{
	LocalAlloc_delete() {}
	void operator()(T* p) throw() { LocalFree(p); }
};

template<typename T>
using unique_localptr = std::unique_ptr<T, LocalAlloc_delete<T>>;

static DWORD(NTAPI* g_origFwOpenPolicyStore)(WORD clientVersion, void* a2, int fwStoreType, int fwAccessRight, DWORD flags, void** policyStore);

// reference: https://msdn.microsoft.com/en-us/library/cc231498.aspx (internal IDL)
static DWORD FWOpenPolicyStore_Wrap(WORD clientVersion, void* a2, int fwStoreType, int fwAccessRight, DWORD flags, void** policyStore)
{
	if (fwStoreType == 2) // FW_STORE_TYPE_LOCAL
	{
		fwStoreType = 5; // FW_STORE_TYPE_DYNAMIC
	}

	return g_origFwOpenPolicyStore(clientVersion, a2, fwStoreType, fwAccessRight, flags, policyStore);
}

error_status_t update_firewall_rule(handle_t rpc_handle, unsigned long process_id, const wchar_t *orig_family_name)
{
	// hook stuff to actually use the dynamic policy and not the local one
	static std::once_flag of;
	std::call_once(of, [] ()
	{
		LoadLibrary(L"firewallapi.dll");

		MH_Initialize();
		MH_CreateHookApi(L"firewallapi.dll", "FWOpenPolicyStore", FWOpenPolicyStore_Wrap, (void**)&g_origFwOpenPolicyStore);
		MH_EnableHook(MH_ALL_HOOKS);
	});

	// get the SID for the package family
	PSID sid;

	wchar_t* sidStringResult;
	DeriveAppContainerSidFromAppContainerName(orig_family_name, &sid);

	// convert to string
	ConvertSidToStringSid(sid, &sidStringResult);

	// get a pointer to the string
	unique_localptr<wchar_t> sidString(sidStringResult);
	sidStringResult = nullptr;

	// free
	FreeSid(sid);

	// get the parent process path
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);

	if (hProcess == INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	// get filename
	wchar_t processFileName[1024] = { 0 };
	GetModuleFileNameEx(hProcess, nullptr, processFileName, _countof(processFileName) - 1);

	// close process handle
	CloseHandle(hProcess);

	// initialize COM
	HRESULT hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

	// check failure
	if (FAILED(hr) && hr != RPC_E_CHANGED_MODE)
	{
		return hr;
	}

	// CoInitialize constructor scope - see https://blogs.msdn.microsoft.com/oldnewthing/20040520-00/?p=39243
	// this is actually an important dummy scope for once :-)
	{
		// initialize the firewall API
		ComPtr<INetFwPolicy2> policy;
		hr = CoCreateInstance(__uuidof(NetFwPolicy2), nullptr, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)policy.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<INetFwRules> rules;
			hr = policy->get_Rules(rules.GetAddressOf());

			if (SUCCEEDED(hr))
			{
				long numRules;
				hr = rules->get_Count(&numRules);

				if (SUCCEEDED(hr))
				{
					ComPtr<IUnknown> enumerator;
					ComPtr<IEnumVARIANT> variantEnum;
					hr = rules->get__NewEnum(enumerator.GetAddressOf());
					enumerator.As(&variantEnum);

					if (SUCCEEDED(hr))
					{
						VARIANT var;
						ULONG cFetched;

						while (variantEnum->Next(1, &var, &cFetched) == S_OK)
						{
							ComPtr<INetFwRule3> rule;
							if (SUCCEEDED(var.pdispVal->QueryInterface<INetFwRule3>(rule.GetAddressOf())))
							{
								BSTR name;
								rule->get_Name(&name);

								if (wcsstr(name, L"Xbox")) // hoping this actually doesn't translated in localized Windows versions
								{
									BSTR appPackageId;
									if (SUCCEEDED(rule->get_LocalAppPackageId(&appPackageId)))
									{
										if (appPackageId)
										{
											if (_wcsnicmp(appPackageId, sidString.get(), SysStringLen(appPackageId)) == 0)
											{
												BSTR procName = SysAllocString(processFileName);
												HRESULT hr = rule->put_ApplicationName(procName);

												if (SUCCEEDED(hr))
												{
													hr = rule->put_LocalAppPackageId(nullptr);

													if (SUCCEEDED(hr))
													{
														hr = 0;
													}
												}

												SysFreeString(procName);
											}

											SysFreeString(appPackageId);
										}
									}
								}

								SysFreeString(name);
							}

							VariantClear(&var);
						}
					}
				}
			}
		}
	}

	CoUninitialize();

	return hr;
}

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
	return malloc(len);
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
	free(ptr);
}
