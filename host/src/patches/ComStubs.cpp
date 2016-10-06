#include <StdInc.h>
#include <PackageIdentity.h>

#include <interfaces/CallingProcessInfo.h>
#include <interfaces/RuntimeBroker.h>

#include <MinHook.h>

#include <concurrent_vector.h>
#include <concurrent_unordered_map.h>

static concurrency::concurrent_vector<std::function<void()>> g_activationCallbacks;
static concurrency::concurrent_unordered_map<std::wstring, std::function<HRESULT(HSTRING, IActivationFactory**, void*)>> g_activationFactories;

static HRESULT(WINAPI* g_origGetActivatableClasses)(HSTRING serverName, HSTRING** activatableClassIds, DWORD* count);
static HRESULT(WINAPI* g_origCoInitializeEx)(LPVOID a1, DWORD a2);
static HRESULT(WINAPI* g_origCoCreateInstance)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv);
static HRESULT(WINAPI* g_origActivateInstance)(HSTRING activatableClassId, IInspectable ** instance);
static HRESULT(WINAPI* g_origCoGetCallContext)(REFIID riid, void** ppInterface);
static HRESULT(WINAPI* g_origGetActivationFactory)(HSTRING activatableClassId, REFIID iid, void** factory);

HRESULT ImHost_GetActivationFactory(HSTRING activatableClassId, REFIID iid, void** factory)
{
	return g_origGetActivationFactory(activatableClassId, iid, factory);
}

HRESULT WINAPI RoGetServerActivatableClasses_Custom(HSTRING serverName, HSTRING** activatableClassIds, DWORD* count)
{
	HRESULT hr = g_origGetActivatableClasses(serverName, activatableClassIds, count);

	if (hr == REGDB_E_CLASSNOTREG)
	{
		const auto& applicationName = GetCurrentPackageIdentity()->GetPrimaryAppId();

		HSTRING* classIds = reinterpret_cast<HSTRING*>(CoTaskMemAlloc(sizeof(HSTRING) * 1));
		WindowsCreateString(applicationName.c_str(), applicationName.length(), &classIds[0]);

		*count = 1;
		*activatableClassIds = classIds;

		hr = S_OK;
	}

	return hr;
}

static HRESULT(WINAPI* g_origRegisterActivationFactories)(HSTRING* activatableClassIds, PFNGETACTIVATIONFACTORY* activationFactoryCallbacks, UINT32 count, RO_REGISTRATION_COOKIE* cookie);

HRESULT WINAPI RoRegisterActivationFactories_Custom(HSTRING* activatableClassIds, PFNGETACTIVATIONFACTORY* activationFactoryCallbacks, UINT32 count, RO_REGISTRATION_COOKIE* cookie)
{
	HRESULT origHr = g_origRegisterActivationFactories(activatableClassIds, activationFactoryCallbacks, count, cookie);

	for (UINT32 i = 0; i < count; i++)
	{
		g_activationFactories.insert({ WindowsGetStringRawBuffer(activatableClassIds[i], nullptr), std::bind(activationFactoryCallbacks[i], std::placeholders::_1, std::placeholders::_2) });
	}

	// invoke callbacks
	for (auto& entry : g_activationCallbacks)
	{
		entry();
	}

	/*std::wstring id = WindowsGetStringRawBuffer(activatableClassIds[0], nullptr);

	std::thread([=] ()
	{
		HSTRING string;
		WindowsCreateString(id.c_str(), id.length(), &string);

		ComPtr<IActivatableApplication> factory;
		RoGetActivationFactory(string, GUID_IActivatableApplication, (void**)factory.ReleaseAndGetAddressOf());

		ComPtr<ICoreWindowFactory> windowFactory = Make<CoreWindowFactory>();
		ComPtr<IActivatedEventArgs> eventArgs = Make<ActivatedEventArgs>();

		factory->Activate(windowFactory.Get(), string, eventArgs.Get());
	}).detach();*/

	/*g_activationFactories.insert({ L"Windows.Gaming.XboxLive.Storage.GameSaveProvider", AutoActivation });
	g_activationFactories.insert({ L"Windows.Internal.Security.Authentication.XboxLive.XboxLiveTokenBrokerExtension", AutoActivation });
	g_activationFactories.insert({ L"Windows.Networking.XboxLive.XboxLiveDeviceAddress", AutoActivation });
	g_activationFactories.insert({ L"Windows.Networking.XboxLive.XboxLiveEndpointPair", AutoActivation });
	g_activationFactories.insert({ L"Windows.Networking.XboxLive.XboxLiveEndpointPairTemplate", AutoActivation });
	g_activationFactories.insert({ L"Windows.Networking.XboxLive.XboxLiveQualityOfServiceMeasurement", AutoActivation });*/
	//g_activationFactories.insert({ L"Windows.ApplicationModel.ExtendedExecution.ExtendedExecutionSession", AutoActivation });
	//g_activationFactories.insert({ L"Windows.ApplicationModel.ExtendedExecution.Foreground.ExtendedExecutionForegroundSession", AutoActivation });
	//g_activationFactories.insert({ L"Windows.Internal.Security.Credentials.WebAccountProviderInternal", AutoActivation });

	return S_OK;
}

