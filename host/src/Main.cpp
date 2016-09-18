#include "StdInc.h"
#include "ExecutableLoader.h"
#include <PackageIdentity.h>

#include <roapi.h>
#include <roregistrationapi.h>

#pragma comment(lib, "runtimeobject.lib")

static std::wstring g_appDataPath = L"B:\\tools\\bigstate";

bool ActivationClient_Intercept(int argc, char** argv);
void PrepareActivation();

void RegisterStoreOverride();
void RegisterExtendedExecution();

void InitializeWindowInterfaces();
void InitializeBaseStubs();
void InitializeComStubs();
void InitializePackageStubs();
void InitializeKeyStubs();
void InitializeUserStubs();

void PatchTokenBroker();

const std::wstring& ImHost_GetAppDataPath()
{
	return g_appDataPath;
}

static std::wstring g_cli;

const std::wstring& ImHost_GetOriginalCommandLine()
{
	return g_cli;
}

int main(int argc, char** argv)
{
	g_cli = GetCommandLineW();

	if (argc == 2 && !_stricmp(argv[1], "-register"))
	{
		RoInitialize(RO_INIT_MULTITHREADED);
		PrepareActivation();

		return 0;
	}

	std::wstring packagePath = L"C:\\Program Files\\WindowsApps\\Microsoft.DeltaPC_1.6.0.0_x64__8wekyb3d8bbwe";

	if (argc >= 2)
	{
		wchar_t widePath[256] = { 0 };
		mbstowcs(widePath, argv[1], _countof(widePath) - 1);

		packagePath = widePath;
	}

	if (GetFileAttributes(L"overrides.ini") != INVALID_FILE_ATTRIBUTES)
	{
		wchar_t executableRoot[260];
		GetPrivateProfileString(L"AppX", L"ExecutableRoot", L"", executableRoot, _countof(executableRoot), L".\\overrides.ini");

		packagePath = executableRoot;

		GetPrivateProfileString(L"AppX", L"StateDir", L"", executableRoot, _countof(executableRoot), L".\\overrides.ini");

		g_appDataPath = executableRoot;
	}


	std::shared_ptr<PackageIdentity> identity = std::make_shared<PackageIdentity>(packagePath + L"\\AppXManifest.xml");
	SetCurrentPackageIdentity(identity);

	if (true)
	{
		RegisterStoreOverride();
	}

	RegisterExtendedExecution();

	LoadLibrary(L"tlsdll.dll");

	PatchTokenBroker();

	unlink("B:\\tools\\bigstate\\crash.detect");
	LoadLibrary(L"S:\\games\\steam\\gameoverlayrenderer64.dll");

	// initialize
	MH_Initialize();

	InitializeBaseStubs();
	InitializeComStubs();
	InitializePackageStubs();
	InitializeKeyStubs();
	InitializeWindowInterfaces();
	InitializeUserStubs();

	SetDllDirectory(identity->GetApplicationRoot().c_str());

	if (ActivationClient_Intercept(argc, argv))
	{
		return 0;
	}

	ExecutableLoader executableLoader(identity->GetApplicationRoot() + L"\\", identity->GetApplicationFileName());
	
	if (executableLoader.LoadExecutable())
	{
		MH_EnableHook(MH_ALL_HOOKS);

		if (!SetCurrentDirectory(identity->GetApplicationRoot().c_str()))
		{
			assert(!"Couldn't set current directory!");
			return 5;
		}

		executableLoader.Run();
	}

	return 0;
}