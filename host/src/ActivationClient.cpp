#include "StdInc.h"

#include <assert.h>
#include <sddl.h>
#include <userenv.h>
#include <shellapi.h>

#include <PackageIdentity.h>

#include "../../build/elevation_rpc_h.h"
#include "../../build/IActivationProxy_h.h"

ComPtr<IActivationProxy> g_activationProxy;
static HANDLE g_activationProxyHandle;

void PrepareActivation();
HRESULT GetActivationClient(HANDLE hPipe, IActivationProxy** proxy);

NTSTATUS(NTAPI* _RtlDeriveCapabilitySidsFromName)(PUNICODE_STRING capabilityName, PSID ntSid, PSID appPackageSid);
extern "C" NTSTATUS NTAPI NtSetInformationToken(HANDLE, TOKEN_INFORMATION_CLASS, PVOID, ULONG);

#pragma comment(lib, "synchronization.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "userenv.lib")

void CreateSub()
{
	_RtlDeriveCapabilitySidsFromName = (decltype(_RtlDeriveCapabilitySidsFromName))GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlDeriveCapabilitySidsFromName");

	SIZE_T size = 0;
	InitializeProcThreadAttributeList(nullptr, 1, 0, &size);

	std::vector<uint8_t> data(size);
	InitializeProcThreadAttributeList(reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(&data[0]), 1, 0, &size);

	std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();

	PSID sid;
	DeriveAppContainerSidFromAppContainerName(identity->GetFamilyName().c_str(), &sid);
	//ConvertStringSidToSid(L"S-1-15-2-416407723-3285746419-2367270041-232825398-2668409381-1607780646-3635468208", &sid); // Delta
	//ConvertStringSidToSid(L"S-1-15-2-1699277692-4065247422-3279742015-3034533294-417409081-2146411952-4219464500", &sid); // TR
	//ConvertStringSidToSid(L"S-1-15-2-3251328282-1839204328-2750637086-2348874566-3932489820-1149607920-1645858330", &sid);

	std::vector<std::unique_ptr<uint8_t>> sids;
	std::vector<SID_AND_ATTRIBUTES> sidCaps;

	auto addCapability = [&] (const std::wstring& capabilityName)
	{
		UNICODE_STRING capabilityNameStr;
		RtlInitUnicodeString(&capabilityNameStr, capabilityName.c_str());

		auto ntSid = std::unique_ptr<uint8_t>(new uint8_t[SECURITY_MAX_SID_SIZE]);
		auto capSid = std::unique_ptr<uint8_t>(new uint8_t[SECURITY_MAX_SID_SIZE]);

		_RtlDeriveCapabilitySidsFromName(&capabilityNameStr, ntSid.get(), capSid.get());

		SID_AND_ATTRIBUTES data;
		data.Sid = capSid.get();
		data.Attributes = SE_GROUP_ENABLED;

		sids.push_back(std::move(capSid));
		sidCaps.push_back(data);
	};

	addCapability(L"internetClient");
	addCapability(L"internetClientServer");
	addCapability(L"codeGeneration");
	addCapability(L"privateNetworkClientServer");
	addCapability(L"userAccountInformation");
	addCapability(identity->GetFamilyName());

	//PSID sid2;
	//ConvertStringSidToSid(L"S-1-15-3-416407723-3285746419-2367270041-232825398-2668409381-1607780646-3635468208", &sid2); // Delta
	//ConvertStringSidToSid(L"S-1-15-3-1699277692-4065247422-3279742015-3034533294-417409081-2146411952-4219464500", &sid2);
	//ConvertStringSidToSid(L"S-1-15-3-3251328282-1839204328-2750637086-2348874566-3932489820-1149607920-1645858330", &sid2);

	/*{
		SID_AND_ATTRIBUTES data;
		data.Sid = sid2;
		data.Attributes = SE_GROUP_ENABLED;

		sidCaps.push_back(data);
	}*/

	static SECURITY_CAPABILITIES securityCapabilities = { 0 };
	securityCapabilities.AppContainerSid = sid;
	securityCapabilities.CapabilityCount = sidCaps.size();
	securityCapabilities.Capabilities = &sidCaps[0];

	BOOL result;
	result = UpdateProcThreadAttribute(reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(&data[0]), 0, PROC_THREAD_ATTRIBUTE_SECURITY_CAPABILITIES, &securityCapabilities, sizeof(securityCapabilities), nullptr, nullptr);

	STARTUPINFOEX startupInfo = { 0 };
	startupInfo.StartupInfo.cb = sizeof(startupInfo);
	startupInfo.lpAttributeList = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(&data[0]);

	HANDLE hReadPipe;
	HANDLE hWritePipe;
	CreatePipe(&hReadPipe, &hWritePipe, nullptr, 0);

	HANDLE hDuplicateWrite;
	HANDLE hParentWait;

	DuplicateHandle(GetCurrentProcess(), hWritePipe, GetCurrentProcess(), &hDuplicateWrite, 0, TRUE, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), &hParentWait, SYNCHRONIZE, TRUE, 0);

	wchar_t args[64];
	wsprintf(args, L"%p %p", hDuplicateWrite, hParentWait);

	std::wstring cli;

	{
		int argc;
		LPWSTR* argv = CommandLineToArgvW(ImHost_GetOriginalCommandLine().c_str(), &argc);

		cli = L"\"" + std::wstring(argv[0]) + L"\"";

		LocalFree(argv);
	}

	cli += L" \"" + identity->GetApplicationRoot() + L"\" -ActivationProxy ";
	cli += args;

	PROCESS_INFORMATION processInfo;
	result = CreateProcess(nullptr, const_cast<wchar_t*>(cli.c_str()), nullptr, nullptr, TRUE, CREATE_SUSPENDED | EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, &startupInfo.StartupInfo, &processInfo);

	DWORD error = GetLastError();

	RPC_WSTR stringBinding = nullptr;
	RpcStringBindingCompose(nullptr, reinterpret_cast<RPC_WSTR>(L"ncacn_np"), nullptr, reinterpret_cast<RPC_WSTR>(L"\\pipe\\ImmersiveElevationServer"), nullptr, &stringBinding);

	RPC_BINDING_HANDLE bindingHandle;
	RpcBindingFromStringBinding(stringBinding, &bindingHandle);

	error_status_t rpcErr = set_token_appid(bindingHandle, processInfo.dwProcessId, identity->GetFullName().c_str(), identity->GetPrimaryAppId().c_str(), identity->GetFamilyName().c_str());

	ResumeThread(processInfo.hThread);
	CloseHandle(processInfo.hThread);

	g_activationProxyHandle = processInfo.hProcess;

	GetActivationClient(hReadPipe, g_activationProxy.GetAddressOf());

	{
		HRESULT result = g_activationProxy->VerifyProxy();

		assert(result == 0x80071234);
	}

	FreeSid(sid);
}

