#include <windows.h>

#pragma comment(lib, "rpcrt4.lib")

void RunRpcServer();
void StopRpcServer();

static SERVICE_STATUS_HANDLE g_controlHandle;
static SERVICE_STATUS g_status;

void WINAPI ServiceMain(DWORD argc, LPWSTR* argv)
{
	g_controlHandle = RegisterServiceCtrlHandler(L"ImmersiveElevationService", [] (DWORD controlCode)
	{
		if (controlCode == SERVICE_CONTROL_STOP)
		{
			if (g_status.dwCurrentState != SERVICE_RUNNING)
			{
				return;
			}

			g_status.dwControlsAccepted = 0;
			g_status.dwCurrentState = SERVICE_STOP_PENDING;
			g_status.dwWin32ExitCode = 0;
			g_status.dwCheckPoint = 4;

			SetServiceStatus(g_controlHandle, &g_status);

			StopRpcServer();
		}
	});

	memset(&g_status, 0, sizeof(g_status));

	// run the service
	g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_status.dwCurrentState = SERVICE_RUNNING;
	g_status.dwCheckPoint = 0;

	SetServiceStatus(g_controlHandle, &g_status);

	RunRpcServer();

	// mark the service as stopped
	g_status.dwControlsAccepted = 0;
	g_status.dwCurrentState = SERVICE_STOPPED;
	g_status.dwWin32ExitCode = 0;
	g_status.dwCheckPoint = 3;

	SetServiceStatus(g_controlHandle, &g_status);
}

int main(int argc, char** argv)
{
	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ L"ImmersiveElevationService", (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ 0, 0 }
	};

	if (StartServiceCtrlDispatcher(serviceTable) == FALSE)
	{
		return GetLastError();
	}

	return 0;
}