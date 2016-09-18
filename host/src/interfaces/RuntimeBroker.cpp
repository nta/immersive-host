#include "StdInc.h"
#include <interfaces/RuntimeBroker.h>

class RuntimeBroker : public RuntimeClass<IRuntimeBroker>
{
public:
	STDMETHOD(ActivateInstance)(const wchar_t* instanceName, IInspectable** out)
	{
		HString hString;
		hString.Set(instanceName);

		return RoActivateInstance(hString.Get(), out);
	}

	STDMETHOD(GetActivationFactory)(const wchar_t* instanceName, REFIID uuid, void** out)
	{
		HString hString;
		hString.Set(instanceName);

		return RoGetActivationFactory(hString.Get(), uuid, out);
	}

	STDMETHOD(SetErrorFlags)(uint32_t)
	{
		return S_OK;
	}

	STDMETHOD(GetErrorFlags)(uint32_t* a)
	{
		*a = 0;
		return S_OK;
	}

	STDMETHOD(DebuggerAddRef)()
	{
		return S_OK;
	}

	STDMETHOD(DebuggerRelease)()
	{
		return S_OK;
	}

	STDMETHOD(GetClipboardBroker)(IUnknown**)
	{
		return E_NOTIMPL;
	}
};

ComPtr<IRuntimeBroker> MakeRuntimeBroker()
{
	return Make<RuntimeBroker>();
}