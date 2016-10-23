#include "StdInc.h"
#include <Windows.Services.Store.h>
#include <StubInternal.h>

#include <experimental/filesystem>

#include <Map.h>
#include <ShlObj.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Services::Store;
using namespace ABI::Windows::System;

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
					IMap<HSTRING, ABI::Windows::Services::Store::StoreProduct*> : IMap_impl<HSTRING, ABI::Windows::Foundation::Internal::AggregateType<ABI::Windows::Services::Store::StoreProduct*, ABI::Windows::Services::Store::IStoreProduct*>>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IMap`2<String, Windows.Services.Store.StoreProduct>";
					}
				};
				typedef IMap<HSTRING, ABI::Windows::Services::Store::StoreProduct*> __FIMap_2_HSTRING_Windows__CServices__CStore__CStoreProduct_t;

				template <>
				struct __declspec(uuid("7d0d74f5-4020-54aa-9f3d-0f17127abeed"))
					IVector<IKeyValuePair<HSTRING, ABI::Windows::Services::Store::StoreProduct*>*> : IVector_impl<IKeyValuePair<HSTRING, ABI::Windows::Services::Store::StoreProduct*>*>
				{
					static const wchar_t* z_get_rc_name_impl()
					{
						return L"Windows.Foundation.Collections.IKeyValuePair`2<String, Windows.Services.Store.StoreProduct>";
					}
				};

				template <>
				struct __declspec(uuid("7d0d74f5-4020-54aa-9f3d-0f17127abeee"))
					IVectorView<IKeyValuePair<HSTRING, ABI::Windows::Services::Store::StoreProduct*>*> : IVectorView_impl<IKeyValuePair<HSTRING, ABI::Windows::Services::Store::StoreProduct*>*>
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

#include <json.hpp>
#include <fstream>
#include <sstream>
#include <codecvt>

using json = nlohmann::json;
namespace fs = std::experimental::filesystem;

static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> g_converter;

std::wstring ToWide(const std::string& narrow)
{
	return g_converter.from_bytes(narrow);
}

HRESULT ToHString(const json& jsonField, HSTRING* out)
{
	std::wstring wstr = ToWide(jsonField.get<std::string>());
	return WindowsCreateString(wstr.c_str(), wstr.size(), out);
}

class CustomStoreProduct : public RuntimeClass<IStoreProduct>
{
private:
	json m_data;

public:
	CustomStoreProduct(const fs::path& filePath)
	{
		std::ifstream productFile(filePath.c_str());
		m_data = json::parse(productFile);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_StoreId(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value) override
	{
		ToHString(m_data["StoreId"], value);
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Language(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value) override
	{
		ToHString(m_data["Language"], value);
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Title(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value) override
	{
		ToHString(m_data["Title"], value);
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Description(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value) override
	{
		ToHString(m_data["Description"], value);
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ProductKind(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value) override
	{
		ToHString(m_data["ProductKind"], value);
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HasDigitalDownload(
		/* [out][retval] */ __RPC__out boolean *value) override
	{
		*value = m_data["HasDigitalDownload"].get<bool>();
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Keywords(
		/* [out][retval] */ __RPC__deref_out_opt __FIVectorView_1_HSTRING **value) override
	{
		auto view = Make<EmptyVectorView<HSTRING>>();
		return view.CopyTo(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Images(
		/* [out][retval] */ __RPC__deref_out_opt __FIVectorView_1_Windows__CServices__CStore__CStoreImage **value) override
	{
		auto view = Make<EmptyVectorView<StoreImage*, IStoreImage*>>();
		return view.CopyTo(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Videos(
		/* [out][retval] */ __RPC__deref_out_opt __FIVectorView_1_Windows__CServices__CStore__CStoreVideo **value) override
	{
		auto view = Make<EmptyVectorView<StoreVideo*, IStoreVideo*>>();
		return view.CopyTo(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Skus(
		/* [out][retval] */ __RPC__deref_out_opt __FIVectorView_1_Windows__CServices__CStore__CStoreSku **value) override
	{
		auto view = Make<EmptyVectorView<StoreSku*, IStoreSku*>>();
		return view.CopyTo(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IsInUserCollection(
		/* [out][retval] */ __RPC__out boolean *value) override
	{
		*value = true;
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Price(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Services::Store::IStorePrice **value) override
	{
		return E_NOTIMPL;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ExtendedJsonData(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value) override
	{
		ToHString(m_data["ExtendedJsonData"], value);
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_LinkUri(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Foundation::IUriRuntimeClass **value) override
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetIsAnySkuInstalledAsync(
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_boolean **operation) override
	{
		*operation = (std::remove_pointer_t<decltype(operation)>)(0xCEFED);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE RequestPurchaseAsync(
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CServices__CStore__CStorePurchaseResult **operation) override
	{
		*operation = (std::remove_pointer_t<decltype(operation)>)(0xCEDEF);
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE RequestPurchaseWithPurchasePropertiesAsync(
		/* [in] */ __RPC__in_opt ABI::Windows::Services::Store::IStorePurchaseProperties *storePurchaseProperties,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CServices__CStore__CStorePurchaseResult **operation) override
	{
		*operation = (std::remove_pointer_t<decltype(operation)>)(0xCADEFADE);
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_InAppOfferToken(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value) override
	{
		ToHString(m_data["InAppOfferToken"], value);
		return S_OK;
	}
};

#include <PackageIdentity.h>

class CustomProductQueryResult : public RuntimeClass<IStoreProductQueryResult>
{
public:
	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Products(
		/* [out][retval] */ __RPC__deref_out_opt __FIMapView_2_HSTRING_Windows__CServices__CStore__CStoreProduct **value)
	{
		auto productMap = Make<collections::Map<HSTRING, StoreProduct*>>();

		PWSTR appDataPath;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_SavedGames, 0, nullptr, &appDataPath)))
		{
			// create the directory if not existent
			std::wstring dataPath = std::wstring(appDataPath) + L"\\ImmersiveHost";
			CreateDirectory(dataPath.c_str(), nullptr);

			dataPath += L"\\";
			dataPath += GetCurrentPackageIdentity()->GetFamilyName();

			CreateDirectory(dataPath.c_str(), nullptr);

			try
			{
				// look for entries
				for (auto& dir : fs::directory_iterator(dataPath + L"\\iap"))
				{
					if (dir.path().extension() == ".json")
					{
						auto product = Make<CustomStoreProduct>(dir.path());

						HString storeIdStr;
						product->get_StoreId(storeIdStr.GetAddressOf());

						boolean replaced;
						productMap->Insert(storeIdStr.Get(), product.Get(), &replaced);
					}
				}
			}
			catch (std::exception)
			{

			}

			CoTaskMemFree(appDataPath);
		}

		return productMap->GetView(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ExtendedError(
		/* [out][retval] */ __RPC__out HRESULT *value)
	{
		*value = S_OK;
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
		auto productsStub = Make<CustomProductQueryResult>();

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