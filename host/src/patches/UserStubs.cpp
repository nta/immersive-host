#include "StdInc.h"
#include "Hooking.Patterns.h"

#include <Windows.UI.Core.CoreWindowFactory.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::ViewManagement;

static HWND g_coreWindow;

static HWND(WINAPI* g_origCreateWindowInBandEx)(_In_ DWORD dwExStyle, _In_opt_ LPCWSTR lpClassName, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band, DWORD dwTypeFlags);

HWND WINAPI CreateWindowInBandExImpl(_In_ DWORD dwExStyle, _In_opt_ LPCWSTR lpClassName, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band, DWORD dwTypeFlags)
{
	dwStyle |= WS_VISIBLE;

	HString hStr;
	hStr.Set(RuntimeClass_Windows_UI_ViewManagement_ApplicationView);

	ComPtr<IApplicationViewStatics3> applicationView;
	ImHost_GetActivationFactory(hStr.Get(), __uuidof(IApplicationViewStatics3), (void**)applicationView.GetAddressOf());

	ApplicationViewWindowingMode windowingMode;
	applicationView->get_PreferredLaunchWindowingMode(&windowingMode);

	if (windowingMode != ApplicationViewWindowingMode_FullScreen)
	{
		//dwStyle |= WS_OVERLAPPEDWINDOW;
	}

	hWndParent = nullptr;// ImHost_GetCoreWindowHandle();

	//dwStyle |= WS_CHILD;
	//dwStyle |= WS_OVERLAPPEDWINDOW;
	//dwStyle &= ~WS_POPUP;
	dwStyle &= ~WS_OVERLAPPEDWINDOW;
	dwStyle = WS_POPUP;

	HMONITOR primaryMonitor = MonitorFromPoint(POINT{ 0, 0 }, 0);
	MONITORINFO mi = { sizeof(MONITORINFO) };
	GetMonitorInfo(primaryMonitor, &mi);

	X = mi.rcMonitor.left;
	Y = mi.rcMonitor.top;
	nWidth = mi.rcMonitor.right - mi.rcMonitor.left;
	nHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

	if (!lpWindowName)
	{
		lpWindowName = L"MEOW!";
	}

	std::wstring windowTitle = std::wstring(lpWindowName) + L" (ImmersiveHost)";

	HWND hWindow = CreateWindowExW(dwExStyle, lpClassName, windowTitle.c_str(), dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	//g_coreWindow = hWindow;

	return hWindow;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND ImHost_GetCoreWindowHandle()
{
	if (!g_coreWindow)
	{
		// create the actual game window - well, the class
		{
			// TODO: icon resource
			WNDCLASS windowClass = { 0 };
			windowClass.lpfnWndProc = WindowProcedure;
			windowClass.hInstance = GetModuleHandle(nullptr);
			windowClass.hCursor = LoadCursor(0, IDC_ARROW);
			windowClass.lpszClassName = L"Win32GameWindow";

			RegisterClass(&windowClass);
		}

		DWORD windowStyle = WS_VISIBLE;
		windowStyle |= WS_SYSMENU | WS_CAPTION;

		// adjust the rectangle to fit a possible client area
		RECT windowRect;
		windowRect.left = 10;
		windowRect.right = 1034;
		windowRect.top = 10;
		windowRect.bottom = 758;

		AdjustWindowRect(&windowRect, windowStyle, FALSE);

		// create the window
		g_coreWindow = CreateWindowEx(0, L"Win32GameWindow", L"meow",
										windowStyle, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
										nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
	}

	return g_coreWindow;
}

void WriteNetworkLog(int type, const char* logEntry, ...)
{
	FILE* log = fopen("B:\\dev\\i-h\\game.log", "a");

	if (log)
	{
		va_list ap;
		va_start(ap, logEntry);
		vfprintf(log, logEntry, ap);
		va_end(ap);

		fprintf(log, "\n");

		fclose(log);
	}
}

#include <PackageIdentity.h>
#include "../../build/elevation_rpc_h.h"

void DuplicateNetworkFirewallRule()
{
	if (ImHost_IsActivationProxy())
	{
		return;
	}

	RPC_WSTR stringBinding = nullptr;
	RpcStringBindingCompose(nullptr, reinterpret_cast<RPC_WSTR>(L"ncacn_np"), nullptr, reinterpret_cast<RPC_WSTR>(L"\\pipe\\ImmersiveElevationServer"), nullptr, &stringBinding);

	RPC_BINDING_HANDLE bindingHandle;
	RpcBindingFromStringBinding(stringBinding, &bindingHandle);

	update_firewall_rule(bindingHandle, GetCurrentProcessId(), GetCurrentPackageIdentity()->GetFamilyName().c_str());

	RpcStringFree(&stringBinding);
	RpcBindingFree(&bindingHandle);
}

int InitAfterDec()
{
	// temp fix for opus
	char* mB = (char*)GetModuleHandle(nullptr);

	auto pattern = hook::module_pattern(mB, "74 08 49 8B CF E8 ? ? ? ? 44 89");

	if (pattern.size() > 0)
	{
		char* location = pattern.get(0).get<char>(6);
		char* address = (location + *(int32_t*)location + 4);

		DWORD a;
		VirtualProtect(address, 1, PAGE_EXECUTE_READWRITE, &a);
		*address = 0xC3;
	}

	// opus #2: net log api
	pattern = hook::module_pattern(mB, "48 89 74 24 ? 48 8D 54 24 ? 4C 8B CB");

	for (int i = 0; i < pattern.size(); i++)
	{
		char* address = pattern.get(i).get<char>(-0x27);

		MH_CreateHook(address, WriteNetworkLog, nullptr);
	}

	MH_EnableHook(MH_ALL_HOOKS);

	return 0;
}

#include <thread>

void InitializeUserStubs()
{
	LoadLibrary(L"vcruntime140_app.dll");

	MH_CreateHookApi(L"user32.dll", "CreateWindowInBandEx", CreateWindowInBandExImpl, (void**)&g_origCreateWindowInBandEx);
	MH_CreateHookApi(L"vcruntime140_app.dll", "__telemetry_main_invoke_trigger", InitAfterDec, nullptr);
	MH_EnableHook(MH_ALL_HOOKS);

	std::thread([] ()
	{
		Sleep(10000);

		while (true)
		{
			Sleep(10000);

			DuplicateNetworkFirewallRule();
		}
	}).detach();
}