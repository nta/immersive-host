#pragma once

class __declspec(uuid("68C6A1B9-DE39-42C3-8D28-BF40A5126541")) ICallingProcessInfo : public IUnknown
{
public:
	virtual STDMETHOD(OpenCallerProcessHandle)(DWORD id, HANDLE* handle) = 0;
};

class CallingProcessInfo : public RuntimeClass<RuntimeClassFlags<ClassicCom>, ICallingProcessInfo>
{
public:
	STDMETHOD(OpenCallerProcessHandle)(DWORD id, HANDLE* handle) override;

	PVOID ReturnAddress;
};
