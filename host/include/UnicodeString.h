#pragma once

template<typename Deleter>
class UnicodeString
{
public:
	inline UnicodeString()
	{
		m_internal.Buffer = nullptr;
		m_internal.Length = 0;
		m_internal.MaximumLength = 0;
	}

	inline ~UnicodeString()
	{
		if (m_internal.Buffer)
		{
			Deleter()(m_internal.Buffer);
			m_internal.Buffer = nullptr;
		}
	}

	inline operator PUNICODE_STRING()
	{
		return &m_internal;
	}

	inline PUNICODE_STRING operator&()
	{
		return &m_internal;
	}

	inline PUNICODE_STRING operator->()
	{
		return &m_internal;
	}

	inline PUNICODE_STRING Get()
	{
		return &m_internal;
	}

private:
	UNICODE_STRING m_internal;
};

struct RtlHeapDeleter
{
	inline void operator()(void* buffer)
	{
		RtlFreeHeap(GetProcessHeap(), 0, buffer);
	}
};

using HeapUnicodeString = UnicodeString<RtlHeapDeleter>;