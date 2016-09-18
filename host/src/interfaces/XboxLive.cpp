#include "StdInc.h"

class IXboxLiveDeviceAddress;
class IXboxLiveEndpointPairTemplate;
class IXboxLiveInboundEndpointPairCreatedEventArgs;

namespace ABI
{
	namespace Windows
	{
		namespace Foundation
		{
			namespace Collections
			{
				template <>
				struct __declspec(uuid("8AACB2DA-C37E-5BE0-AB46-BA445910B9CB"))
					IVector<IXboxLiveDeviceAddress*> : IVector_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveDeviceAddress*, IXboxLiveDeviceAddress*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IVector`1<IXboxLiveDeviceAddress>";
					}
				};
			}

			template <>
			struct __declspec(uuid("7FA76199-D1B8-5494-A042-7002A416ADC3"))
				ITypedEventHandler<IXboxLiveDeviceAddress*, IInspectable*> : ITypedEventHandler_impl<IXboxLiveDeviceAddress*, IInspectable*>
			{
				static const wchar_t* z_get_rc_name_impl()
				{
					return L"a";
				}
			};

			template <>
			struct __declspec(uuid("2AEF5136-6E7A-51F8-8853-CC0CE466FEF9"))
				ITypedEventHandler<IXboxLiveEndpointPairTemplate*, IXboxLiveInboundEndpointPairCreatedEventArgs*> : ITypedEventHandler_impl<IXboxLiveEndpointPairTemplate*, IXboxLiveInboundEndpointPairCreatedEventArgs*>
			{
				static const wchar_t* z_get_rc_name_impl()
				{
					return L"a";
				}
			};
		}
	}
}

class __declspec(uuid("f5bbd279-3c86-4b57-a31a-b9462408fd01")) IXboxLiveDeviceAddress : public IInspectable
{
public:
	STDMETHOD(add_SnapshotChanged)(ABI::Windows::Foundation::ITypedEventHandler<IXboxLiveDeviceAddress*, IInspectable*>*, EventRegistrationToken*) = 0;
	STDMETHOD(remove_SnapshotChanged)(EventRegistrationToken) = 0;
	STDMETHOD(GetSnapshotAsBase64)(HSTRING*) = 0;
	STDMETHOD(GetSnapshotAsBuffer)(ABI::Windows::Storage::Streams::IBuffer**) = 0;
	STDMETHOD(GetSnapshotAsBytes)(uint32_t, uint8_t*, uint32_t*) = 0;
	STDMETHOD(Compare)(IXboxLiveDeviceAddress*, int*) = 0;
	STDMETHOD(get_IsValid)(boolean*) = 0;
	STDMETHOD(get_IsLocal)(boolean*) = 0;
	STDMETHOD(get_NetworkAccessKind)(int*) = 0;
};

class __declspec(uuid("43431FCB-4D86-4E50-976C-A766526E7766")) IHasOriginalPtr : public IInspectable
{
public:
	STDMETHOD(GetOriginalPointer)(REFIID iid, void** ptr) = 0;
};

using namespace ABI::Windows::Foundation;

// ??_7XboxLiveDeviceAddressInterface@XboxLive@Networking@Windows@@6B@
class XboxLiveDeviceAddress : public RuntimeClass<IXboxLiveDeviceAddress, IHasOriginalPtr>
{
private:
	ComPtr<IXboxLiveDeviceAddress> m_original;

	EventSource<ITypedEventHandler<IXboxLiveDeviceAddress*, IInspectable*>> m_snapshotChangedEvent;

public:
	HRESULT RuntimeClassInitialize(ComPtr<IXboxLiveDeviceAddress> original)
	{
		m_original = original;

		auto cb = Callback<ITypedEventHandler<IXboxLiveDeviceAddress*, IInspectable*>>([=] (IXboxLiveDeviceAddress* sender, IInspectable* args)
		{
			return m_snapshotChangedEvent.InvokeAll(this, args);
		});

		EventRegistrationToken token;
		m_original->add_SnapshotChanged(cb.Get(), &token);

		return S_OK;
	}

	STDMETHOD(GetOriginalPointer)(REFIID iid, void** ptr) override
	{
		return m_original.CopyTo(iid, ptr);
	}

	STDMETHOD(add_SnapshotChanged)(ABI::Windows::Foundation::ITypedEventHandler<IXboxLiveDeviceAddress*, IInspectable*>* a1, EventRegistrationToken* a2) override
	{
		return m_snapshotChangedEvent.Add(a1, a2);
	}

	STDMETHOD(remove_SnapshotChanged)(EventRegistrationToken a1) override
	{
		return m_snapshotChangedEvent.Remove(a1);
	}

	STDMETHOD(GetSnapshotAsBase64)(HSTRING* a1) override
	{
		return m_original->GetSnapshotAsBase64(a1);
	}

	STDMETHOD(GetSnapshotAsBuffer)(ABI::Windows::Storage::Streams::IBuffer** a1) override
	{
		return m_original->GetSnapshotAsBuffer(a1);
	}

	STDMETHOD(GetSnapshotAsBytes)(uint32_t a1, uint8_t* a2, uint32_t* a3) override
	{
		return m_original->GetSnapshotAsBytes(a1, a2, a3);
	}

	STDMETHOD(Compare)(IXboxLiveDeviceAddress* a1, int* a2) override
	{
		ComPtr<IXboxLiveDeviceAddress> device = a1;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(device.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(IXboxLiveDeviceAddress), (void**)device.ReleaseAndGetAddressOf());
		}

		return m_original->Compare(device.Get(), a2);
	}
	STDMETHOD(get_IsValid)(boolean* a1) override
	{
		return m_original->get_IsValid(a1);
	}

	STDMETHOD(get_IsLocal)(boolean* a1) override
	{
		return m_original->get_IsLocal(a1);
	}

	STDMETHOD(get_NetworkAccessKind)(int* a1) override
	{
		return m_original->get_NetworkAccessKind(a1);
	}
};

class IXboxLiveEndpointPair;

class __declspec(uuid("d9a8bb95-2aab-4d1e-9794-33ecc0dcf0fe")) IXboxLiveEndpointPairCreationResult : public IInspectable
{
public:
	STDMETHOD(get_DeviceAddress)(IXboxLiveDeviceAddress** a1) = 0;
	STDMETHOD(get_Status)(int* a1) = 0;
	STDMETHOD(get_IsExistingPathEvaluation)(boolean* a1) = 0;
	STDMETHOD(get_EndpointPair)(IXboxLiveEndpointPair** a1) = 0;
};

class __declspec(uuid("dc183b62-22ba-48d2-80de-c23968bd198b")) IXboxLiveInboundEndpointPairCreatedEventArgs : public IInspectable
{
public:
	STDMETHOD(get_EndpointPair)(IXboxLiveEndpointPair** a1) = 0;
};

class __declspec(uuid("6b286ecf-3457-40ce-b9a1-c0cfe0213ea7")) IXboxLiveEndpointPairTemplate : public IInspectable
{
public:
	STDMETHOD(add_InboundEndpointPairCreated)(ABI::Windows::Foundation::ITypedEventHandler<IXboxLiveEndpointPairTemplate*, IXboxLiveInboundEndpointPairCreatedEventArgs*>* a1, EventRegistrationToken* a2) = 0;
	STDMETHOD(remove_InboundEndpointPairCreated)(EventRegistrationToken a1) = 0;
	STDMETHOD(CreateEndpointPairDefaultAsync)(IXboxLiveDeviceAddress* a1, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** a2) = 0;
	STDMETHOD(CreateEndpointPairWithBehaviorsAsync)(IXboxLiveDeviceAddress* a1, int a2, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** a3) = 0;
	STDMETHOD(CreateEndpointPairForPortsDefaultAsync)(IXboxLiveDeviceAddress* a1, HSTRING a2, HSTRING a3, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** a4) = 0;
	STDMETHOD(CreateEndpointPairForPortsWithBehaviorsAsync)(IXboxLiveDeviceAddress* a1, HSTRING a2, HSTRING a3, int a4, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** a5) = 0;
	STDMETHOD(get_Name)(HSTRING* a1) = 0;
	STDMETHOD(get_SocketKind)(int* a1) = 0;
	STDMETHOD(get_InitiatorBoundPortRangeLower)(uint16_t*) = 0;
	STDMETHOD(get_InitiatorBoundPortRangeUpper)(uint16_t*) = 0;
	STDMETHOD(get_AcceptorBoundPortRangeLower)(uint16_t*) = 0;
	STDMETHOD(get_AcceptorBoundPortRangeUpper)(uint16_t*) = 0;
	STDMETHOD(get_EndpointPairs)(ABI::Windows::Foundation::Collections::IVectorView<IXboxLiveEndpointPair*>** a1) = 0;
};

namespace ABI
{
	namespace Windows
	{
		namespace Foundation
		{
			namespace Collections
			{
				template <>
				struct __declspec(uuid("1128b50b-3ac4-543e-9d7e-d796c6226669"))
					IVectorView<IXboxLiveEndpointPairTemplate*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveEndpointPairTemplate*, IXboxLiveEndpointPairTemplate*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IVectorView`1<IXboxLiveEndpointPairTemplate>";
					}
				};

