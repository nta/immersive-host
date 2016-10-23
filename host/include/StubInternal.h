#pragma once

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;

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