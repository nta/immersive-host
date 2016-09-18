#include "StdInc.h"
#include <Windows.ApplicationModel.Store.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::ApplicationModel::Store;

class AppReceiptOperation : public RuntimeClass<AsyncBase<IAsyncOperationCompletedHandler<HSTRING>>, IAsyncOperation<HSTRING>>
{
public:
	AppReceiptOperation()
	{
		Start();

		FireCompletion();
	}

	virtual auto STDMETHODCALLTYPE put_Completed(IAsyncOperationCompletedHandler<HSTRING>* handler) -> HRESULT override
	{
		return this->PutOnComplete(handler);
	}

	virtual auto STDMETHODCALLTYPE get_Completed(IAsyncOperationCompletedHandler<HSTRING>** handler) -> HRESULT override
	{
		return this->GetOnComplete(handler);
	}

	virtual auto STDMETHODCALLTYPE GetResults(HSTRING* results)  -> HRESULT override
	{
		HString str;
		str.Set(LR"(<?xml version="1.0"?><Receipt Version="1.0" CertificateId="A656B9B1B3AA509EEA30222E6D5E7DBDA9822DCD" ReceiptDate="2016-03-09T20:44:57.987Z" ReceiptDeviceId="6896129195083824" xmlns="http://schemas.microsoft.com/windows/2012/store/receipt"><AppReceipt Id="4f5e7e4e-efa0-4346-ac2b-12830f9141be" AppId="39C668CD.RiseoftheTombRaider_r7bfsmp40f67j" LicenseType="Purchase" PurchaseDate="2016-03-01T13:27:05.744Z" /><Signature xmlns="http://www.w3.org/2000/09/xmldsig#"><SignedInfo><CanonicalizationMethod Algorithm="http://www.w3.org/TR/2001/REC-xml-c14n-20010315" /><SignatureMethod Algorithm="http://www.w3.org/2001/04/xmldsig-more#rsa-sha256" /><Reference URI=""><Transforms><Transform Algorithm="http://www.w3.org/2000/09/xmldsig#enveloped-signature" /></Transforms><DigestMethod Algorithm="http://www.w3.org/2001/04/xmlenc#sha256" /><DigestValue>Uw7U4hcGns0j3YgybYxZb3bSGAuGBCFiFx0oHp/lii8=</DigestValue></Reference></SignedInfo><SignatureValue>jTdCBxtQg/mDoqXQD5b/k7XlBLg+ZYprGaYBf044GlB7Un/6Q5mHEFVKWie+8elhkCGslXTdBeoNCTPTOu6xiCQeR61Rk32M7v0CjbgxsRzS74N0rHchRvXcZvK+juRwxTJRYEnxzhzzCLd375PFYe5s/D6sVKs4uI8OuGkK9EHEHrnSEG1qNFwjOYJy8HQxn+4lLPHPJ/QyZT/htq6fwkf9RSnFHiJgFomwexBGOFGFwRSLtZiEdhDOWxrGERQxfZRhokT67wGuzGfUlgaGr0+KDWJr3k4nRrzAwEL1TrmKb5fUkcOxxnM0SHvVUWTwOXAqK0CiT+0EDud7wYBWrw==</SignatureValue></Signature></Receipt>)");
		str.CopyTo(results);

		return S_OK;
	}

protected:

	virtual auto OnStart()  -> HRESULT override { return S_OK; }
	virtual auto OnClose()  -> void    override {};
	virtual auto OnCancel() -> void    override {};
};

class CurrentApp : public ActivationFactory<Implements<ICurrentApp, ICurrentAppWithConsumables, ICurrentAppWithCampaignId>, ICurrentApp2Statics, ICurrentAppStaticsWithFiltering>
{
private:
	ComPtr<ICurrentAppSimulator> m_currentApp;
	ComPtr<ICurrentAppSimulatorWithConsumables> m_currentAppWithConsumables;
	ComPtr<ICurrentAppSimulatorWithCampaignId> m_currentAppWithCampaign;
	ComPtr<ICurrentAppSimulatorStaticsWithFiltering> m_filtering;

