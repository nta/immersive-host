#include <StdInc.h>
#include <PackageIdentity.h>

#include <appmodel.h>
#include <AppxPackaging.h>

#include <MinHook.h>

static NTSTATUS(WINAPI* g_origQueryPackageIdentity)(HANDLE hToken, WCHAR* fullName, SIZE_T* fullNameSize, WCHAR* appId, SIZE_T* appIdSize, BOOLEAN* bUnk);

NTSTATUS WINAPI RtlQueryPackageIdentity_Custom(HANDLE hToken, WCHAR* fullName, SIZE_T* fullNameSize, WCHAR* appId, SIZE_T* appIdSize, BOOLEAN* bUnk)
{
	if (hToken != (HANDLE)-4)
	{
		return g_origQueryPackageIdentity(hToken, fullName, fullNameSize, appId, appIdSize, bUnk);
	}

	std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();
	NTSTATUS error = STATUS_SUCCESS;

	size_t targetFullNameSize = (identity->GetFullName().length() * 2) + 2;
	size_t targetAppIdSize = (identity->GetPrimaryAppId().length() * 2) + 2;

	if (fullNameSize)
	{
		if (*fullNameSize < targetFullNameSize)
		{
			error = STATUS_BUFFER_TOO_SMALL;
		}
		
		*fullNameSize = targetFullNameSize;
	}

	if (appIdSize)
	{
		if (*appIdSize < targetAppIdSize)
		{
			error = STATUS_BUFFER_TOO_SMALL;
		}

		*appIdSize = targetAppIdSize;
	}

	if (error == STATUS_SUCCESS)
	{
		if (appId)
		{
			wcscpy(appId, identity->GetPrimaryAppId().c_str());
		}

		if (fullName)
		{
			wcscpy(fullName, identity->GetFullName().c_str());
		}
	}

	return error;
}

DWORD WINAPI GetCurrentPackageIdImpl(UINT32* length, PACKAGE_ID* id)
{
	if (*length < sizeof(PACKAGE_ID))
	{
		*length = sizeof(PACKAGE_ID);
		return ERROR_INSUFFICIENT_BUFFER;
	}

	std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();

	memset(id, 0, sizeof(*id));
	id->name = const_cast<PWSTR>(identity->GetName().c_str()); // must match resources.pri
	id->processorArchitecture = APPX_PACKAGE_ARCHITECTURE_X64;
	id->publisher = const_cast<PWSTR>(identity->GetPublisher().c_str());
	id->publisherId = const_cast<PWSTR>(identity->GetPublisherId().c_str());
	id->resourceId = nullptr;
	id->version.Major = identity->GetVersionPart(0);
	id->version.Minor = identity->GetVersionPart(1);
	id->version.Build = identity->GetVersionPart(2);
	id->version.Revision = identity->GetVersionPart(3);

	*length = sizeof(PACKAGE_ID);

	return ERROR_SUCCESS;
}

LONG WINAPI GetCurrentPackageFullNameImpl(UINT32 *packageFullNameLength, PWSTR packageFullName)
{
	std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();
	NTSTATUS error = STATUS_SUCCESS;

	size_t targetFullNameSize = identity->GetFullName().length();
	size_t passedSize = *packageFullNameLength;

	*packageFullNameLength = targetFullNameSize;

	if (packageFullName && passedSize >= targetFullNameSize)
	{
		wcscpy(packageFullName, identity->GetFullName().c_str());

		return ERROR_SUCCESS;
	}

	return ERROR_INSUFFICIENT_BUFFER;
}

static LONG(WINAPI* g_origGetPackageFullName)(HANDLE hProcess, UINT32 *packageFullNameLength, PWSTR packageFullName);

LONG WINAPI GetPackageFullName_Custom(HANDLE hProcess, UINT32 *packageFullNameLength, PWSTR packageFullName)
{
	// might want to check for the current process?
	return GetCurrentPackageFullNameImpl(packageFullNameLength, packageFullName);
}

