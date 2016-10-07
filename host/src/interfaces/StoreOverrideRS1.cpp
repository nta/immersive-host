#include "StdInc.h"
#include <Windows.Services.Store.h>

#include <Map.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Services::Store;
using namespace ABI::Windows::System;

template<typename T>
struct TypeStubs
{
	using StorageType = ComPtr<std::remove_pointer_t<T>>;

	static inline void Copy(ComPtr<std::remove_pointer_t<T>> result, T* out)
	{
		result.CopyTo(out);
	}
};

template<>
struct TypeStubs<HSTRING>
{
	using StorageType = HString;

	static inline void Copy(HString result, HSTRING* out)
	{
		result.CopyTo(out);
	}
};

template<>
struct TypeStubs<int>
{
	using StorageType = int;

	static inline void Copy(int result, int* out)
	{
		*out = result;
	}
};

template<typename T, typename TIface = T>
class AsyncOperationStub : public RuntimeClass<AsyncBase<IAsyncOperationCompletedHandler<T>>, IAsyncOperation<T>>
{
private:
	typename TypeStubs<TIface>::StorageType m_result;

public:
	AsyncOperationStub(typename TypeStubs<TIface>::StorageType result)
	{
		m_result = result;

		Start();

		FireCompletion();
	}

	virtual auto STDMETHODCALLTYPE put_Completed(IAsyncOperationCompletedHandler<T>* handler) -> HRESULT override
	{
		return this->PutOnComplete(handler);
	}

	virtual auto STDMETHODCALLTYPE get_Completed(IAsyncOperationCompletedHandler<T>** handler) -> HRESULT override
	{
		return this->GetOnComplete(handler);
	}

	virtual auto STDMETHODCALLTYPE GetResults(TIface* results)  -> HRESULT override
	{
		TypeStubs<TIface>::Copy(m_result, results);

		return S_OK;
	}

protected:

