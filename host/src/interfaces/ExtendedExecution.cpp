#include "StdInc.h"

#include <Windows.ApplicationModel.ExtendedExecution.h>

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::ApplicationModel::ExtendedExecution;

class ExecutionResultOperation : public RuntimeClass<AsyncBase<IAsyncOperationCompletedHandler<ExtendedExecutionResult>>, IAsyncOperation<ExtendedExecutionResult>>
{
public:
	ExecutionResultOperation()
	{
		Start();

		FireCompletion();
	}

	virtual auto STDMETHODCALLTYPE put_Completed(IAsyncOperationCompletedHandler<ExtendedExecutionResult>* handler) -> HRESULT override
	{
		return this->PutOnComplete(handler);
	}

	virtual auto STDMETHODCALLTYPE get_Completed(IAsyncOperationCompletedHandler<ExtendedExecutionResult>** handler) -> HRESULT override
	{
		return this->GetOnComplete(handler);
	}

	virtual auto STDMETHODCALLTYPE GetResults(ExtendedExecutionResult* results)  -> HRESULT override
	{
		*results = ExtendedExecutionResult_Allowed;

		return S_OK;
	}

protected:

	virtual auto OnStart()  -> HRESULT override { return S_OK; }
	virtual auto OnClose()  -> void    override {};
	virtual auto OnCancel() -> void    override {};
};

class ExecutionSession : public ActivationFactory<IExtendedExecutionSession>
{
public:
	virtual HRESULT STDMETHODCALLTYPE ActivateInstance(_Outptr_result_nullonfailure_ IInspectable **ppvObject)
	{
		return QueryInterface(IID_IInspectable, (void**)ppvObject);
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Reason(
		/* [out][retval] */ __RPC__out ABI::Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionReason *value)
	{
		*value = ExtendedExecutionReason_Unspecified;

		return S_OK;
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Reason(
		/* [in] */ ABI::Windows::ApplicationModel::ExtendedExecution::ExtendedExecutionReason value)
	{
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Description(
		/* [out][retval] */ __RPC__deref_out_opt HSTRING *value)
	{
		HString hString;
		hString.Set(L"just 'cause?");

		*value = hString.Get();

		return S_OK;
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Description(
		/* [in] */ __RPC__in HSTRING value)
	{
		return S_OK;
	}

	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PercentProgress(
		/* [out][retval] */ __RPC__out UINT32 *value)
	{
		*value = 42;

		return S_OK;
	}

	virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PercentProgress(
		/* [in] */ UINT32 value)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE add_Revoked(
		/* [in] */ __RPC__in_opt __FITypedEventHandler_2_IInspectable_Windows__CApplicationModel__CExtendedExecution__CExtendedExecutionRevokedEventArgs *handler,
		/* [out][retval] */ __RPC__out EventRegistrationToken *token)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE remove_Revoked(
		/* [in] */ EventRegistrationToken token)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE RequestExtensionAsync(
		/* [out][retval] */ __RPC__deref_out_opt __FIAsyncOperation_1_Windows__CApplicationModel__CExtendedExecution__CExtendedExecutionResult **operation)
	{
		ComPtr<ExecutionResultOperation> thisOperation = Make<ExecutionResultOperation>();
		return thisOperation.CopyTo(operation);
	}
};

void RegisterExtendedExecution()
{
	ImHost_AddActivationFactory(L"Windows.ApplicationModel.ExtendedExecution.ExtendedExecutionSession", [] (HSTRING, IActivationFactory** factory)
	{
		auto app = Make<ExecutionSession>();
		return app.CopyTo(factory);
	});
}