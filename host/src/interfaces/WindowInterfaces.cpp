#include "StdInc.h"

#include <thread>

#include <PackageIdentity.h>

#include <Windows.ApplicationModel.Activation.h>
#include <Windows.UI.Core.CoreWindowFactory.h>

#include <initguid.h>
DEFINE_GUID(GUID_IActivatableApplication, 0x92696c00, 0x7578, 0x48e1, 0xac, 0x1a, 0x2c, 0xa9, 0x09, 0xe2, 0xc8, 0xcf);

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::ApplicationModel::Activation;
using namespace ABI::Windows::UI::Core;
using namespace ABI::Windows::UI::ViewManagement;

class IActivatableApplication : public IInspectable
{
public:
	STDMETHOD(Activate)(ICoreWindowFactory* windowFactory, HSTRING serverName, IActivatedEventArgs* eventArgs) = 0;
};

enum WINDOW_TYPE { IMMERSIVE_BODY = 0x0, IMMERSIVE_DOCK = 0x1, IMMERSIVE_HOSTED = 0x2, IMMERSIVE_TEST = 0x3, IMMERSIVE_BODY_ACTIVE = 0x4, IMMERSIVE_DOCK_ACTIVE = 0x5 };

static HRESULT(STDMETHODCALLTYPE* g_origActivate)(ICoreWindow* window);

static HRESULT STDMETHODCALLTYPE Activate(ICoreWindow* window)
{
	HString hStr;
	hStr.Set(RuntimeClass_Windows_UI_ViewManagement_ApplicationView);

	ComPtr<IApplicationViewStatics> applicationView;
	RoGetActivationFactory(hStr.Get(), __uuidof(IApplicationViewStatics), (void**)applicationView.GetAddressOf());

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

	std::thread([=] ()
	{
		MoveWindow(hWnd, x, y, w, h, FALSE);

		if (windowingMode != ApplicationViewWindowingMode_FullScreen)
		{
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_CAPTION);
			SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOZORDER);
		}
		else
		{
			SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE) & (~WS_CAPTION)) | WS_POPUP);
			SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOZORDER);
		}

		ShowWindow(ImHost_GetCoreWindowHandle(), SW_SHOW);
	}).detach();

	return g_origActivate(window);
}

static bool g_hookedWindow;

#include "../../build/IActivationProxy_h.h"

extern ComPtr<IActivationProxy> g_activationProxy;

class CoreWindowFactory : public RuntimeClass<ICoreWindowFactory>
{
public:
	STDMETHOD(CreateCoreWindow)(__RPC__in HSTRING windowTitle, __RPC__deref_out_opt ABI::Windows::UI::Core::ICoreWindow **window)
	{
		HMODULE uiCore = LoadLibrary(L"Windows.UI.dll");
		auto privateCreateCoreWindow = (HRESULT(*)(WINDOW_TYPE, const wchar_t* title, int x, int y, int width, int height, int attributes, HWND owner, const IID& guid, void** ppv))GetProcAddress(uiCore, MAKEINTRESOURCEA(1500));

		std::shared_ptr<PackageIdentity> packageIdentity = GetCurrentPackageIdentity();

		ComPtr<ICoreWindow> coreWindow;
		HRESULT hr = privateCreateCoreWindow(IMMERSIVE_BODY, packageIdentity->GetDisplayName().c_str(), 0, 0, 1024, 768, 0, ImHost_GetCoreWindowHandle(), IID_ICoreWindow, (void**)coreWindow.GetAddressOf());

		/*wchar_t buffer[64];
		swprintf(buffer, L"%p", ImHost_GetCoreWindowHandle());

		BSTR str = SysAllocString(L"InternalCoreWindowFactory");

		ComPtr<IUnknown> factoryRef;
		HRESULT hr = g_activationProxy->GetActivationFactory(str, factoryRef.GetAddressOf());

		ComPtr<ICoreWindowFactory> factory;
		factoryRef.As(&factory);

		HString hStr;
		hStr.Set(buffer);

		hr = factory->CreateCoreWindow(hStr.Get(), window);*/

		/*if (!g_hookedWindow)
		{
			uintptr_t* vtbl = *(uintptr_t**)(coreWindow.Get());
			void* activate = (void*)vtbl[18];

			MH_CreateHook(activate, Activate, (void**)&g_origActivate);
			MH_EnableHook(MH_ALL_HOOKS);

			g_hookedWindow = true;
		}*/

		coreWindow.CopyTo(window);

		return hr;
	}