LONG WINAPI GetCurrentPackageInfoImpl(const UINT32 flags, UINT32 * bufferLength, BYTE * buffer, UINT32 * count)
{
	if (*bufferLength < sizeof(PACKAGE_INFO))
	{
		if (count)
		{
			*count = 1; // Windows expects count to be >0
		}

		*bufferLength = sizeof(PACKAGE_INFO);
		return ERROR_INSUFFICIENT_BUFFER;
	}

	*bufferLength = sizeof(PACKAGE_INFO);
	*count = 1;

	std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();

	PACKAGE_INFO info;
	ZeroMemory(&info, sizeof(info));
	info.flags = flags;
	info.reserved = 0;
	info.path = const_cast<PWSTR>(identity->GetApplicationRoot().c_str());
	info.packageFamilyName = const_cast<PWSTR>(identity->GetFamilyName().c_str());
	info.packageFullName = const_cast<PWSTR>(identity->GetFullName().c_str());

	UINT32 size = sizeof(info.packageId);
	GetCurrentPackageIdImpl(&size, &info.packageId);

	memcpy(buffer, &info, sizeof(PACKAGE_INFO));

	return ERROR_SUCCESS;
}

LONG WINAPI GetCurrentPackageFamilyNameImpl(UINT32 *packageFamilyNameLength, PWSTR packageFamilyName)
{
	std::shared_ptr<PackageIdentity> identity = GetCurrentPackageIdentity();

	if (*packageFamilyNameLength < identity->GetFamilyName().length())
	{
		*packageFamilyNameLength = identity->GetFamilyName().length();
		return ERROR_INSUFFICIENT_BUFFER;
	}

	wcscpy(packageFamilyName, identity->GetFamilyName().c_str());
	*packageFamilyNameLength = identity->GetFamilyName().length();

	return ERROR_SUCCESS;
}

LONG WINAPI GetPackageFamilyName_Custom(HANDLE hProcess, UINT32* length, PWSTR name)
{
	// might again need the current process?
	return GetCurrentPackageFamilyNameImpl(length, name);
}

static LONG(WINAPI* GetPackageResourcesContext)(PACKAGE_INFO_REFERENCE, DWORD, DWORD, void**);

LONG WINAPI GetCurrentPackageResourcesContext(DWORD a1, DWORD a2, void** context)
{
	static PACKAGE_INFO_REFERENCE reference;

	if (!reference)
	{
		OpenPackageInfoByFullName(GetCurrentPackageIdentity()->GetFullName().c_str(), 0, &reference);
	}

	return GetPackageResourcesContext(reference, a1, a2, context);
}

void InitializePackageStubs()
{
	MH_CreateHookApi(L"ntdll.dll", "RtlQueryPackageIdentity", RtlQueryPackageIdentity_Custom, (void**)&g_origQueryPackageIdentity);
	MH_CreateHookApi(L"kernelbase.dll", "GetCurrentPackageId", GetCurrentPackageIdImpl, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "GetCurrentPackageFullName", GetCurrentPackageFullNameImpl, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "GetCurrentPackageFamilyName", GetCurrentPackageFamilyNameImpl, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "GetCurrentPackageInfo", GetCurrentPackageInfoImpl, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "GetPackageFamilyName", GetPackageFamilyName_Custom, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "GetCurrentPackageResourcesContext", GetCurrentPackageResourcesContext, nullptr);
	MH_CreateHookApi(L"kernelbase.dll", "GetPackageFullName", GetPackageFullName_Custom, (void**)&g_origGetPackageFullName);
	MH_EnableHook(MH_ALL_HOOKS);

	GetPackageResourcesContext = (decltype(GetPackageResourcesContext))GetProcAddress(GetModuleHandle(L"kernelbase.dll"), "GetPackageResourcesContext");
}