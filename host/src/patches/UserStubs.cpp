#include "StdInc.h"

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
		dwStyle |= WS_OVERLAPPEDWINDOW;
	}

	hWndParent = nullptr;// ImHost_GetCoreWindowHandle();

	//dwStyle |= WS_CHILD;
	dwStyle |= WS_OVERLAPPEDWINDOW;
	dwStyle &= ~WS_POPUP;

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

const wchar_t* va(const wchar_t* string, ...);

void RaiseTrailBlazer()
{
	OutputDebugString(va(L"MURDER Tpw %08x\n", GetCurrentThreadId()));

	while (true)
	{
		Sleep(30000);
	}

	RaiseException(0x4000BADE, 0, 0, nullptr);
}

BOOL(WINAPI* g_origQueueUserWorkItem)(
	_In_     LPTHREAD_START_ROUTINE Function,
	_In_opt_ PVOID                  Context,
	_In_     ULONG                  Flags
	);

struct UwiStubCtx
{
	LPTHREAD_START_ROUTINE function;
	PVOID context;
};

#define BUFFER_COUNT 8
#define BUFFER_LENGTH 32768

const char* va(const char* string, ...)
{
	static thread_local int currentBuffer;
	static thread_local std::vector<char> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_COUNT * BUFFER_LENGTH);
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vsnprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}

const wchar_t* va(const wchar_t* string, ...)
{
	static thread_local int currentBuffer;
	static thread_local std::vector<wchar_t> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_COUNT * BUFFER_LENGTH);
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vswprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}

DWORD WINAPI UwiStub(LPVOID argument)
{
	UwiStubCtx ctx = *(UwiStubCtx*)argument;
	//delete (UwiStubCtx*)argument;

	__try
	{
		OutputDebugString(va(L"begin Uwi %08x\n", GetCurrentThreadId()));
		auto v = ctx.function(ctx.context);
		OutputDebugString(va(L"end Uwi %08x\n", GetCurrentThreadId()));
		return v;
	}
	__except ((GetExceptionCode() == 0x4000BADE) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return 0;
	}
}

BOOL WINAPI QueueUserWorkItem_Custom(
	_In_     LPTHREAD_START_ROUTINE Function,
	_In_opt_ PVOID                  Context,
	_In_     ULONG                  Flags
)
{
	return g_origQueueUserWorkItem(UwiStub, new UwiStubCtx{ Function, Context }, Flags);
}

struct TpwStubCtx
{
	PTP_WORK_CALLBACK function;
	PVOID context;
};

PTP_WORK(WINAPI* g_origCreateThreadpoolWork)(_In_        PTP_WORK_CALLBACK    pfnwk,
											 _Inout_opt_ PVOID                pv,
											 _In_opt_    PTP_CALLBACK_ENVIRON pcbe);

VOID WINAPI TpwStub(PTP_CALLBACK_INSTANCE cb, LPVOID argument, PTP_WORK work)
{
	TpwStubCtx ctx = *(TpwStubCtx*)argument;
	//delete (TpwStubCtx*)argument;

	__try
	{
		OutputDebugString(va(L"begin Tpw %08x\n", GetCurrentThreadId()));
		ctx.function(cb, ctx.context, work);
		OutputDebugString(va(L"end Tpw %08x\n", GetCurrentThreadId()));
	}
	__except ((GetExceptionCode() == 0x4000BADE) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		return;
	}
}

PTP_WORK WINAPI CreateThreadpoolWork_Custom(_In_        PTP_WORK_CALLBACK    pfnwk,
											_Inout_opt_ PVOID                pv,
											_In_opt_    PTP_CALLBACK_ENVIRON pcbe)
{
	return g_origCreateThreadpoolWork(TpwStub, new TpwStubCtx{ pfnwk, pv }, pcbe);
}

LONG WINAPI Vehsies(PEXCEPTION_POINTERS ptrs)
{
	if (ptrs->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		if (((uintptr_t)ptrs->ExceptionRecord->ExceptionAddress >> 32) == 0x7FF8)
		{
			//TerminateThread(GetCurrentThread(), 1);
			ptrs->ContextRecord->Rip = (DWORD64)RaiseTrailBlazer;
			ptrs->ContextRecord->Rsp &= 0xFFFFFFFFFFFFFFF0;
			ptrs->ContextRecord->Rsp |= 8;

			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void InitializeUserStubs()
{
	MH_CreateHookApi(L"user32.dll", "CreateWindowInBandEx", CreateWindowInBandExImpl, (void**)&g_origCreateWindowInBandEx);
	MH_CreateHookApi(L"kernelbase.dll", "QueueUserWorkItem", QueueUserWorkItem_Custom, (void**)&g_origQueueUserWorkItem);
	MH_CreateHookApi(L"kernelbase.dll", "CreateThreadpoolWork", CreateThreadpoolWork_Custom, (void**)&g_origCreateThreadpoolWork);
	MH_EnableHook(MH_ALL_HOOKS);

	AddVectoredExceptionHandler(0, Vehsies);
}