	STDMETHOD(get_WindowReuseAllowed)(__RPC__out boolean *value)
	{
		*value = true;

		return S_OK;
	}
};

class ActivatedEventArgs : public RuntimeClass<IActivatedEventArgs, IPrelaunchActivatedEventArgs, ILaunchActivatedEventArgs>
{
public:
	STDMETHOD(get_Kind)(ActivationKind* value) override
	{
		*value = ActivationKind_Launch;

		return S_OK;
	}

	STDMETHOD(get_PreviousExecutionState)(ApplicationExecutionState* value) override
	{
		*value = ApplicationExecutionState_NotRunning;

		return S_OK;
	}

	STDMETHOD(get_SplashScreen)(ISplashScreen** value) override
	{
		*value = nullptr;

		return E_NOINTERFACE;
	}

	STDMETHOD(get_PrelaunchActivated)(boolean* value) override
	{
		*value = false;

		return S_OK;
	}

	STDMETHOD(get_Arguments)(HSTRING* string) override
	{
		WindowsCreateString(L"", 0, string);

		return S_OK;
	}

	STDMETHOD(get_TileId)(HSTRING* string) override
	{
		WindowsCreateString(L"", 0, string);

		return S_OK;
	}
};

class RApplicationView : public RuntimeClass<IApplicationView, IApplicationView2, IApplicationView3>
{
private:
	ComPtr<IApplicationView> m_original;
	ComPtr<IApplicationView2> m_original2;
	ComPtr<IApplicationView3> m_original3;

public:
	RApplicationView(ComPtr<IApplicationView> origPtr)
	{
		origPtr.As(&m_original);
		origPtr.As(&m_original2);
		origPtr.As(&m_original3);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Orientation(
		/* [out][retval] */ __RPC__out ABI::Windows::UI::ViewManagement::ApplicationViewOrientation *value)
	{
		return m_original->get_Orientation(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AdjacentToLeftDisplayEdge(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original->get_AdjacentToLeftDisplayEdge(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AdjacentToRightDisplayEdge(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original->get_AdjacentToRightDisplayEdge(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsFullScreen(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original->get_IsFullScreen(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsOnLockScreen(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original->get_IsOnLockScreen(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsScreenCaptureEnabled(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original->get_IsScreenCaptureEnabled(value);
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IsScreenCaptureEnabled(
		/* [in] */ boolean value)
	{
		return m_original->put_IsScreenCaptureEnabled(value);
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Title(
		/* [in] */ __RPC__in HSTRING value)
	{
		return m_original->put_Title(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Title(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value)
	{
		return m_original->get_Title(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Id(
		/* [out][retval] */ __RPC__out INT32 *value)
	{
		return m_original->get_Id(value);
	}

	virtual HRESULT STDMETHODCALLTYPE add_Consolidated(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CViewManagement__CApplicationView_Windows__CUI__CViewManagement__CApplicationViewConsolidatedEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *token)
	{
		return m_original->add_Consolidated(handler, token);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_Consolidated(
		/* [in] */ EventRegistrationToken token)
	{
		return m_original->remove_Consolidated(token);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SuppressSystemOverlays(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original2->get_SuppressSystemOverlays(value);
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_SuppressSystemOverlays(
		/* [in] */ boolean value)
	{
		return m_original2->put_SuppressSystemOverlays(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_VisibleBounds(
		/* [out][retval] */ __RPC__out ABI::Windows::Foundation::Rect *value)
	{
		return m_original2->get_VisibleBounds(value);
	}

	virtual HRESULT STDMETHODCALLTYPE add_VisibleBoundsChanged(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_Windows__CUI__CViewManagement__CApplicationView_IInspectable *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *token)
	{
		return m_original2->add_VisibleBoundsChanged(handler, token);
	}

	virtual HRESULT STDMETHODCALLTYPE remove_VisibleBoundsChanged(
		/* [in] */ EventRegistrationToken token)
	{
		return m_original2->remove_VisibleBoundsChanged(token);
	}

	virtual HRESULT STDMETHODCALLTYPE SetDesiredBoundsMode(
		/* [in] */ ABI::Windows::UI::ViewManagement::ApplicationViewBoundsMode boundsMode,
		/* [out][retval] */ __RPC__out boolean *success)
	{
		return m_original2->SetDesiredBoundsMode(boundsMode, success);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DesiredBoundsMode(
		/* [out][retval] */ __RPC__out ABI::Windows::UI::ViewManagement::ApplicationViewBoundsMode *value)
	{
		return m_original2->get_DesiredBoundsMode(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TitleBar(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::UI::ViewManagement::IApplicationViewTitleBar **value)
	{
		return m_original3->get_TitleBar(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FullScreenSystemOverlayMode(
		/* [out][retval] */ __RPC__out ABI::Windows::UI::ViewManagement::FullScreenSystemOverlayMode *value)
	{
		return m_original3->get_FullScreenSystemOverlayMode(value);
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_FullScreenSystemOverlayMode(
		/* [in] */ ABI::Windows::UI::ViewManagement::FullScreenSystemOverlayMode value)
	{
		return m_original3->put_FullScreenSystemOverlayMode(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsFullScreenMode(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		return m_original3->get_IsFullScreenMode(value);
	}

	virtual HRESULT STDMETHODCALLTYPE TryEnterFullScreenMode(
		/* [out][retval] */ __RPC__out boolean *success)
	{
		// TODO: figure out exclusive fullscreen?
		HWND hWnd = ImHost_GetCoreWindowHandle();

		RECT rect;
		GetWindowRect(hWnd, &rect);

		POINT point = { rect.left, rect.top };
		HMONITOR monitor = MonitorFromPoint(point, 0);

		MONITORINFO monitorInfo = { 0 };
		monitorInfo.cbSize = sizeof(monitorInfo);
		GetMonitorInfo(monitor, &monitorInfo);

		SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE) & (~WS_CAPTION)) | WS_POPUP);

		MoveWindow(hWnd, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top, TRUE);
		SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOZORDER);

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE ExitFullScreenMode(void)
	{
		HWND hWnd = ImHost_GetCoreWindowHandle();
		SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE) | WS_CAPTION) & ~(WS_POPUP));
		SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED | SWP_NOZORDER);

		return m_original3->ExitFullScreenMode();
	}

	virtual HRESULT STDMETHODCALLTYPE ShowStandardSystemOverlays(void)
	{
		return m_original3->ShowStandardSystemOverlays();
	}

	virtual HRESULT STDMETHODCALLTYPE TryResizeView(
		/* [in] */ ABI::Windows::Foundation::Size value,
		/* [out][retval] */ __RPC__out boolean *success)
	{
		HWND hWnd = ImHost_GetCoreWindowHandle();

		std::thread([=] ()
		{
			// badbad?
			Sleep(1000);

			RECT rect;
			GetWindowRect(hWnd, &rect);

			rect.right = rect.left + static_cast<int>(value.Width);
			rect.bottom = rect.top + static_cast<int>(value.Height);

			AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), FALSE);

			MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, FALSE);
		}).detach();

		*success = true;

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE SetPreferredMinSize(
		/* [in] */ ABI::Windows::Foundation::Size minSize)
	{
		boolean success;
		return TryResizeView(minSize, &success);
		//return m_original3->SetPreferredMinSize(minSize);
	}
};

static Size g_preferredLaunchSize;
static ApplicationViewWindowingMode g_preferredLaunchMode;

class ApplicationViewStatics : public ActivationFactory<IApplicationViewStatics, IApplicationViewStatics2, IApplicationViewStatics3>
{
private:
	ComPtr<IApplicationViewStatics> m_statics;
	ComPtr<IApplicationViewStatics2> m_statics2;
	ComPtr<IApplicationViewStatics3> m_statics3;

public:
	ApplicationViewStatics(ComPtr<IActivationFactory> origPtr)
	{
		origPtr.As(&m_statics);
		origPtr.As(&m_statics2);
		origPtr.As(&m_statics3);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Value(
		/* [out][retval] */ __RPC__out ABI::Windows::UI::ViewManagement::ApplicationViewState *value) override
	{
		return m_statics->get_Value(value);
	}

	virtual HRESULT STDMETHODCALLTYPE TryUnsnap(
		/* [out][retval] */ __RPC__out boolean *success) override
	{
		return m_statics->TryUnsnap(success);
	}

	virtual HRESULT STDMETHODCALLTYPE GetForCurrentView(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::UI::ViewManagement::IApplicationView **current) override
	{
		ComPtr<IApplicationView> origView;
		HRESULT hr = m_statics2->GetForCurrentView(origView.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			auto view = Make<RApplicationView>(origView);
			hr = view.CopyTo(current);
		}

		return hr;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TerminateAppOnFinalViewClose(
		/* [out][retval] */ __RPC__out boolean *value) override
	{
		return m_statics2->get_TerminateAppOnFinalViewClose(value);
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_TerminateAppOnFinalViewClose(
		/* [in] */ boolean value) override
	{
		return m_statics2->put_TerminateAppOnFinalViewClose(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PreferredLaunchWindowingMode(
		/* [out][retval] */ __RPC__out ABI::Windows::UI::ViewManagement::ApplicationViewWindowingMode *value)
	{
		*value = g_preferredLaunchMode;

		return S_OK;
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PreferredLaunchWindowingMode(
		/* [in] */ ABI::Windows::UI::ViewManagement::ApplicationViewWindowingMode value)
	{
		g_preferredLaunchMode = value;

		return m_statics3->put_PreferredLaunchWindowingMode(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PreferredLaunchViewSize(
		/* [out][retval] */ __RPC__out ABI::Windows::Foundation::Size *value)
	{
		*value = g_preferredLaunchSize;

		return S_OK;
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PreferredLaunchViewSize(
		/* [in] */ ABI::Windows::Foundation::Size value)
	{
		g_preferredLaunchSize = value;

		return m_statics3->put_PreferredLaunchViewSize(value);
	}
};

template<typename T>
HRESULT WINAPI MakeFactory(HSTRING hString, IActivationFactory** outFactory)
{
	ComPtr<IActivationFactory> origPtr;
	ImHost_GetActivationFactory(hString, IID_IActivationFactory, (void**)origPtr.GetAddressOf());

	auto ptr = Make<T>(origPtr);
	return ptr.CopyTo(outFactory);
}

void InitializeWindowInterfaces()
{
	//ImHost_AddActivationFactory(RuntimeClass_Windows_UI_ViewManagement_ApplicationView, MakeFactory<ApplicationViewStatics>);

	ImHost_AddActivationCallback([] ()
	{
		std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();

		std::wstring id = identity->GetPrimaryAppId();

		std::thread([=] ()
		{
			HSTRING string;
			WindowsCreateString(id.c_str(), id.length(), &string);

			ComPtr<IActivatableApplication> factory;
			RoGetActivationFactory(string, GUID_IActivatableApplication, (void**)factory.ReleaseAndGetAddressOf());

			ComPtr<ICoreWindowFactory> windowFactory = Make<CoreWindowFactory>();
			ComPtr<IActivatedEventArgs> eventArgs = Make<ActivatedEventArgs>();

			factory->Activate(windowFactory.Get(), string, eventArgs.Get());
		}).detach();
	});
}