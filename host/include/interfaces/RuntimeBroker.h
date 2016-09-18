#pragma once

#include <initguid.h>
DEFINE_GUID(GUID_IRuntimeBroker, 0x6040ec14, 0x6557, 0x41f9, 0xa3, 0xf7, 0xb1, 0xca, 0xb7, 0xb4, 0x21, 0x20);
DEFINE_GUID(CLSID_RuntimeBroker, 0xD63B10C5, 0xBB46, 0x4990, 0xA9, 0x4F, 0xE4, 0x0B, 0x9D, 0x52, 0x01, 0x60);

class __declspec(uuid("6040ec14-6557-41f9-a3f7-b1cab7b42120")) IRuntimeBroker : public IInspectable
{
public:
	STDMETHOD(ActivateInstance)(const wchar_t* instanceName, IInspectable** out) = 0;
	STDMETHOD(GetActivationFactory)(const wchar_t* instanceName, REFIID uuid, void** out) = 0;
	STDMETHOD(SetErrorFlags)(uint32_t) = 0;
	STDMETHOD(GetErrorFlags)(uint32_t*) = 0;
	STDMETHOD(DebuggerAddRef)() = 0;
	STDMETHOD(DebuggerRelease)() = 0;
	STDMETHOD(GetClipboardBroker)(IUnknown**) = 0;
};

ComPtr<IRuntimeBroker> MakeRuntimeBroker();