	ComPtr<ICurrentApp> m_realApp;

public:
	CurrentApp()
	{
		HString hString;
		hString.Set(L"Windows.ApplicationModel.Store.CurrentAppSimulator");

		GetActivationFactory(hString.Get(), &m_currentApp);

		hString.Set(L"Windows.ApplicationModel.Store.CurrentApp");

		ImHost_GetActivationFactory(hString.Get(), IID_ICurrentApp, (void**)m_realApp.GetAddressOf());

		m_currentApp.As(&m_currentAppWithCampaign);
		m_currentApp.As(&m_currentAppWithConsumables);
		m_currentApp.As(&m_filtering);
	}

public:
	STDMETHOD(get_LicenseInformation)(ABI::Windows::ApplicationModel::Store::ILicenseInformation **value)
	{
		return m_currentApp->get_LicenseInformation(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_LinkUri(
		/* [out][retval] */ __RPC__deref_out_opt ABI::Windows::Foundation::IUriRuntimeClass **value)
	{
		return m_currentApp->get_LinkUri(value);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AppId(
		/* [out][retval] */ __RPC__out GUID *value)
	{
		return m_currentApp->get_AppId(value);
	}

	virtual HRESULT STDMETHODCALLTYPE RequestAppPurchaseAsync(
		/* [in] */ boolean includeReceipt,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_HSTRING **requestAppPurchaseOperation)
	{
		return m_currentApp->RequestAppPurchaseAsync(includeReceipt, requestAppPurchaseOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE RequestProductPurchaseAsync(
		/* [in] */ __RPC__in HSTRING productId,
		/* [in] */ boolean includeReceipt,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_HSTRING **requestProductPurchaseOperation)
	{
		return m_currentApp->RequestProductPurchaseAsync(productId, includeReceipt, requestProductPurchaseOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE LoadListingInformationAsync(
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CApplicationModel__CStore__CListingInformation **loadListingOperation)
	{
		return m_currentApp->LoadListingInformationAsync(loadListingOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE GetAppReceiptAsync(
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_HSTRING **appReceiptOperation)
	{
		//ComPtr<AppReceiptOperation> operation = Make<AppReceiptOperation>();
		//return operation.CopyTo(appReceiptOperation);
		return m_currentApp->GetAppReceiptAsync(appReceiptOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE GetProductReceiptAsync(
		/* [in] */ __RPC__in HSTRING productId,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_HSTRING **getProductReceiptOperation)
	{
		return m_currentApp->GetProductReceiptAsync(productId, getProductReceiptOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE ReportConsumableFulfillmentAsync(
		/* [in] */ __RPC__in HSTRING productId,
		/* [in] */ GUID transactionId,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CApplicationModel__CStore__CFulfillmentResult **reportConsumableFulfillmentOperation)
	{
		return m_currentAppWithConsumables->ReportConsumableFulfillmentAsync(productId, transactionId, reportConsumableFulfillmentOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE RequestProductPurchaseWithResultsAsync(
		/* [in] */ __RPC__in HSTRING productId,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CApplicationModel__CStore__CPurchaseResults **requestProductPurchaseWithResultsOperation)
	{
		return m_currentAppWithConsumables->RequestProductPurchaseWithResultsAsync(productId, requestProductPurchaseWithResultsOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE RequestProductPurchaseWithDisplayPropertiesAsync(
		/* [in] */ __RPC__in HSTRING productId,
		/* [in] */ __RPC__in HSTRING offerId,
		/* [in] */ __RPC__in_opt ABI::Windows::ApplicationModel::Store::IProductPurchaseDisplayProperties *displayProperties,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CApplicationModel__CStore__CPurchaseResults **requestProductPurchaseWithDisplayPropertiesOperation)
	{
		return m_currentAppWithConsumables->RequestProductPurchaseWithDisplayPropertiesAsync(productId, offerId, displayProperties, requestProductPurchaseWithDisplayPropertiesOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE GetUnfulfilledConsumablesAsync(
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1___FIVectorView_1_Windows__CApplicationModel__CStore__CUnfulfilledConsumable **getUnfulfilledConsumablesOperation)
	{
		return m_currentAppWithConsumables->GetUnfulfilledConsumablesAsync(getUnfulfilledConsumablesOperation);
	}

	virtual HRESULT STDMETHODCALLTYPE GetAppPurchaseCampaignIdAsync(
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_HSTRING **operation)
	{
		return m_currentAppWithCampaign->GetAppPurchaseCampaignIdAsync(operation);
	}

	virtual HRESULT STDMETHODCALLTYPE LoadListingInformationByProductIdsAsync(
		/* [in] */ __RPC__in_opt __FIIterable_1_HSTRING *productIds,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CApplicationModel__CStore__CListingInformation **loadListingOperation)
	{
		//return m_filtering->LoadListingInformationByProductIdsAsync(productIds, loadListingOperation);
		return S_FALSE;
	}

	virtual HRESULT STDMETHODCALLTYPE LoadListingInformationByKeywordsAsync(
		/* [in] */ __RPC__in_opt __FIIterable_1_HSTRING *keywords,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CApplicationModel__CStore__CListingInformation **loadListingOperation)
	{
		//return m_filtering->LoadListingInformationByKeywordsAsync(keywords, loadListingOperation);
		return S_FALSE;
	}

	virtual HRESULT STDMETHODCALLTYPE ReportProductFulfillment(
		/* [in] */ __RPC__in HSTRING productId)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetCustomerPurchaseIdAsync(
		/* [in] */ __RPC__in HSTRING serviceTicket,
		/* [in] */ __RPC__in HSTRING publisherUserId,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_HSTRING **operation)
	{
		return 0x80071337;
	}

	virtual HRESULT STDMETHODCALLTYPE GetCustomerCollectionsIdAsync(
		/* [in] */ __RPC__in HSTRING serviceTicket,
		/* [in] */ __RPC__in HSTRING publisherUserId,
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_HSTRING **operation)
	{
		return 0x80071337;
	}
};

void RegisterStoreOverride()
{
	ImHost_AddActivationFactory(L"Windows.ApplicationModel.Store.CurrentApp", [] (HSTRING, IActivationFactory** factory)
	{
		auto app = Make<CurrentApp>();
		return app.CopyTo(factory);
	});
}