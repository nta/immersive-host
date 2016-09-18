#include "StdInc.h"
#include <interfaces/CallingProcessInfo.h>

HRESULT CallingProcessInfo::OpenCallerProcessHandle(DWORD id, HANDLE* handle)
{
	HANDLE activationProxyHandle = ImHost_GetActivationProxyProcessHandle();

	if (IsModule<TokenBrokerModule>(ReturnAddress) || IsModule<WebCoreModule>(ReturnAddress) || IsModule<StoreModule>(ReturnAddress))
	{
		HANDLE newHandle;
		DuplicateHandle(GetCurrentProcess(), activationProxyHandle, GetCurrentProcess(), &newHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

		*handle = (activationProxyHandle) ? newHandle : GetCurrentProcess();
	}
	else
	{
		*handle = GetCurrentProcess();
	}

	return S_OK;
}