				template <>
				struct __declspec(uuid("f9a0d8f2-27c6-5e3f-b023-fe4dc1ca38ce"))
					IVector<IXboxLiveEndpointPairTemplate*> : IVector_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveEndpointPairTemplate*, IXboxLiveEndpointPairTemplate*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IVector`1<IXboxLiveEndpointPairTemplate>";
					}
				};

				template <>
				struct __declspec(uuid("515331c6-39e1-5cc7-8454-7f1c8bcba18e"))
					IIterator<IXboxLiveEndpointPairTemplate*> : IIterator_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveEndpointPairTemplate*, IXboxLiveEndpointPairTemplate*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IIterator`1<IXboxLiveEndpointPairTemplate>";
					}
				};

				template <>
				struct __declspec(uuid("9fce55c3-5580-581a-85b5-88dd5e9d4ff7"))
					IIterable<IXboxLiveEndpointPairTemplate*> : IIterable_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveEndpointPairTemplate*, IXboxLiveEndpointPairTemplate*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IIterable`1<IXboxLiveEndpointPairTemplate>";
					}
				};

				template <>
				struct __declspec(uuid("c88caa17-ae4a-5e4c-8435-d6db6b120464"))
					IVectorView<IXboxLiveEndpointPair*> : IVectorView_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveEndpointPair*, IXboxLiveEndpointPair*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IVectorView`1<IXboxLiveEndpointPairTemplate>";
					}
				};

				template <>
				struct __declspec(uuid("1ad14b6d-cac5-5ae5-8c56-247318682067"))
					IVector<IXboxLiveEndpointPair*> : IVector_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveEndpointPair*, IXboxLiveEndpointPair*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IVector`1<IXboxLiveEndpointPairTemplate>";
					}
				};

				template <>
				struct __declspec(uuid("e796a96b-98a0-52a2-bb54-9dba5bb64fc4"))
					IIterator<IXboxLiveEndpointPair*> : IIterator_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveEndpointPair*, IXboxLiveEndpointPair*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IIterator`1<IXboxLiveEndpointPairTemplate>";
					}
				};

				template <>
				struct __declspec(uuid("7fcaf666-ddc0-50dc-a76f-9bb6058be3c9"))
					IIterable<IXboxLiveEndpointPair*> : IIterable_impl<ABI::Windows::Foundation::Internal::AggregateType<IXboxLiveEndpointPair*, IXboxLiveEndpointPair*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IIterable`1<IXboxLiveEndpointPairTemplate>";
					}
				};
				/* ABI */
			} /* Windows */

			template <>
			struct __declspec(uuid("a4b9d41e-4b2c-5bf9-a19d-ed395b23823c"))
				IAsyncOperation<IXboxLiveEndpointPairCreationResult*> : IAsyncOperation_impl<IXboxLiveEndpointPairCreationResult*>
			{
				static const wchar_t* z_get_rc_name_impl()
				{
					return L"a";
				}
			};

			template <>
			struct __declspec(uuid("AD57FA1D-DC6B-58D1-9F70-60CC47BF1CED"))
				IAsyncOperationCompletedHandler<IXboxLiveEndpointPairCreationResult*> : IAsyncOperationCompletedHandler_impl<IXboxLiveEndpointPairCreationResult*>
			{
				static const wchar_t* z_get_rc_name_impl()
				{
					return L"a";
				}
			};
		} /* Foundation */
	} /* Collections */
}


// ??_7XboxLiveEndpointPairInterface@XboxLive@Networking@Windows@@6B@
class __declspec(uuid("1e9a839b-813e-44e0-b87f-c87a093475e4")) IXboxLiveEndpointPair : public IInspectable
{
public:
	STDMETHOD(add_StateChanged)(ABI::Windows::Foundation::ITypedEventHandler<void*, void*>* a1, EventRegistrationToken* a2) = 0;
	STDMETHOD(remove_StateChanged)(EventRegistrationToken a1) = 0;
	STDMETHOD(DeleteAsync)(ABI::Windows::Foundation::IAsyncAction* a1) = 0;
	STDMETHOD(GetRemoteSocketAddressBytes)(uint32_t a1, BYTE* a2) = 0;
	STDMETHOD(GetLocalSocketAddressBytes)(uint32_t a1, BYTE* a2) = 0;
	STDMETHOD(get_State)(int* a1) = 0;
	STDMETHOD(get_Template)(IXboxLiveEndpointPairTemplate** a1) = 0;
	STDMETHOD(get_RemoteDeviceAddress)(IXboxLiveDeviceAddress** a1) = 0;
	STDMETHOD(get_RemoteHostName)(ABI::Windows::Networking::IHostName** a1) = 0;
	STDMETHOD(get_RemotePort)(HSTRING* a1) = 0;
	STDMETHOD(get_LocalHostName)(ABI::Windows::Networking::IHostName** a1) = 0;
	STDMETHOD(get_LocalPort)(HSTRING* a1) = 0;
};

class __declspec(uuid("1e13137b-737b-4a23-bc64-0870f75655ba")) IXboxLiveEndpointPairTemplateStatics : public IInspectable
{
public:
	STDMETHOD(GetTemplateByName)(HSTRING a1, IXboxLiveEndpointPairTemplate** a2) = 0;
	STDMETHOD(get_Templates)(ABI::Windows::Foundation::Collections::IVectorView<IXboxLiveEndpointPairTemplate*>** a1) = 0;
};

class __declspec(uuid("64316b30-217a-4243-8ee1-6729281d27db")) IXboxLiveEndpointPairStatics : public IInspectable
{
public:
	STDMETHOD(FindEndpointPairBySocketAddressBytes)(uint32_t a1, const BYTE* a2, uint32_t a3, const BYTE* a4, IXboxLiveEndpointPair** a5) = 0;
	STDMETHOD(FindEndpointPairByHostNamesAndPorts)(ABI::Windows::Networking::IHostName* a1, HSTRING a2, ABI::Windows::Networking::IHostName* a3, HSTRING a4, IXboxLiveEndpointPair** a5) = 0;
};

class __declspec(uuid("5954a819-4a79-4931-827c-7f503e963263")) IXboxLiveDeviceAddressStatics : public IInspectable
{
public:
	STDMETHOD(CreateFromSnapshotBase64)(HSTRING, IXboxLiveDeviceAddress**) = 0;
	STDMETHOD(CreateFromSnapshotBuffer)(ABI::Windows::Storage::Streams::IBuffer*, IXboxLiveDeviceAddress**) = 0;
	STDMETHOD(CreateFromSnapshotBytes)(uint32_t, const BYTE*, IXboxLiveDeviceAddress**) = 0;
	STDMETHOD(GetLocal)(IXboxLiveDeviceAddress** address) = 0;
	STDMETHOD(get_MaxSnapshotBytesSize)(uint32_t*) = 0;
};

class XboxLiveEndpointPair : public RuntimeClass<IXboxLiveEndpointPair>
{
private:
	ComPtr<IXboxLiveEndpointPair> m_originalPtr;

public:
	HRESULT RuntimeClassInitialize(ComPtr<IXboxLiveEndpointPair> origPair)
	{
		origPair.As(&m_originalPtr);

		return S_OK;
	}

	STDMETHOD(add_StateChanged)(ABI::Windows::Foundation::ITypedEventHandler<void*, void*>* a1, EventRegistrationToken* a2) override
	{
		return m_originalPtr->add_StateChanged(a1, a2);
	}

	STDMETHOD(remove_StateChanged)(EventRegistrationToken a1) override
	{
		return m_originalPtr->remove_StateChanged(a1);
	}

	STDMETHOD(DeleteAsync)(ABI::Windows::Foundation::IAsyncAction* a1) override
	{
		return m_originalPtr->DeleteAsync(a1);
	}

	STDMETHOD(GetRemoteSocketAddressBytes)(uint32_t a1, BYTE* a2) override
	{
		return m_originalPtr->GetRemoteSocketAddressBytes(a1, a2);
	}

	STDMETHOD(GetLocalSocketAddressBytes)(uint32_t a1, BYTE* a2) override
	{
		return m_originalPtr->GetLocalSocketAddressBytes(a1, a2);
	}

	STDMETHOD(get_State)(int* a1) override
	{
		return m_originalPtr->get_State(a1);
	}

	STDMETHOD(get_Template)(IXboxLiveEndpointPairTemplate** a1) override;

	STDMETHOD(get_RemoteDeviceAddress)(IXboxLiveDeviceAddress** a1) override;

	STDMETHOD(get_RemoteHostName)(ABI::Windows::Networking::IHostName** a1) override
	{
		return m_originalPtr->get_RemoteHostName(a1);
	}

	STDMETHOD(get_RemotePort)(HSTRING* a1) override
	{
		return m_originalPtr->get_RemotePort(a1);
	}

	STDMETHOD(get_LocalHostName)(ABI::Windows::Networking::IHostName** a1) override
	{
		return m_originalPtr->get_LocalHostName(a1);
	}

	STDMETHOD(get_LocalPort)(HSTRING* a1) override
	{
		return m_originalPtr->get_LocalPort(a1);
	}
};

class XboxLiveEndpointPairCreationResult : public RuntimeClass<IXboxLiveEndpointPairCreationResult>
{
private:
	ComPtr<IXboxLiveEndpointPairCreationResult> m_originalPtr;

public:
	HRESULT RuntimeClassInitialize(ComPtr<IXboxLiveEndpointPairCreationResult> origResult)
	{
		origResult.As(&m_originalPtr);

		return S_OK;
	}

	STDMETHOD(get_DeviceAddress)(IXboxLiveDeviceAddress** a1) override;

	STDMETHOD(get_Status)(int* a1) override
	{
		return m_originalPtr->get_Status(a1);
	}

	STDMETHOD(get_IsExistingPathEvaluation)(boolean* a1) override
	{
		return m_originalPtr->get_IsExistingPathEvaluation(a1);
	}

	STDMETHOD(get_EndpointPair)(IXboxLiveEndpointPair** a1) override;
};

using namespace ABI::Windows::Foundation;

class XboxLiveInboundEndpointPairCreatedEventArgs : public RuntimeClass<IXboxLiveInboundEndpointPairCreatedEventArgs>
{
private:
	ComPtr<IXboxLiveInboundEndpointPairCreatedEventArgs> m_originalPtr;

public:
	HRESULT RuntimeClassInitialize(ComPtr<IXboxLiveInboundEndpointPairCreatedEventArgs> ptr)
	{
		m_originalPtr = ptr;

		return S_OK;
	}

	STDMETHOD(get_EndpointPair)(IXboxLiveEndpointPair** pair) override
	{
		ComPtr<IXboxLiveEndpointPair> ptr;
		HRESULT hr = m_originalPtr->get_EndpointPair(ptr.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<XboxLiveEndpointPair> result;
			MakeAndInitialize<XboxLiveEndpointPair>(&result, ptr);

			hr = result.CopyTo(pair);
		}

		return hr;
	}
};

class XboxLiveEndpointPairTemplate : public RuntimeClass<IXboxLiveEndpointPairTemplate>
{
private:
	ComPtr<IXboxLiveEndpointPairTemplate> m_originalPtr;

	EventSource<ITypedEventHandler<IXboxLiveEndpointPairTemplate*, IXboxLiveInboundEndpointPairCreatedEventArgs*>> m_epCreated;

private:
	HRESULT DoCreateEndpointPair(const std::function<HRESULT(IAsyncOperation<IXboxLiveEndpointPairCreationResult*>**, IXboxLiveDeviceAddress*)>& fn, IXboxLiveDeviceAddress* address, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** result)
	{
		ComPtr<IXboxLiveDeviceAddress> device = address;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(device.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(IXboxLiveDeviceAddress), (void**)device.ReleaseAndGetAddressOf());
		}

		ComPtr<ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>> origOperation;
		HRESULT hr = fn(origOperation.GetAddressOf(), device.Get());

		if (SUCCEEDED(hr))
		{
			auto operation = Make<AsyncOperation<IXboxLiveEndpointPairCreationResult>>(origOperation, [=] ()
			{
				ComPtr<IXboxLiveEndpointPairCreationResult> origResults;
				origOperation->GetResults(origResults.GetAddressOf());

				ComPtr<XboxLiveEndpointPairCreationResult> results;
				MakeAndInitialize<XboxLiveEndpointPairCreationResult>(&results, origResults);

				return results;
			});

			hr = operation.CopyTo(result);
		}

		return hr;
	}

public:
	HRESULT RuntimeClassInitialize(ComPtr<IUnknown> ptr)
	{
		ptr.As(&m_originalPtr);

		auto cb = Callback<ITypedEventHandler<IXboxLiveEndpointPairTemplate*, IXboxLiveInboundEndpointPairCreatedEventArgs*>>([=] (IXboxLiveEndpointPairTemplate* sender, IXboxLiveInboundEndpointPairCreatedEventArgs* args)
		{
			ComPtr<IXboxLiveInboundEndpointPairCreatedEventArgs> argsPtr(args);

			ComPtr<XboxLiveInboundEndpointPairCreatedEventArgs> ev;
			MakeAndInitialize<XboxLiveInboundEndpointPairCreatedEventArgs>(&ev, argsPtr);

			return m_epCreated.InvokeAll(this, ev.Get());
		});

		EventRegistrationToken token;
		m_originalPtr->add_InboundEndpointPairCreated(cb.Get(), &token);

		return S_OK;
	}

	STDMETHOD(add_InboundEndpointPairCreated)(ABI::Windows::Foundation::ITypedEventHandler<IXboxLiveEndpointPairTemplate*, IXboxLiveInboundEndpointPairCreatedEventArgs*>* a1, EventRegistrationToken* a2) override
	{
		return m_epCreated.Add(a1, a2);
	}

	STDMETHOD(remove_InboundEndpointPairCreated)(EventRegistrationToken a1) override
	{
		return m_epCreated.Remove(a1);
	}

	STDMETHOD(CreateEndpointPairDefaultAsync)(IXboxLiveDeviceAddress* a1, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** a2) override
	{
		return DoCreateEndpointPair(std::bind(&IXboxLiveEndpointPairTemplate::CreateEndpointPairDefaultAsync, m_originalPtr.Get(), std::placeholders::_2, std::placeholders::_1), a1, a2);
	}

	STDMETHOD(CreateEndpointPairWithBehaviorsAsync)(IXboxLiveDeviceAddress* a1, int a2, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** a3) override
	{
		return DoCreateEndpointPair(std::bind(&IXboxLiveEndpointPairTemplate::CreateEndpointPairWithBehaviorsAsync, m_originalPtr.Get(), std::placeholders::_2, a2, std::placeholders::_1), a1, a3);
	}

	STDMETHOD(CreateEndpointPairForPortsDefaultAsync)(IXboxLiveDeviceAddress* a1, HSTRING a2, HSTRING a3, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** a4) override
	{
		return DoCreateEndpointPair(std::bind(&IXboxLiveEndpointPairTemplate::CreateEndpointPairForPortsDefaultAsync, m_originalPtr.Get(), std::placeholders::_2, a2, a3, std::placeholders::_1), a1, a4);
	}

	STDMETHOD(CreateEndpointPairForPortsWithBehaviorsAsync)(IXboxLiveDeviceAddress* a1, HSTRING a2, HSTRING a3, int a4, ABI::Windows::Foundation::IAsyncOperation<IXboxLiveEndpointPairCreationResult*>** a5) override
	{
		return DoCreateEndpointPair(std::bind(&IXboxLiveEndpointPairTemplate::CreateEndpointPairForPortsWithBehaviorsAsync, m_originalPtr.Get(), std::placeholders::_2, a2, a3, a4, std::placeholders::_1), a1, a5);
	}

	STDMETHOD(get_Name)(HSTRING* a1) override
	{
		return m_originalPtr->get_Name(a1);
	}

	STDMETHOD(get_SocketKind)(int* a1) override
	{
		return m_originalPtr->get_SocketKind(a1);
	}

	STDMETHOD(get_InitiatorBoundPortRangeLower)(uint16_t* a1) override
	{
		return m_originalPtr->get_InitiatorBoundPortRangeLower(a1);
	}

	STDMETHOD(get_InitiatorBoundPortRangeUpper)(uint16_t* a1) override
	{
		return m_originalPtr->get_InitiatorBoundPortRangeUpper(a1);
	}

	STDMETHOD(get_AcceptorBoundPortRangeLower)(uint16_t* a1) override
	{
		return m_originalPtr->get_AcceptorBoundPortRangeLower(a1);
	}

	STDMETHOD(get_AcceptorBoundPortRangeUpper)(uint16_t* a1) override
	{
		return m_originalPtr->get_AcceptorBoundPortRangeUpper(a1);
	}

	STDMETHOD(get_EndpointPairs)(ABI::Windows::Foundation::Collections::IVectorView<IXboxLiveEndpointPair*>** a1) override
	{
		ComPtr<ABI::Windows::Foundation::Collections::IVectorView<IXboxLiveEndpointPair*>> rv;
		HRESULT hr = m_originalPtr->get_EndpointPairs(rv.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			auto vec = Make<collections::Vector<IXboxLiveEndpointPair*>>();

			uint32_t size;
			rv->get_Size(&size);

			for (uint32_t i = 0; i < size; i++)
			{
				ComPtr<IXboxLiveEndpointPair> origTpl;
				rv->GetAt(i, origTpl.GetAddressOf());

				ComPtr<XboxLiveEndpointPair> tpl;
				MakeAndInitialize<XboxLiveEndpointPair>(&tpl, origTpl);

				vec->Append(tpl.Get());
			}

			auto view = Make<collections::VectorView<IXboxLiveEndpointPair*, collections::Vector<IXboxLiveEndpointPair*>>>(vec.Get());
			hr = view.CopyTo(a1);
		}

		return hr;
	}
};

class XboxLiveEndpointPairTemplateStatics : public ActivationFactory<IXboxLiveEndpointPairTemplateStatics>
{
private:
	ComPtr<IXboxLiveEndpointPairTemplateStatics> m_originalPtr;
	ComPtr<IActivationFactory> m_originalFactory;

public:
	HRESULT RuntimeClassInitialize(ComPtr<IUnknown> ptr)
	{
		ptr.As(&m_originalPtr);
		ptr.As(&m_originalFactory);

		return S_OK;
	}

	virtual HRESULT ActivateInstance(_Outptr_result_nullonfailure_ IInspectable **ppvObject) override
	{
		return m_originalFactory->ActivateInstance(ppvObject);
	}

	STDMETHOD(GetTemplateByName)(HSTRING a1, IXboxLiveEndpointPairTemplate** a2) override
	{
		ComPtr<IXboxLiveEndpointPairTemplate> tpl;
		HRESULT hr = m_originalPtr->GetTemplateByName(a1, tpl.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<XboxLiveEndpointPairTemplate> proxyTpl;
			MakeAndInitialize<XboxLiveEndpointPairTemplate>(&proxyTpl, tpl);

			hr = proxyTpl.CopyTo(a2);
		}

		return hr;
	}

	STDMETHOD(get_Templates)(ABI::Windows::Foundation::Collections::IVectorView<IXboxLiveEndpointPairTemplate*>** a1) override
	{
		ComPtr<ABI::Windows::Foundation::Collections::IVectorView<IXboxLiveEndpointPairTemplate*>> rv;
		HRESULT hr = m_originalPtr->get_Templates(rv.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			auto vec = Make<collections::Vector<IXboxLiveEndpointPairTemplate*>>();

			uint32_t size;
			rv->get_Size(&size);

			for (uint32_t i = 0; i < size; i++)
			{
				ComPtr<IXboxLiveEndpointPairTemplate> origTpl;
				rv->GetAt(i, origTpl.GetAddressOf());

				ComPtr<XboxLiveEndpointPairTemplate> tpl;
				MakeAndInitialize<XboxLiveEndpointPairTemplate>(&tpl, origTpl);

				vec->Append(tpl.Get());
			}

			auto view = Make<collections::VectorView<IXboxLiveEndpointPairTemplate*, collections::Vector<IXboxLiveEndpointPairTemplate*>>>(vec.Get());
			hr = view.CopyTo(a1);
		}

		return hr;
	}
};

class XboxLiveDeviceAddressStatics : public ActivationFactory<IXboxLiveDeviceAddressStatics>
{
private:
	ComPtr<IXboxLiveDeviceAddressStatics> m_originalDevice;
	ComPtr<IActivationFactory> m_originalFactory;

public:
	HRESULT RuntimeClassInitialize(ComPtr<IUnknown> xboxLiveDevice)
	{
		xboxLiveDevice.As(&m_originalDevice);
		xboxLiveDevice.As(&m_originalFactory);

		return S_OK;
	}

	virtual HRESULT ActivateInstance(_Outptr_result_nullonfailure_ IInspectable **ppvObject) override
	{
		return m_originalFactory->ActivateInstance(ppvObject);
	}

	STDMETHOD(CreateFromSnapshotBase64)(HSTRING a1, IXboxLiveDeviceAddress** a2) override
	{
		ComPtr<IXboxLiveDeviceAddress> address;
		HRESULT hr = m_originalDevice->CreateFromSnapshotBase64(a1, address.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<XboxLiveDeviceAddress> proxyAddress;
			MakeAndInitialize<XboxLiveDeviceAddress>(&proxyAddress, address);

			hr = proxyAddress.CopyTo(a2);
		}

		return hr;
	}

	STDMETHOD(CreateFromSnapshotBuffer)(ABI::Windows::Storage::Streams::IBuffer* a1, IXboxLiveDeviceAddress** a2) override
	{
		ComPtr<IXboxLiveDeviceAddress> address;
		HRESULT hr = m_originalDevice->CreateFromSnapshotBuffer(a1, address.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<XboxLiveDeviceAddress> proxyAddress;
			MakeAndInitialize<XboxLiveDeviceAddress>(&proxyAddress, address);

			hr = proxyAddress.CopyTo(a2);
		}

		return hr;
	}

	STDMETHOD(CreateFromSnapshotBytes)(uint32_t a1, const BYTE* a2, IXboxLiveDeviceAddress** a3) override
	{
		ComPtr<IXboxLiveDeviceAddress> address;
		HRESULT hr = m_originalDevice->CreateFromSnapshotBytes(a1, a2, address.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<XboxLiveDeviceAddress> proxyAddress;
			MakeAndInitialize<XboxLiveDeviceAddress>(&proxyAddress, address);

			hr = proxyAddress.CopyTo(a3);
		}

		return hr;
	}

	STDMETHOD(GetLocal)(IXboxLiveDeviceAddress** a1) override
	{
		ComPtr<IXboxLiveDeviceAddress> address;
		HRESULT hr = m_originalDevice->GetLocal(address.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<XboxLiveDeviceAddress> proxyAddress;
			MakeAndInitialize<XboxLiveDeviceAddress>(&proxyAddress, address);

			hr = proxyAddress.CopyTo(a1);
		}

		return hr;
	}

	STDMETHOD(get_MaxSnapshotBytesSize)(uint32_t* a1) override
	{
		return m_originalDevice->get_MaxSnapshotBytesSize(a1);
	}
};

class XboxLiveEndpointPairStatics : public ActivationFactory<>
{
public:
	HRESULT RuntimeClassInitialize(ComPtr<IUnknown> origPtr)
	{
		return S_OK;
	}

	virtual HRESULT ActivateInstance(IInspectable** instance) override
	{
		return E_FAIL;
	}
};

class __declspec(uuid("4d682bce-a5d6-47e6-a236-cfde5fbdf2ed")) IXboxLiveQualityOfServiceMeasurement : public IInspectable
{
public:
	STDMETHOD(MeasureAsync)(IAsyncAction** a1) = 0;
	STDMETHOD(GetMetricResultsForDevice)(IXboxLiveDeviceAddress* a1, ABI::Windows::Foundation::Collections::IVectorView<void*>** a2) = 0; // XboxLiveQualityOfServiceMetricResult
	STDMETHOD(GetMetricResultsForMetric)(int a1, ABI::Windows::Foundation::Collections::IVectorView<void*>** a2) = 0; // XboxLiveQualityOfServiceMetricResult
	STDMETHOD(GetMetricResult)(IXboxLiveDeviceAddress* a1, int a2, void** a3) = 0; // same as above
	STDMETHOD(GetPrivatePayloadResult)(IXboxLiveDeviceAddress* a1, void** a2) = 0; // IXboxLiveQualityOfServicePrivatePayloadResult
	STDMETHOD(get_Metrics)(ABI::Windows::Foundation::Collections::IVector<int>**) = 0; // XboxLiveQualityOfServiceMetric
	STDMETHOD(get_DeviceAddresses)(ABI::Windows::Foundation::Collections::IVector<IXboxLiveDeviceAddress*>**) = 0;
	STDMETHOD(get_ShouldRequestPrivatePayloads)(boolean* a1) = 0;
	STDMETHOD(put_ShouldRequestPrivatePayloads)(boolean a1) = 0;
	STDMETHOD(get_TimeoutInMilliseconds)(uint32_t* a1) = 0;
	STDMETHOD(put_TimeoutInMilliseconds)(uint32_t a1) = 0;
	STDMETHOD(get_NumberOfProbesToAttempt)(uint32_t* a1) = 0;
	STDMETHOD(put_NumberOfProbesToAttempt)(uint32_t a1) = 0;
	STDMETHOD(get_NumberOfResultsPending)(uint32_t* a1) = 0;
	STDMETHOD(get_MetricResults)(ABI::Windows::Foundation::Collections::IVectorView<void*>** a1) = 0;
	STDMETHOD(get_PrivatePayloadResults)(ABI::Windows::Foundation::Collections::IVectorView<void*>** a1) = 0;
};

using namespace ABI::Windows::Foundation::Collections;

template<typename TElement, typename TImpl>
class VectorWrapper : public RuntimeClass<IVector<TElement*>>
{
private:
	ComPtr<IVector<TElement*>> m_originalPtr;

public:
	VectorWrapper(ComPtr<IVector<TElement*>> base)
	{
		m_originalPtr = base;
	}

	virtual HRESULT STDMETHODCALLTYPE GetAt(unsigned index, TElement** item) override
	{
		ComPtr<TElement> itemPtr;
		HRESULT hr = m_originalPtr->GetAt(index, itemPtr.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<TImpl> implPtr;
			MakeAndInitialize<TImpl>(&implPtr, itemPtr);

			hr = implPtr.CopyTo(item);
		}

		return hr;
	}

	virtual /* propget */ HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned *size) override
	{
		return m_originalPtr->get_Size(size);
	}

	virtual HRESULT STDMETHODCALLTYPE GetView(_Outptr_result_maybenull_ IVectorView<TElement*> **view) override
	{
		return m_originalPtr->GetView(view);
	}

	virtual HRESULT STDMETHODCALLTYPE IndexOf(_In_opt_ TElement* value, _Out_ unsigned *index, _Out_ boolean *found) override
	{
		ComPtr<TElement> element = value;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(element.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(TElement), (void**)element.ReleaseAndGetAddressOf());
		}

		return m_originalPtr->IndexOf(element.Get(), index, found);
	}

	// write methods
	virtual HRESULT STDMETHODCALLTYPE SetAt(_In_ unsigned index, _In_opt_ TElement* item) override
	{
		ComPtr<TElement> element = item;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(element.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(TElement), (void**)element.ReleaseAndGetAddressOf());
		}

		return m_originalPtr->SetAt(index, element.Get());
	}

	virtual HRESULT STDMETHODCALLTYPE InsertAt(_In_ unsigned index, _In_opt_ TElement* item) override
	{
		ComPtr<TElement> element = item;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(element.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(TElement), (void**)element.ReleaseAndGetAddressOf());
		}

		return m_originalPtr->InsertAt(index, element.Get());
	}

	virtual HRESULT STDMETHODCALLTYPE RemoveAt(_In_ unsigned index) override
	{
		return m_originalPtr->RemoveAt(index);
	}

	virtual HRESULT STDMETHODCALLTYPE Append(_In_opt_ TElement* item) override
	{
		ComPtr<TElement> element = item;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(element.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(TElement), (void**)element.ReleaseAndGetAddressOf());
		}

		return m_originalPtr->Append(element.Get());
	}

	virtual HRESULT STDMETHODCALLTYPE RemoveAtEnd() override
	{
		return m_originalPtr->RemoveAtEnd();
	}

	virtual HRESULT STDMETHODCALLTYPE Clear() override
	{
		return m_originalPtr->Clear();
	}
};

class XboxLiveQualityOfServiceMeasurement : public RuntimeClass<IXboxLiveQualityOfServiceMeasurement>
{
private:
	ComPtr<IXboxLiveQualityOfServiceMeasurement> m_originalPtr;

public:
	HRESULT RuntimeClassInitialize(ComPtr<IUnknown> origPtr)
	{
		origPtr.As(&m_originalPtr);

		return S_OK;
	}

	STDMETHOD(MeasureAsync)(IAsyncAction** a1) override
	{
		return m_originalPtr->MeasureAsync(a1);
	}

	STDMETHOD(GetMetricResultsForDevice)(IXboxLiveDeviceAddress* a1, ABI::Windows::Foundation::Collections::IVectorView<void*>** a2) override
	{
		ComPtr<IXboxLiveDeviceAddress> device = a1;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(device.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(IXboxLiveDeviceAddress), (void**)device.ReleaseAndGetAddressOf());
		}

		return m_originalPtr->GetMetricResultsForDevice(device.Get(), a2);
	}

	STDMETHOD(GetMetricResultsForMetric)(int a1, ABI::Windows::Foundation::Collections::IVectorView<void*>** a2) override
	{
		return m_originalPtr->GetMetricResultsForMetric(a1, a2);
	}

	STDMETHOD(GetMetricResult)(IXboxLiveDeviceAddress* a1, int a2, void** a3) override
	{
		ComPtr<IXboxLiveDeviceAddress> device = a1;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(device.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(IXboxLiveDeviceAddress), (void**)device.ReleaseAndGetAddressOf());
		}

		return m_originalPtr->GetMetricResult(device.Get(), a2, a3);
	}

	STDMETHOD(GetPrivatePayloadResult)(IXboxLiveDeviceAddress* a1, void** a2) override
	{
		ComPtr<IXboxLiveDeviceAddress> device = a1;
		ComPtr<IHasOriginalPtr> hasOriginal;

		if (SUCCEEDED(device.As(&hasOriginal)))
		{
			hasOriginal->GetOriginalPointer(__uuidof(IXboxLiveDeviceAddress), (void**)device.ReleaseAndGetAddressOf());
		}

		return m_originalPtr->GetPrivatePayloadResult(device.Get(), a2);
	}

	STDMETHOD(get_Metrics)(ABI::Windows::Foundation::Collections::IVector<int>** a1) override
	{
		return m_originalPtr->get_Metrics(a1);
	}

	STDMETHOD(get_DeviceAddresses)(ABI::Windows::Foundation::Collections::IVector<IXboxLiveDeviceAddress*>** a1) override
	{
		ComPtr<ABI::Windows::Foundation::Collections::IVector<IXboxLiveDeviceAddress*>> vector;
		HRESULT hr = m_originalPtr->get_DeviceAddresses(vector.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			auto wrapper = Make<VectorWrapper<IXboxLiveDeviceAddress, XboxLiveDeviceAddress>>(vector);

			hr = wrapper.CopyTo(a1);
		}

		return hr;
	}

	STDMETHOD(get_ShouldRequestPrivatePayloads)(boolean* a1) override
	{
		return m_originalPtr->get_ShouldRequestPrivatePayloads(a1);
	}

	STDMETHOD(put_ShouldRequestPrivatePayloads)(boolean a1) override
	{
		return m_originalPtr->put_ShouldRequestPrivatePayloads(a1);
	}

	STDMETHOD(get_TimeoutInMilliseconds)(uint32_t* a1) override
	{
		return m_originalPtr->get_TimeoutInMilliseconds(a1);
	}

	STDMETHOD(put_TimeoutInMilliseconds)(uint32_t a1) override
	{
		return m_originalPtr->put_TimeoutInMilliseconds(a1);
	}

	STDMETHOD(get_NumberOfProbesToAttempt)(uint32_t* a1) override
	{
		return m_originalPtr->get_NumberOfProbesToAttempt(a1);
	}

	STDMETHOD(put_NumberOfProbesToAttempt)(uint32_t a1) override
	{
		return m_originalPtr->put_NumberOfProbesToAttempt(a1);
	}

	STDMETHOD(get_NumberOfResultsPending)(uint32_t* a1) override
	{
		return m_originalPtr->get_NumberOfResultsPending(a1);
	}

	STDMETHOD(get_MetricResults)(ABI::Windows::Foundation::Collections::IVectorView<void*>** a1) override
	{
		return m_originalPtr->get_MetricResults(a1);
	}

	STDMETHOD(get_PrivatePayloadResults)(ABI::Windows::Foundation::Collections::IVectorView<void*>** a1) override
	{
		return m_originalPtr->get_PrivatePayloadResults(a1);
	}
};

class __declspec(uuid("6e352dca-23cf-440a-b077-5e30857a8234")) IXboxLiveQualityOfServiceMeasurementStatics : public IInspectable
{
public:
	STDMETHOD(PublishPrivatePayloadBytes)(uint32_t a1, const BYTE* a2) = 0;
	STDMETHOD(ClearPrivatePayload)() = 0;
	STDMETHOD(get_MaxSimultaneousProbeConnections)(uint32_t* a1) = 0;
	STDMETHOD(put_MaxSimultaneousProbeConnections)(uint32_t a1) = 0;
	STDMETHOD(get_IsSystemOutboundBandwidthConstrained)(boolean* a1) = 0;
	STDMETHOD(put_IsSystemOutboundBandwidthConstrained)(boolean a1) = 0;
	STDMETHOD(get_IsSystemInboundBandwidthConstrained)(boolean* a1) = 0;
	STDMETHOD(put_IsSystemInboundBandwidthConstrained)(boolean a1) = 0;
	STDMETHOD(get_PublishedPrivatePayload)(ABI::Windows::Storage::Streams::IBuffer** a1) = 0;
	STDMETHOD(put_PublishedPrivatePayload)(ABI::Windows::Storage::Streams::IBuffer* a1) = 0;
	STDMETHOD(get_MaxPrivatePayloadSize)(uint32_t* a1) = 0;
};

class XboxLiveQualityOfServiceMeasurementStatics : public ActivationFactory<IXboxLiveQualityOfServiceMeasurementStatics>
{
private:
	ComPtr<IActivationFactory> m_activationFactory;
	ComPtr<IXboxLiveQualityOfServiceMeasurementStatics> m_originalPtr;

public:
	HRESULT RuntimeClassInitialize(ComPtr<IUnknown> origPtr)
	{
		origPtr.As(&m_activationFactory);
		origPtr.As(&m_originalPtr);

		return S_OK;
	}

	virtual HRESULT ActivateInstance(IInspectable** instance) override
	{
		ComPtr<IInspectable> inst;
		HRESULT hr = m_activationFactory->ActivateInstance(inst.GetAddressOf());

		if (SUCCEEDED(hr))
		{
			ComPtr<XboxLiveQualityOfServiceMeasurement> measurement;
			MakeAndInitialize<XboxLiveQualityOfServiceMeasurement>(&measurement, inst);

			hr = measurement.CopyTo(instance);
		}

		return hr;
	}

	STDMETHOD(PublishPrivatePayloadBytes)(uint32_t a1, const BYTE* a2) override
	{
		return m_originalPtr->PublishPrivatePayloadBytes(a1, a2);
	}

	STDMETHOD(ClearPrivatePayload)() override
	{
		return m_originalPtr->ClearPrivatePayload();
	}

	STDMETHOD(get_MaxSimultaneousProbeConnections)(uint32_t* a1) override
	{
		return m_originalPtr->get_MaxSimultaneousProbeConnections(a1);
	}

	STDMETHOD(put_MaxSimultaneousProbeConnections)(uint32_t a1) override
	{
		return m_originalPtr->put_MaxSimultaneousProbeConnections(a1);
	}

	STDMETHOD(get_IsSystemOutboundBandwidthConstrained)(boolean* a1) override
	{
		return m_originalPtr->get_IsSystemOutboundBandwidthConstrained(a1);
	}

	STDMETHOD(put_IsSystemOutboundBandwidthConstrained)(boolean a1) override
	{
		return m_originalPtr->put_IsSystemOutboundBandwidthConstrained(a1);
	}

	STDMETHOD(get_IsSystemInboundBandwidthConstrained)(boolean* a1) override
	{
		return m_originalPtr->get_IsSystemInboundBandwidthConstrained(a1);
	}

	STDMETHOD(put_IsSystemInboundBandwidthConstrained)(boolean a1) override
	{
		return m_originalPtr->put_IsSystemInboundBandwidthConstrained(a1);
	}

	STDMETHOD(get_PublishedPrivatePayload)(ABI::Windows::Storage::Streams::IBuffer** a1) override
	{
		return m_originalPtr->get_PublishedPrivatePayload(a1);
	}

	STDMETHOD(put_PublishedPrivatePayload)(ABI::Windows::Storage::Streams::IBuffer* a1) override
	{
		return m_originalPtr->put_PublishedPrivatePayload(a1);
	}

	STDMETHOD(get_MaxPrivatePayloadSize)(uint32_t* a1) override
	{
		return m_originalPtr->get_MaxPrivatePayloadSize(a1);
	}
};

HRESULT STDMETHODCALLTYPE XboxLiveEndpointPair::get_Template(IXboxLiveEndpointPairTemplate** a1)
{
	ComPtr<IXboxLiveEndpointPairTemplate> ptr;
	HRESULT hr = m_originalPtr->get_Template(ptr.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		ComPtr<XboxLiveEndpointPairTemplate> result;
		MakeAndInitialize<XboxLiveEndpointPairTemplate>(&result, ptr);

		hr = result.CopyTo(a1);
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE XboxLiveEndpointPair::get_RemoteDeviceAddress(IXboxLiveDeviceAddress** a1)
{
	ComPtr<IXboxLiveDeviceAddress> ptr;
	HRESULT hr = m_originalPtr->get_RemoteDeviceAddress(ptr.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		ComPtr<XboxLiveDeviceAddress> result;
		MakeAndInitialize<XboxLiveDeviceAddress>(&result, ptr);

		hr = result.CopyTo(a1);
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE XboxLiveEndpointPairCreationResult::get_EndpointPair(IXboxLiveEndpointPair** a1)
{
	ComPtr<IXboxLiveEndpointPair> ptr;
	HRESULT hr = m_originalPtr->get_EndpointPair(ptr.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		ComPtr<XboxLiveEndpointPair> result;
		MakeAndInitialize<XboxLiveEndpointPair>(&result, ptr);

		hr = result.CopyTo(a1);
	}

	return hr;
}

HRESULT STDMETHODCALLTYPE XboxLiveEndpointPairCreationResult::get_DeviceAddress(IXboxLiveDeviceAddress** a1)
{
	ComPtr<IXboxLiveDeviceAddress> ptr;
	HRESULT hr = m_originalPtr->get_DeviceAddress(ptr.GetAddressOf());

	if (SUCCEEDED(hr))
	{
		ComPtr<XboxLiveDeviceAddress> result;
		MakeAndInitialize<XboxLiveDeviceAddress>(&result, ptr);

		hr = result.CopyTo(a1);
	}

	return hr;
}

template<typename T>
HRESULT MakeFactory(IUnknown* factory, IActivationFactory** outFactory)
{
	ComPtr<IUnknown> unknown(factory);

	return MakeAndInitialize<T>(outFactory, unknown);
}

void MakeXBLActivationProxies()
{
	ImHost_AddActivationProxy(L"Windows.Networking.XboxLive.XboxLiveDeviceAddress", MakeFactory<XboxLiveDeviceAddressStatics>);
	ImHost_AddActivationProxy(L"Windows.Networking.XboxLive.XboxLiveEndpointPair", MakeFactory<XboxLiveEndpointPairStatics>);
	ImHost_AddActivationProxy(L"Windows.Networking.XboxLive.XboxLiveEndpointPairTemplate", MakeFactory<XboxLiveEndpointPairTemplateStatics>);
	ImHost_AddActivationProxy(L"Windows.Networking.XboxLive.XboxLiveQualityOfServiceMeasurement", MakeFactory<XboxLiveQualityOfServiceMeasurementStatics>);
}