void PostInitializeBaseStubs();
HRESULT RunActivationServer(const char* pipeHandleStr, const char* parentHandleStr);

void RegisterActivationOverrides();

bool ActivationClient_Intercept(int argc, char** argv)
{
	if (argc >= 5)
	{
		if (_stricmp(argv[2], "-ActivationProxy") == 0)
		{
			HRESULT hr = RunActivationServer(argv[3], argv[4]);

			if (FAILED(hr))
			{
				ExitProcess(hr);
			}

			return true;
		}
	}

	PostInitializeBaseStubs();
	CreateSub();

	RegisterActivationOverrides();

	return false;
}

bool ImHost_IsActivationProxy()
{
	return (!g_activationProxy.Get());
}

HANDLE ImHost_GetActivationProxyProcessHandle()
{
	return g_activationProxyHandle;
}

HRESULT WINAPI AutoActivation(HSTRING hString, IActivationFactory** factory)
{
	UINT32 length;
	PCWSTR string = WindowsGetStringRawBuffer(hString, &length);

	BSTR str = SysAllocStringLen(string, length);

	ComPtr<IUnknown> factoryRef;
	HRESULT hr = g_activationProxy->GetActivationFactory(str, factoryRef.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		factoryRef.CopyTo(factory);
	}

	SysFreeString(str);

	return hr;
}

void RegisterActivationOverrides()
{
	ImHost_AddActivationCallback([] ()
	{
		ImHost_AddActivationFactory(L"Windows.Gaming.XboxLive.Storage.GameSaveProvider", AutoActivation);
		ImHost_AddActivationFactory(L"Windows.Internal.Security.Authentication.XboxLive.XboxLiveTokenBrokerExtension", AutoActivation);
		ImHost_AddActivationFactory(L"Windows.Networking.XboxLive.XboxLiveDeviceAddress", AutoActivation);
		ImHost_AddActivationFactory(L"Windows.Networking.XboxLive.XboxLiveEndpointPair", AutoActivation);
		ImHost_AddActivationFactory(L"Windows.Networking.XboxLive.XboxLiveEndpointPairTemplate", AutoActivation);
		ImHost_AddActivationFactory(L"Windows.Networking.XboxLive.XboxLiveQualityOfServiceMeasurement", AutoActivation);
		ImHost_AddActivationFactory(L"Windows.System.Profile.HardwareIdentification", AutoActivation);
	});
}