#include "StdInc.h"
#include <Windows.Devices.Enumeration.h>
#include <StubInternal.h>

extern DWORD g_activationLock;

using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Foundation::Collections;

class DeviceInformationStatics : public ActivationFactory<IDeviceInformationStatics, IDeviceInformationStatics2>
{
private:
	ComPtr<IDeviceInformationStatics> m_realDevice;
	ComPtr<IDeviceInformationStatics2> m_realDevice2;

public:
	DeviceInformationStatics()
	{
		HString hString;
		hString.Set(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation);

		ImHost_GetActivationFactory(hString.Get(), IID_IDeviceInformationStatics, &m_realDevice);
		
		m_realDevice.As(&m_realDevice2);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateFromIdAsync(
		/* [in] */ __RPC__in HSTRING deviceId,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CDevices__CEnumeration__CDeviceInformation **asyncOp) override
	{
		return m_realDevice->CreateFromIdAsync(deviceId, asyncOp);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateFromIdAsyncAdditionalProperties(
		/* [in] */ __RPC__in HSTRING deviceId,
		/* [in] */ __RPC__in_opt __FIIterable_1_HSTRING *additionalProperties,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CDevices__CEnumeration__CDeviceInformation **asyncOp) override
	{
		return m_realDevice->CreateFromIdAsyncAdditionalProperties(deviceId, additionalProperties, asyncOp);
	}

	virtual HRESULT STDMETHODCALLTYPE FindAllAsync(
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CDevices__CEnumeration__CDeviceInformationCollection **asyncOp) override
	{
		return m_realDevice->FindAllAsync(asyncOp);
	}

	virtual HRESULT STDMETHODCALLTYPE FindAllAsyncDeviceClass(
		/* [in] */ ABI::Windows::Devices::Enumeration::DeviceClass deviceClass,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CDevices__CEnumeration__CDeviceInformationCollection **asyncOp) override
	{
		return m_realDevice->FindAllAsyncDeviceClass(deviceClass, asyncOp);
	}

	virtual HRESULT STDMETHODCALLTYPE FindAllAsyncAqsFilter(
		/* [in] */ __RPC__in HSTRING aqsFilter,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CDevices__CEnumeration__CDeviceInformationCollection **asyncOp) override
	{
		auto collectionStub = Make<EmptyVectorView<DeviceInformation*, IDeviceInformation*>>();

		ComPtr<IVectorView<DeviceInformation*>> ptr;
		collectionStub.As(&ptr);

		auto operationStub = Make<AsyncOperationStub<DeviceInformationCollection*, IVectorView<DeviceInformation*>*>>(ptr);
		return operationStub.CopyTo(asyncOp);
	}

	virtual HRESULT STDMETHODCALLTYPE FindAllAsyncAqsFilterAndAdditionalProperties(
		/* [in] */ __RPC__in HSTRING aqsFilter,
		/* [in] */ __RPC__in_opt __FIIterable_1_HSTRING *additionalProperties,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CDevices__CEnumeration__CDeviceInformationCollection **asyncOp) override
	{
		return m_realDevice->FindAllAsyncAqsFilterAndAdditionalProperties(aqsFilter, additionalProperties, asyncOp);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateWatcher(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Devices::Enumeration::IDeviceWatcher **watcher) override
	{
		return m_realDevice->CreateWatcher(watcher);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateWatcherDeviceClass(
		/* [in] */ ABI::Windows::Devices::Enumeration::DeviceClass deviceClass,
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Devices::Enumeration::IDeviceWatcher **watcher) override
	{
		return m_realDevice->CreateWatcherDeviceClass(deviceClass, watcher);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateWatcherAqsFilter(
		/* [in] */ __RPC__in HSTRING aqsFilter,
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Devices::Enumeration::IDeviceWatcher **watcher) override
	{
		return m_realDevice->CreateWatcherAqsFilter(aqsFilter, watcher);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateWatcherAqsFilterAndAdditionalProperties(
		/* [in] */ __RPC__in HSTRING aqsFilter,
		/* [in] */ __RPC__in_opt __FIIterable_1_HSTRING *additionalProperties,
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Devices::Enumeration::IDeviceWatcher **watcher) override
	{
		return m_realDevice->CreateWatcherAqsFilterAndAdditionalProperties(aqsFilter, additionalProperties, watcher);
	}

	virtual HRESULT STDMETHODCALLTYPE GetAqsFilterFromDeviceClass(
		/* [in] */ ABI::Windows::Devices::Enumeration::DeviceClass deviceClass,
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *aqsFilter) override
	{
		return m_realDevice2->GetAqsFilterFromDeviceClass(deviceClass, aqsFilter);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateFromIdAsyncWithKindAndAdditionalProperties(
		/* [in] */ __RPC__in HSTRING deviceId,
		/* [in] */ __RPC__in_opt __FIIterable_1_HSTRING *additionalProperties,
		/* [in] */ ABI::Windows::Devices::Enumeration::DeviceInformationKind kind,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CDevices__CEnumeration__CDeviceInformation **asyncOp) override
	{
		return m_realDevice2->CreateFromIdAsyncWithKindAndAdditionalProperties(deviceId, additionalProperties, kind, asyncOp);
	}

	virtual HRESULT STDMETHODCALLTYPE FindAllAsyncWithKindAqsFilterAndAdditionalProperties(
		/* [in] */ __RPC__in HSTRING aqsFilter,
		/* [in] */ __RPC__in_opt __FIIterable_1_HSTRING *additionalProperties,
		/* [in] */ ABI::Windows::Devices::Enumeration::DeviceInformationKind kind,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CDevices__CEnumeration__CDeviceInformationCollection **asyncOp) override
	{
		auto collectionStub = Make<EmptyVectorView<DeviceInformation*, IDeviceInformation*>>();

		ComPtr<IVectorView<DeviceInformation*>> ptr;
		collectionStub.As(&ptr);

		DWORD oldLock = g_activationLock;
		WaitOnAddress(&g_activationLock, &oldLock, sizeof(g_activationLock), INFINITE);

		auto operationStub = Make<AsyncOperationStub<DeviceInformationCollection*, IVectorView<DeviceInformation*>*>>(ptr);
		return operationStub.CopyTo(asyncOp);
		//return m_realDevice2->FindAllAsyncWithKindAqsFilterAndAdditionalProperties(aqsFilter, additionalProperties, kind, asyncOp);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateWatcherWithKindAqsFilterAndAdditionalProperties(
		/* [in] */ __RPC__in HSTRING aqsFilter,
		/* [in] */ __RPC__in_opt __FIIterable_1_HSTRING *additionalProperties,
		/* [in] */ ABI::Windows::Devices::Enumeration::DeviceInformationKind kind,
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Devices::Enumeration::IDeviceWatcher **watcher) override
	{
		return m_realDevice2->CreateWatcherWithKindAqsFilterAndAdditionalProperties(aqsFilter, additionalProperties, kind, watcher);
	}
};

void RegisterDeviceEnumeration()
{
	ImHost_AddActivationFactory(RuntimeClass_Windows_Devices_Enumeration_DeviceInformation, [](HSTRING, IActivationFactory** factory)
	{
		auto app = Make<DeviceInformationStatics>();
		return app.CopyTo(factory);
	});
}