	virtual auto OnStart()  -> HRESULT override { return S_OK; }
	virtual auto OnClose()  -> void    override {};
	virtual auto OnCancel() -> void    override {};
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
				struct __declspec(uuid("7d0d74f5-4020-54aa-9f3d-0f17127abeef"))
					IMap<HSTRING, ABI::Windows::Services::Store::StoreProduct*> : IMapView_impl<HSTRING, ABI::Windows::Foundation::Internal::AggregateType<ABI::Windows::Services::Store::StoreProduct*, ABI::Windows::Services::Store::IStoreProduct*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IMap`2<String, Windows.Services.Store.StoreProduct>";
					}
				};
				typedef IMap<HSTRING, ABI::Windows::Services::Store::StoreProduct*> __FIMap_2_HSTRING_Windows__CServices__CStore__CStoreProduct_t;

				template <>
				struct __declspec(uuid("7d0d74f5-4020-54aa-9f3d-0f17127abeed"))
					IVector<IKeyValuePair<HSTRING, ABI::Windows::Services::Store::StoreProduct*>*> : IVector_impl<IKeyValuePair_impl<HSTRING, ABI::Windows::Foundation::Internal::AggregateType<ABI::Windows::Services::Store::StoreProduct*, ABI::Windows::Services::Store::IStoreProduct*>>*>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IKeyValuePair`2<String, Windows.Services.Store.StoreProduct>";
					}
				};

				template <>
				struct __declspec(uuid("7d0d74f5-4020-54aa-9f3d-0f17127abeee"))
					IVectorView<IKeyValuePair<HSTRING, ABI::Windows::Services::Store::StoreProduct*>*> : IVectorView_impl<IKeyValuePair_impl<HSTRING, ABI::Windows::Foundation::Internal::AggregateType<ABI::Windows::Services::Store::StoreProduct*, ABI::Windows::Services::Store::IStoreProduct*>>*>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IKeyValuePair`2<String, Windows.Services.Store.StoreProduct>";
					}
				};
				/* ABI */
			} /* Windows */
		} /* Foundation */
	} /* Collections */
}

template<typename T, typename TAbi = T>
class EmptyIterator : public RuntimeClass<ABI::Windows::Foundation::Collections::IIterator<T>>
{
public:
	virtual /* propget */ HRESULT STDMETHODCALLTYPE get_Current(_Out_ TAbi *current) override
	{
		*current = nullptr;
		return S_OK;
	}
	virtual /* propget */ HRESULT STDMETHODCALLTYPE get_HasCurrent(_Out_ boolean *hasCurrent) override
	{
		*hasCurrent = false;
		return S_OK;
	}
	virtual HRESULT STDMETHODCALLTYPE MoveNext(_Out_ boolean *hasCurrent) override
	{
		*hasCurrent = false;
		return S_FALSE;
	}
	virtual HRESULT STDMETHODCALLTYPE GetMany(_In_ unsigned capacity, _Out_writes_to_(capacity, *actual) TAbi *value, _Out_ unsigned *actual) override
	{
		*actual = 0;
		return S_OK;
	}
};

class ProductQueryResultMapView : public Microsoft::WRL::RuntimeClass<ABI::Windows::Foundation::Collections::IMapView<HSTRING, StoreProduct*>,
	ABI::Windows::Foundation::Collections::IIterable<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING, StoreProduct*>*>>
{
public:
	virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned *size)
	{
		*size = 0;
		return S_OK;
	};


	virtual HRESULT STDMETHODCALLTYPE HasKey(HSTRING key, _Out_ boolean* hasKey)
	{
		*hasKey = false;
		return S_OK;
	}


	virtual HRESULT STDMETHODCALLTYPE Lookup(HSTRING key, IStoreProduct** item)
	{
		*item = nullptr;
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE Split(ABI::Windows::Foundation::Collections::IMapView<HSTRING, StoreProduct*>** first, ABI::Windows::Foundation::Collections::IMapView<HSTRING, StoreProduct*>** second)
	{
		*first = nullptr;
		*second = nullptr;
		return S_OK;
	}


	virtual HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING, StoreProduct*>*>** first)
	{
		auto it = Make<EmptyIterator<ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING, StoreProduct*>*>>();
		
		return it.CopyTo(first);
	}
};

template<typename T, typename TIface = T>
class EmptyVectorView : public RuntimeClass<ABI::Windows::Foundation::Collections::IVectorView<T>, ABI::Windows::Foundation::Collections::IIterable<T>>
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetAt(_In_ unsigned index, _Out_ TIface *item) override
	{
		return S_OK;
	}

	virtual /* propget */ HRESULT STDMETHODCALLTYPE get_Size(_Out_ unsigned *size) override
	{
		*size = 0;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE IndexOf(_In_opt_ TIface value, _Out_ unsigned *index, _Out_ boolean *found) override
	{
		*found = false;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetMany(_In_  unsigned startIndex, _In_ unsigned capacity, _Out_writes_to_(capacity, *actual) TIface *value, _Out_ unsigned *actual) override
	{
		*actual = false;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE First(ABI::Windows::Foundation::Collections::IIterator<T>** first) override
	{
		auto it = Make<EmptyIterator<T, TIface>>();

		return it.CopyTo(first);
	}
};

class ProductQueryResultStub : public RuntimeClass<IStoreProductQueryResult>
{
public:
	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Products(
		/* [out][retval] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_Windows__CServices__CStore__CStoreProduct **value)
	{
		auto map = Make<ProductQueryResultMapView>();

		return map.CopyTo(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ExtendedError(
		/* [out][retval] */ __RPC__out HRESULT *value)
	{
		*value = S_OK;
		return S_OK;
	}
};

class AppLicenseStub : public RuntimeClass<IStoreAppLicense>
{
public:
	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SkuStoreId(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value)
	{
		return WindowsCreateString(L"meow", 4, value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsActive(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		*value = true;
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsTrial(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		*value = false;
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ExpirationDate(
		/* [out][retval] */ __RPC__out ABI::Windows::Foundation::DateTime *value)
	{
		SYSTEMTIME endOfWorld;
		endOfWorld.wYear = 2038;
		endOfWorld.wMonth = 1;
		endOfWorld.wDay = 1;
		endOfWorld.wDayOfWeek = 0;
		endOfWorld.wHour = 1;
		endOfWorld.wMinute = 0;
		endOfWorld.wSecond = 0;
		endOfWorld.wMilliseconds = 0;

		FILETIME ft;
		SystemTimeToFileTime(&endOfWorld, &ft);

		ULARGE_INTEGER ul = { ft.dwLowDateTime, ft.dwHighDateTime };

		value->UniversalTime = ul.QuadPart;
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ExtendedJsonData(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value)
	{
		return WindowsCreateString(L"{}", 2, value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AddOnLicenses(
		/* [out][retval] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_Windows__CServices__CStore__CStoreLicense **value)
	{
		*value = (__FIMapView_2_HSTRING_Windows__CServices__CStore__CStoreLicense *)(0xCDEDCDED);

		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TrialTimeRemaining(
		/* [out][retval] */ __RPC__out ABI::Windows::Foundation::TimeSpan *value)
	{
		value->Duration = 0;

		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsTrialOwnedByThisUser(
		/* [out][retval] */ __RPC__out boolean *value)
	{
		*value = false;

		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TrialUniqueId(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value)
	{
		return WindowsCreateString(L"{}", 2, value);
	}
};

class StoreContextStatics : public ActivationFactory<Implements<IStoreContext>, IStoreContextStatics>
{
private:

public:
	virtual STDMETHOD(get_User)(IUser **value)
	{
		return S_OK;
	}

	virtual STDMETHOD(add_OfflineLicensesChanged)(__FITypedEventHandler_2_Windows__CServices__CStore__CStoreContext_IInspectable *handler, EventRegistrationToken *token)
	{
		return S_OK;
	}

	virtual STDMETHOD(remove_OfflineLicensesChanged)(EventRegistrationToken token)
	{
		return S_OK;
	}

	virtual STDMETHOD(GetCustomerPurchaseIdAsync)(HSTRING serviceTicket, HSTRING publisherUserId, __FIAsyncOperation_1_HSTRING **operation)
	{
		return S_OK;
	}

	virtual STDMETHOD(GetCustomerCollectionsIdAsync)(HSTRING serviceTicket, HSTRING publisherUserId, __FIAsyncOperation_1_HSTRING **operation)
	{
		return S_OK;
	}

	virtual STDMETHOD(GetAppLicenseAsync)(__FIAsyncOperation_1_Windows__CServices__CStore__CStoreAppLicense **operation)
	{
		auto licenseStub = Make<AppLicenseStub>();

		ComPtr<IStoreAppLicense> ptr;
		licenseStub.As(&ptr);

		auto operationStub = Make<AsyncOperationStub<StoreAppLicense*, IStoreAppLicense*>>(ptr);
		return operationStub.CopyTo(operation);
	}

	virtual STDMETHOD(GetStoreProductForCurrentAppAsync)(__FIAsyncOperation_1_Windows__CServices__CStore__CStoreProductResult **operation)
	{
		return S_OK;
	}

	virtual STDMETHOD(GetStoreProductsAsync)(__FIIterable_1_HSTRING *productKinds, __FIIterable_1_HSTRING *storeIds, __FIAsyncOperation_1_Windows__CServices__CStore__CStoreProductQueryResult **operation)
	{
		return S_OK;
	}

	virtual STDMETHOD(GetAssociatedStoreProductsAsync)(__FIIterable_1_HSTRING *productKinds, __FIAsyncOperation_1_Windows__CServices__CStore__CStoreProductQueryResult **operation)
	{
		auto productsStub = Make<ProductQueryResultStub>();

		ComPtr<IStoreProductQueryResult> ptr;
		productsStub.As(&ptr);

		auto operationStub = Make<AsyncOperationStub<StoreProductQueryResult*, IStoreProductQueryResult*>>(ptr);
		return operationStub.CopyTo(operation);
	}

	virtual HRESULT STDMETHODCALLTYPE GetAssociatedStoreProductsWithPagingAsync(
		__FIIterable_1_HSTRING *productKinds,
		UINT32 maxItemsToRetrievePerPage,
		__FIAsyncOperation_1_Windows__CServices__CStore__CStoreProductPagedQueryResult **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUserCollectionAsync(
		__FIIterable_1_HSTRING *productKinds,
		__FIAsyncOperation_1_Windows__CServices__CStore__CStoreProductQueryResult **operation)
	{
		auto productsStub = Make<ProductQueryResultStub>();

		ComPtr<IStoreProductQueryResult> ptr;
		productsStub.As(&ptr);

		auto operationStub = Make<AsyncOperationStub<StoreProductQueryResult*, IStoreProductQueryResult*>>(ptr);
		return operationStub.CopyTo(operation);
	}

	virtual HRESULT STDMETHODCALLTYPE GetUserCollectionWithPagingAsync(
		__FIIterable_1_HSTRING *productKinds,
		UINT32 maxItemsToRetrievePerPage,
		__FIAsyncOperation_1_Windows__CServices__CStore__CStoreProductPagedQueryResult **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE ReportConsumableFulfillmentAsync(
		HSTRING productStoreId,
		UINT32 quantity,
		GUID trackingId,
		__FIAsyncOperation_1_Windows__CServices__CStore__CStoreConsumableResult **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetConsumableBalanceRemainingAsync(
		HSTRING productStoreId,
		__FIAsyncOperation_1_Windows__CServices__CStore__CStoreConsumableResult **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE AcquireStoreLicenseForOptionalPackageAsync(
		ABI::Windows::ApplicationModel::IPackage *optionalPackage,
		__FIAsyncOperation_1_Windows__CServices__CStore__CStoreAcquireLicenseResult **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE RequestPurchaseAsync(
		HSTRING storeId,
		__FIAsyncOperation_1_Windows__CServices__CStore__CStorePurchaseResult **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE RequestPurchaseWithPurchasePropertiesAsync(
		HSTRING storeId,
		ABI::Windows::Services::Store::IStorePurchaseProperties *storePurchaseProperties,
		__FIAsyncOperation_1_Windows__CServices__CStore__CStorePurchaseResult **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetAppAndOptionalStorePackageUpdatesAsync(
		__FIAsyncOperation_1___FIVectorView_1_Windows__CServices__CStore__CStorePackageUpdate **operation)
	{
		auto productsStub = Make<EmptyVectorView<StorePackageUpdate*, IStorePackageUpdate*>>();

		ComPtr<ABI::Windows::Foundation::Collections::IVectorView<StorePackageUpdate*>> ptr;
		productsStub.As(&ptr);

		auto operationStub = Make<AsyncOperationStub<ABI::Windows::Foundation::Collections::IVectorView<StorePackageUpdate*>*>>(ptr);
		return operationStub.CopyTo(operation);
	}

	virtual HRESULT STDMETHODCALLTYPE RequestDownloadStorePackageUpdatesAsync(
		__FIIterable_1_Windows__CServices__CStore__CStorePackageUpdate *storePackageUpdates,
		__FIAsyncOperationWithProgress_2_Windows__CServices__CStore__CStorePackageUpdateResult_Windows__CServices__CStore__CStorePackageUpdateStatus **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE RequestDownloadAndInstallStorePackageUpdatesAsync(
		__FIIterable_1_Windows__CServices__CStore__CStorePackageUpdate *storePackageUpdates,
		__FIAsyncOperationWithProgress_2_Windows__CServices__CStore__CStorePackageUpdateResult_Windows__CServices__CStore__CStorePackageUpdateStatus **operation)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE RequestDownloadAndInstallStorePackagesAsync(
		__FIIterable_1_HSTRING *storeIds,
		__FIAsyncOperationWithProgress_2_Windows__CServices__CStore__CStorePackageUpdateResult_Windows__CServices__CStore__CStorePackageUpdateStatus **operation)
	{
		return S_OK;
	}

public:
	STDMETHOD(GetDefault)(IStoreContext** value)
	{
		return AsIID(this, IID_IStoreContext, (void**)value);
	}

	STDMETHOD(GetForUser)(IUser* user, IStoreContext** value)
	{
		return AsIID(this, IID_IStoreContext, (void**)value);
	}
};

void RegisterStoreOverrideRS1()
{
	ImHost_AddActivationFactory(L"Windows.Services.Store.StoreContext", [] (HSTRING, IActivationFactory** factory)
	{
		auto app = Make<StoreContextStatics>();
		return app.CopyTo(factory);
	});
}