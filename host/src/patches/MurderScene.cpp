#include "StdInc.h"
#include <mutex>
#include <set>

void RaiseTrailBlazer()
{
	OutputDebugStringW(va(L"Repurposed Tpw thread %x for trail blazer purposes.\n", GetCurrentThreadId()));

	while (true)
	{
		Sleep(60000);
	}

	RaiseException(0x4000BADE, 0, 0, nullptr);
}

static std::mutex g_taskListMutex;
static std::set<DWORD> g_runningThreadpoolTasks;

struct TpwStubCtx
{
	PTP_WORK_CALLBACK function;
	PVOID context;

	void Run(PTP_CALLBACK_INSTANCE cb, PTP_WORK work)
	{
		DWORD threadId = GetCurrentThreadId();

		{
			std::lock_guard<std::mutex> lock(g_taskListMutex);
			g_runningThreadpoolTasks.insert(threadId);
		}

		function(cb, context, work);

		{
			std::lock_guard<std::mutex> lock(g_taskListMutex);
			g_runningThreadpoolTasks.erase(threadId);
		}
	}
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
		ctx.Run(cb, work);
	}
	// TODO: make unwinding actually work :-)
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

LONG WINAPI MurderSceneVEH(PEXCEPTION_POINTERS ptrs)
{
	if (ptrs->ExceptionRecord->ExceptionCode == 0xC0000005)
	{
		// if this is on a threadpool task...
		bool found = false;

		{
			std::lock_guard<std::mutex> lock(g_taskListMutex);

			found = (g_runningThreadpoolTasks.find(GetCurrentThreadId()) != g_runningThreadpoolTasks.end());
		}
		
		if (found)
		{
			// stub out this threadpool task
			//if (((uintptr_t)ptrs->ExceptionRecord->ExceptionAddress >> 36) == 0x7FF)
			{
				ptrs->ContextRecord->Rip = (DWORD64)RaiseTrailBlazer;
				ptrs->ContextRecord->Rsp &= 0xFFFFFFFFFFFFFFF0;
				ptrs->ContextRecord->Rsp |= 8;

				return EXCEPTION_CONTINUE_EXECUTION;
			}
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void InitializeMurderScene()
{
	MH_CreateHookApi(L"kernelbase.dll", "CreateThreadpoolWork", CreateThreadpoolWork_Custom, (void**)&g_origCreateThreadpoolWork);
	MH_EnableHook(MH_ALL_HOOKS);

	AddVectoredExceptionHandler(0, MurderSceneVEH);
}