HRESULT WINAPI RoGetActivationFactory_Custom(HSTRING activatableClassId, REFIID iid, void** factory)
{
	HRESULT hr;
	auto it = g_activationFactories.find(WindowsGetStringRawBuffer(activatableClassId, nullptr));

	wchar_t buffer[1024];
	wsprintf(buffer, L"GAF: %s\n", WindowsGetStringRawBuffer(activatableClassId, nullptr));
	OutputDebugStringW(buffer);

	if (wcsstr(WindowsGetStringRawBuffer(activatableClassId, nullptr), L"CoreApplication"))
	{
		printf("");
	}

	HString overrideString;

	if (it != g_activationFactories.end())
	{
		ComPtr<IActivationFactory> factoryBase;
		hr = it->second(activatableClassId, factoryBase.GetAddressOf(), _ReturnAddress());

		if (SUCCEEDED(hr))
		{
			hr = factoryBase.CopyTo(iid, factory);
		}
	}
	else
	{
		hr = ImHost_GetAppScopedActivationFactory(activatableClassId, iid, factory);

		if (FAILED(hr))
		{
			hr = g_origGetActivationFactory(activatableClassId, iid, factory);
		}
	}

	return hr;
}

HRESULT WINAPI RoActivateInstance_Custom(HSTRING activatableClassId, IInspectable ** instance)
{
	HRESULT hr = S_OK;
	auto it = g_activationFactories.find(WindowsGetStringRawBuffer(activatableClassId, nullptr));

	wchar_t buffer[1024];
	wsprintf(buffer, L"AI: %s\n", WindowsGetStringRawBuffer(activatableClassId, nullptr));
	OutputDebugStringW(buffer);

	if (it != g_activationFactories.end())
	{
		ComPtr<IActivationFactory> factoryBase;
		hr = it->second(activatableClassId, factoryBase.GetAddressOf(), _ReturnAddress());

		if (SUCCEEDED(hr))
		{
			hr = factoryBase->ActivateInstance(instance);
		}
	}
	else
	{
		hr = g_origActivateInstance(activatableClassId, instance);
	}

	return hr;
}

#include <initguid.h>
DEFINE_GUID(IID_ICallingProcessInfo, 0x68C6A1B9, 0xDE39, 0x42C3, 0x8D, 0x28, 0xBF, 0x40, 0xA5, 0x12, 0x65, 0x41);

HRESULT WINAPI CoGetCallContext_Custom(REFIID riid, void** ppInterface)
{
	HRESULT hr = g_origCoGetCallContext(riid, ppInterface);

	if (hr == RPC_E_CALL_COMPLETE || ImHost_IsActivationProxy()) // the latter check is to identify whether we're in the activation proxy
	{
		if (riid == IID_ICallingProcessInfo)
		{
			auto ptr = Make<CallingProcessInfo>();
			ptr->ReturnAddress = _ReturnAddress();
			ptr.CopyTo(riid, ppInterface);

			return S_OK;
		}
	}

	return hr;
}

HRESULT WINAPI CoInitializeEx_Custom(LPVOID a1, DWORD a2)
{
	//g_origCoInitializeEx(a1, a2);

	return S_OK;
}

void CoUninitialize_Custom()
{
	// no-op
}

HRESULT WINAPI CoCreateInstance_Custom(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv)
{
	if (rclsid == CLSID_RuntimeBroker && !ImHost_IsActivationProxy())
	{
		ComPtr<IRuntimeBroker> broker = MakeRuntimeBroker();
		broker.CopyTo(riid, ppv);

		return S_OK;
	}

	return g_origCoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

void ImHost_AddActivationFactoryWithResult(const std::wstring& classId, const std::function<HRESULT(HSTRING, IActivationFactory**, void*)>& factory)
{
	g_activationFactories.insert({ classId, factory });
}

void ImHost_AddActivationCallback(const std::function<void()>& function)
{
	g_activationCallbacks.push_back(function);
}

void InitializeComStubs()
{
	MH_CreateHookApi(L"combase.dll", "CoInitializeEx", CoInitializeEx_Custom, (void**)&g_origCoInitializeEx);
	MH_CreateHookApi(L"combase.dll", "CoUninitialize", CoUninitialize_Custom, nullptr);
	MH_CreateHookApi(L"combase.dll", "CoGetCallContext", CoGetCallContext_Custom, (void**)&g_origCoGetCallContext);
	MH_CreateHookApi(L"combase.dll", "CoCreateInstance", CoCreateInstance_Custom, (void**)&g_origCoCreateInstance);
	MH_CreateHookApi(L"combase.dll", "RoRegisterActivationFactories", RoRegisterActivationFactories_Custom, (void**)&g_origRegisterActivationFactories);
	MH_CreateHookApi(L"combase.dll", "RoGetActivationFactory", RoGetActivationFactory_Custom, (void**)&g_origGetActivationFactory);
	MH_CreateHookApi(L"combase.dll", "RoActivateInstance", RoActivateInstance_Custom, (void**)&g_origActivateInstance);
	MH_CreateHookApi(L"combase.dll", "RoGetServerActivatableClasses", RoGetServerActivatableClasses_Custom, (void**)&g_origGetActivatableClasses);
	MH_EnableHook(MH_ALL_HOOKS);
}