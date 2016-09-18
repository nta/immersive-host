#include "StdInc.h"
#include <roapi.h>

#include "PackageIdentity.h"

#include "../../build/IActivationProxy_h.h"
#include "../../build/IActivationProxy_i.c"
#include <OleAuto.h>

#include <wrl.h>

#include <concurrent_unordered_map.h>
#include <functional>

static concurrency::concurrent_unordered_map<std::wstring, std::function<HRESULT(IUnknown*, IActivationFactory**)>> g_activationProxies;

using namespace Microsoft::WRL;

enum WINDOW_TYPE { IMMERSIVE_BODY = 0x0, IMMERSIVE_DOCK = 0x1, IMMERSIVE_HOSTED = 0x2, IMMERSIVE_TEST = 0x3, IMMERSIVE_BODY_ACTIVE = 0x4, IMMERSIVE_DOCK_ACTIVE = 0x5 };

#include <windows.ui.core.h>
#include <Windows.UI.Core.CoreWindowFactory.h>

using namespace ABI::Windows::UI::Core;

class RCoreWindow : public RuntimeClass<ICoreWindow, ICoreWindow2>
{
private:
	ComPtr<ICoreWindow> m_original;
	ComPtr<ICoreWindow2> m_original2;

public:
	RCoreWindow(ComPtr<ICoreWindow> origPtr)
	{
		origPtr.As(&m_original);
		origPtr.As(&m_original2);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AutomationHostProvider(
		/* [out][retval] */ __RPC__deref_out_opt IInspectable **value)
	{
		return m_original->get_AutomationHostProvider(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Bounds(
		/* [out][retval] */ __RPC__out ABI::Windows::Foundation::Rect *value)
	{
		return m_original->get_Bounds(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CustomProperties(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Foundation::Collections::IPropertySet **value)
	{
		return m_original->get_CustomProperties(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Dispatcher(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::UI::Core::ICoreDispatcher **value)
	{
		return m_original->get_Dispatcher(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FlowDirection(
		/* [out][retval] */ __RPC__out ABI::Windows::UI::Core::CoreWindowFlowDirection *value)
	{
		return m_original->get_FlowDirection(value);
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_FlowDirection(
		/* [in] */ ABI::Windows::UI::Core::CoreWindowFlowDirection value)
	{
		return m_original->put_FlowDirection(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsInputEnabled(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original->get_IsInputEnabled(value);
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IsInputEnabled(
		/* [in] */ boolean value)
	{
		return m_original->put_IsInputEnabled(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PointerCursor(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::UI::Core::ICoreCursor **value)
	{
		return m_original->get_PointerCursor(value);
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PointerCursor(
		/* [in] */ __RPC__in_opt ABI::Windows::UI::Core::ICoreCursor *value)
	{
		return m_original->put_PointerCursor(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PointerPosition(
		/* [out][retval] */ __RPC__out ABI::Windows::Foundation::Point *value)
	{
		return m_original->get_PointerPosition(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Visible(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original->get_Visible(value);
	}

	virtual HRESULT STDMETHODCALLTYPE Activate(void)
	{
		/*HString hStr;
		hStr.Set(RuntimeClass_Windows_UI_ViewManagement_ApplicationView);

		ComPtr<IApplicationViewStatics> applicationView;
		ImHost_GetActivationFactory(hStr.Get(), __uuidof(IApplicationViewStatics), (void**)applicationView.GetAddressOf());

		ComPtr<IApplicationViewStatics3> windowView;
		applicationView.As(&windowView);

		Size size = { 1024.0f, 768.0f };
		windowView->get_PreferredLaunchViewSize(&size);

		ApplicationViewWindowingMode windowingMode;
		windowView->get_PreferredLaunchWindowingMode(&windowingMode);

		HWND hWnd = ImHost_GetCoreWindowHandle();
		RECT rect;
		GetWindowRect(hWnd, &rect);

		int x = rect.left;
		int y = rect.top;
		int w = static_cast<int>(size.Width);
		int h = static_cast<int>(size.Height);

		if (windowingMode == ApplicationViewWindowingMode_FullScreen)
		{
			x = 0;
			y = 0;

			w = 2560;
			h = 1440;
		}

		MoveWindow(hWnd, x, y, w, h, TRUE);

		if (windowingMode != ApplicationViewWindowingMode_FullScreen)
		{
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_OVERLAPPED);
		}

		ShowWindow(ImHost_GetCoreWindowHandle(), SW_SHOW);*/

		return m_original->Activate();
	}

	virtual HRESULT STDMETHODCALLTYPE Close(void)
	{
		return m_original->Close();
	}

	virtual HRESULT STDMETHODCALLTYPE GetAsyncKeyState(
		/* [in] */ ABI::Windows::System::VirtualKey virtualKey,
		/* [out][retval] */ __RPC__out ABI::Windows::UI::Core::CoreVirtualKeyStates *KeyState)
	{
		return m_original->GetAsyncKeyState(virtualKey, KeyState);
	}

	virtual HRESULT STDMETHODCALLTYPE GetKeyState(
		/* [in] */ ABI::Windows::System::VirtualKey virtualKey,
		/* [out][retval] */ __RPC__out ABI::Windows::UI::Core::CoreVirtualKeyStates *KeyState)
	{
		return m_original->GetKeyState(virtualKey, KeyState);
	}

	virtual HRESULT STDMETHODCALLTYPE ReleasePointerCapture(void)
	{
		return m_original->ReleasePointerCapture();
	}

	virtual HRESULT STDMETHODCALLTYPE SetPointerCapture(void)
	{
		return m_original->SetPointerCapture();
	}

	virtual HRESULT STDMETHODCALLTYPE add_Activated(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CWindowActivatedEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_Activated(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_Activated(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_Activated(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_AutomationProviderRequested(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CAutomationProviderRequestedEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *cookie)
	{
		return m_original->add_AutomationProviderRequested(handler, cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_AutomationProviderRequested(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_AutomationProviderRequested(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_CharacterReceived(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CCharacterReceivedEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_CharacterReceived(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_CharacterReceived(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_CharacterReceived(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_Closed(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CCoreWindowEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_Closed(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_Closed(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_Closed(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_InputEnabled(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CInputEnabledEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_InputEnabled(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_InputEnabled(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_InputEnabled(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_KeyDown(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CKeyEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_KeyDown(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_KeyDown(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_KeyDown(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_KeyUp(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CKeyEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_KeyUp(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_KeyUp(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_KeyUp(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_PointerCaptureLost(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *cookie)
	{
		return m_original->add_PointerCaptureLost(handler, cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_PointerCaptureLost(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_PointerCaptureLost(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_PointerEntered(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *cookie)
	{
		return m_original->add_PointerEntered(handler, cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_PointerEntered(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_PointerEntered(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_PointerExited(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *cookie)
	{
		return m_original->add_PointerExited(handler, cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_PointerExited(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_PointerExited(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_PointerMoved(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *cookie)
	{
		return m_original->add_PointerMoved(handler, cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_PointerMoved(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_PointerMoved(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_PointerPressed(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *cookie)
	{
		return m_original->add_PointerPressed(handler, cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_PointerPressed(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_PointerPressed(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_PointerReleased(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *cookie)
	{
		return m_original->add_PointerReleased(handler, cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_PointerReleased(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_PointerReleased(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_TouchHitTesting(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CTouchHitTestingEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_TouchHitTesting(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_TouchHitTesting(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_TouchHitTesting(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_PointerWheelChanged(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CPointerEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *cookie)
	{
		return m_original->add_PointerWheelChanged(handler, cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_PointerWheelChanged(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_PointerWheelChanged(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_SizeChanged(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CWindowSizeChangedEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_SizeChanged(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_SizeChanged(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_SizeChanged(cookie);
	}

	virtual HRESULT STDMETHODCALLTYPE add_VisibilityChanged(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CVisibilityChangedEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *pCookie)
	{
		return m_original->add_VisibilityChanged(handler, pCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_VisibilityChanged(
		/* [in] */ EventRegistrationToken cookie)
	{
		return m_original->remove_VisibilityChanged(cookie);
	}

	// ICoreWindow2
	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PointerPosition(
		/* [in] */ ABI::Windows::Foundation::Point value)
	{
		return m_original2->put_PointerPosition(value);
	}
};


class CoreWindowFactoryInt : public RuntimeClass<ICoreWindowFactory>
{
public:
	STDMETHOD(CreateCoreWindow)(__RPC__in HSTRING windowTitle, __RPC__deref_out_opt ABI::Windows::UI::Core::ICoreWindow **window)
	{
		HMODULE uiCore = LoadLibrary(L"Windows.UI.dll");
		auto privateCreateCoreWindow = (HRESULT(*)(WINDOW_TYPE, const wchar_t* title, int x, int y, int width, int height, int attributes, HWND owner, const IID& guid, void** ppv))GetProcAddress(uiCore, MAKEINTRESOURCEA(1500));

		std::shared_ptr<PackageIdentity> packageIdentity = GetCurrentPackageIdentity();

		HWND hParent = nullptr;

		std::wstring wt = WindowsGetStringRawBuffer(windowTitle, nullptr);
		swscanf(wt.c_str(), L"%p", &hParent);

		ComPtr<ICoreWindow> coreWindow;
		HRESULT hr = privateCreateCoreWindow(IMMERSIVE_BODY, packageIdentity->GetDisplayName().c_str(), 0, 0, 1024, 768, 0, hParent, IID_ICoreWindow, (void**)coreWindow.GetAddressOf());

		auto cw = Make<RCoreWindow>(coreWindow);

		cw.CopyTo(window);

		return hr;
	}

	STDMETHOD(get_WindowReuseAllowed)(__RPC__out boolean *value)
	{
		*value = true;

		return S_OK;
	}
};

class ActivationProxyWrapper : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IActivationProxy>
{
public:
	STDMETHOD(GetActivationFactory)(BSTR className, IUnknown** factory)
	{
		HSTRING hString;
		HRESULT hr;

		if (!_wcsicmp(className, L"InternalCoreWindowFactory"))
		{
			auto f = Make<CoreWindowFactoryInt>();
			f.CopyTo(factory);

			return S_OK;
		}

		hr = WindowsCreateString(className, SysStringLen(className), &hString);

		if (SUCCEEDED(hr))
		{
			hr = RoGetActivationFactory(hString, IID_IInspectable, (void**)factory);
		}

		{
			auto it = g_activationProxies.find(className);

			if (it != g_activationProxies.end())
			{
				ComPtr<IActivationFactory> af;

				if (SUCCEEDED(it->second((IUnknown*)*factory, af.GetAddressOf())))
				{
					af.CopyTo(factory);
				}
			}
		}

		if (hr == E_NOINTERFACE)
		{
			if (!_wcsicmp(className, L"Windows.Internal.Security.Authentication.Web.TokenBrokerInternal"))
			{
				HMODULE hModule = LoadLibrary(L"tokenbroker.dll");
				auto gaf = (HRESULT(WINAPI*)(HSTRING, IActivationFactory**))GetProcAddress(hModule, "DllGetActivationFactory");

				ComPtr<IActivationFactory> af;
				hr = gaf(hString, af.GetAddressOf());

				if (SUCCEEDED(hr))
				{
					af.CopyTo(IID_IUnknown, (void**)factory);
				}
			}
		}
		
		WindowsDeleteString(hString);

		return hr;
	}

	STDMETHOD(VerifyProxy)()
	{
		HMODULE uiCore = LoadLibrary(L"Windows.UI.dll");
		auto privateCreateCoreWindow = (HRESULT(*)(WINDOW_TYPE, const wchar_t* title, int x, int y, int width, int height, int attributes, HWND owner, const IID& guid, void** ppv))GetProcAddress(uiCore, MAKEINTRESOURCEA(1500));

		ICoreWindow* windowPtr;
		//HRESULT hr = privateCreateCoreWindow(IMMERSIVE_BODY, L"ImmersiveHost (subhost)", 30, 30, 1024, 768, 0, nullptr, IID_ICoreWindow, (void**)&windowPtr);
		HRESULT hr = privateCreateCoreWindow(IMMERSIVE_BODY, L"ImmersiveHost (subhost)", 30, 30, 1024, 768, 0, nullptr, IID_ICoreWindow, (void**)&windowPtr);

		//windowPtr->Activate();

		if (FAILED(hr))
		{
			return hr;
		}

		return 0x80071234;
	}
};

HRESULT RunActivationServer(HANDLE hPipe, HANDLE hParent);
void MakeXBLActivationProxies();

void ImHost_AddActivationProxy(const std::wstring& id, const std::function<HRESULT(IUnknown*, IActivationFactory**)>& function)
{
	g_activationProxies.insert({ id, function });
}

HRESULT RunActivationServer(const char* pipeHandleStr, const char* parentHandleStr)
{
	MakeXBLActivationProxies();

	HANDLE pipeHandle;
	sscanf(pipeHandleStr, "%p", &pipeHandle);

	HANDLE parentHandle;
	sscanf(parentHandleStr, "%p", &parentHandle);

	return RunActivationServer(pipeHandle, parentHandle);
}

#include <string>

HRESULT RunActivationServer(HANDLE hPipe, HANDLE hParent)
{
	RoInitialize(RO_INIT_SINGLETHREADED);

	HRESULT hr;

	ComPtr<ActivationProxyWrapper> activationProxy = Make<ActivationProxyWrapper>();
	
	{
		ComPtr<IStream> stream;
		hr = CreateStreamOnHGlobal(nullptr, TRUE, stream.GetAddressOf());

		ComPtr<IUnknown> unknown;
		activationProxy.AsIID(IID_IActivationProxy, &unknown);

		hr = CoMarshalInterface(stream.Get(), IID_IActivationProxy, unknown.Get(), MSHCTX_LOCAL, nullptr, MSHLFLAGS_NORMAL);

		if (FAILED(hr))
		{
			return hr;
		}

		STATSTG stat;
		stream->Stat(&stat, STATFLAG_NONAME);

		DWORD bytesWritten;
		if (!WriteFile(hPipe, &stat.cbSize.QuadPart, sizeof(stat.cbSize.QuadPart), &bytesWritten, nullptr))
		{
			return 0x80070000 | GetLastError();
		}

		LARGE_INTEGER seekTarget;
		seekTarget.QuadPart = 0;

		hr = stream->Seek(seekTarget, STREAM_SEEK_SET, nullptr);

		ULONG bytesRead;

		std::vector<uint8_t> dataWrite(stat.cbSize.QuadPart);
		hr = stream->Read(&dataWrite[0], stat.cbSize.QuadPart, &bytesRead);

		if (!WriteFile(hPipe, &dataWrite[0], dataWrite.size(), &bytesWritten, nullptr))
		{
			return 0x80070000 | GetLastError();
		}
	}

	{
		DWORD index;
		hr = CoWaitForMultipleHandles(COWAIT_DISPATCH_CALLS | COWAIT_DISPATCH_WINDOW_MESSAGES, INFINITE, 1, &hParent, &index);
	}

	return hr;
}

void PrepareActivation()
{
	OaEnablePerUserTLibRegistration();

	ITypeLib* tlb;
	if (SUCCEEDED(LoadTypeLib(L"ImmersiveHost.tlb", &tlb)))
	{
		wchar_t path[MAX_PATH];
		//GetModuleFileName(GetModuleHandle(nullptr), path, _countof(path));
		GetWindowsDirectory(path, _countof(path));

		//wcsrchr(path, '\\')[1] = L'\0';
		wcscat(path, L"\\system32\\");
		wcscat(path, L"ImmersiveHost.tlb");

		//HRESULT hr = RegisterTypeLib(tlb, L"B:\\dev\\immersive-host\\bin\\debug\\x86_64\\ImmersiveHost.tlb", nullptr);
		HRESULT hr = RegisterTypeLib(tlb, path, nullptr);

		tlb->Release();
	}
}

#include <assert.h>

HRESULT GetActivationClient(HANDLE hPipe, IActivationProxy** proxy)
{
	// wait until we read a byte count
	uint64_t byteCount = 0;
	DWORD bytesRead;
	ReadFile(hPipe, &byteCount, sizeof(byteCount), &bytesRead, nullptr);

	// read the actual byte data
	HRESULT hr = S_OK;
	HGLOBAL globalHandle = GlobalAlloc(0, byteCount);

	LPVOID byteData = GlobalLock(globalHandle);
	ReadFile(hPipe, byteData, byteCount, &bytesRead, nullptr);

	assert(bytesRead == byteCount);

	GlobalUnlock(byteData);

	// create a stream
	ComPtr<IStream> stream;
	CreateStreamOnHGlobal(globalHandle, TRUE, stream.GetAddressOf());

	ComPtr<IActivationProxy> proxyLocal;
	hr = CoUnmarshalInterface(stream.Get(), IID_IActivationProxy, (void**)proxyLocal.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		hr = proxyLocal.CopyTo(proxy);
	}

	return hr;
}