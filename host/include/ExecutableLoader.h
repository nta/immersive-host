#pragma once

#include "UnicodeString.h"

class ExecutableLoader
{
public:
	ExecutableLoader(const std::wstring& exeRoot, const std::wstring& exeName);

	~ExecutableLoader();

	bool LoadExecutable();

	void Run();

private:
	bool MapSection();

	bool LoadDependentLibraries();

	bool ApplyRelocations();

	bool InstallExceptionHandler();

	void LoadTLS();

	void AddToPEB();

	HMODULE ResolveLibrary(const char* name);

	template<typename T>
	T* GetTargetRVA(ptrdiff_t offset)
	{
		return (T*)((char*)m_executableHandle + offset);
	}

private:
	HANDLE m_sectionHandle;

	PVOID m_executableHandle;

	std::wstring m_executableRoot;
	std::wstring m_executableName;
};