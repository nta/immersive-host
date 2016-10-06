#include "StdInc.h"

#define BUFFER_COUNT 8
#define BUFFER_LENGTH 32768

const char* va(const char* string, ...)
{
	static thread_local int currentBuffer;
	static thread_local std::vector<char> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_COUNT * BUFFER_LENGTH);
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vsnprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}

const wchar_t* va(const wchar_t* string, ...)
{
	static thread_local int currentBuffer;
	static thread_local std::vector<wchar_t> buffer;

	if (!buffer.size())
	{
		buffer.resize(BUFFER_COUNT * BUFFER_LENGTH);
	}

	int thisBuffer = currentBuffer;

	va_list ap;
	va_start(ap, string);
	int length = vswprintf(&buffer[thisBuffer * BUFFER_LENGTH], BUFFER_LENGTH, string, ap);
	va_end(ap);

	if (length >= BUFFER_LENGTH)
	{
		//GlobalError("Attempted to overrun string in call to va()!");
		exit(1);
	}

	buffer[(thisBuffer * BUFFER_LENGTH) + BUFFER_LENGTH - 1] = '\0';

	currentBuffer = (currentBuffer + 1) % BUFFER_COUNT;

	return &buffer[thisBuffer * BUFFER_LENGTH];
}
