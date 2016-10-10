#include "StdInc.h"
#include "ExecutableLoader.h"

// Microsoft.SpartaUWP seems to want a really large stack
#pragma comment(linker, "/stack:5000000,131072")

#include <jitasm.h>

FARPROC MakeLogWrapper(const char* fname, FARPROC function)
{
	return function;

	struct FE : public jitasm::Frontend
	{
		const char* name;
		FARPROC function;

		void InternalMain() override
		{
			push(rcx);
			push(rdx);
			push(r8);
			push(r9);

			sub(rsp, 0x28);

			mov(rcx, (uint64_t)name);
			mov(rax, (uint64_t)PrintName);

			call(rax);

			add(rsp, 0x28);

			pop(r9);
			pop(r8);
			pop(rdx);
			pop(rcx);

			mov(rax, (uint64_t)function);
			jmp(rax);
		}

		static void PrintName(const char* str)
		{
			if (strstr(str, "Heap") || strstr(str, "CriticalSection"))
			{
				return;
			}

			OutputDebugStringA((std::string(str) + "\n").c_str());
		}
	};

	FE* fe = new FE();
	fe->name = strdup(fname);
	fe->function = function;

	fe->Assemble();

	return (FARPROC)fe->GetCode();
}

ExecutableLoader::ExecutableLoader(const std::wstring& exeRoot, const std::wstring& exeName)
	: m_executableName(exeName), m_executableRoot(exeRoot), m_executableHandle(nullptr)
{

}

ExecutableLoader::~ExecutableLoader()
{

}

#include <psapi.h>

bool ExecutableLoader::LoadExecutable()
{
	// create a section containing the actual executable
	if (!MapSection())
	{
		return false;
	}

	if (!LoadDependentLibraries())
	{
		return false;
	}

	if (!ApplyRelocations())
	{
		return false;
	}

	if (!InstallExceptionHandler())
	{
		return false;
	}

	// store module handle
	NtCurrentPeb()->ImageBaseAddress = m_executableHandle;

	auto tlsProc = (void(*)(void(*)(void*), void*))GetProcAddress(GetModuleHandle(L"tlsdll.dll"), "SetTlsCallback");
	tlsProc([] (void* ctx)
	{
		reinterpret_cast<ExecutableLoader*>(ctx)->LoadTLS();
	}, this);

	LoadTLS();
	//AddToPEB();

	return true;
}
//
void ExecutableLoader::LoadTLS()
{
	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_executableHandle);

	IMAGE_NT_HEADERS* ntHeader = GetTargetRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);

	const IMAGE_TLS_DIRECTORY* sourceTls = GetTargetRVA<IMAGE_TLS_DIRECTORY>(ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

	if (sourceTls->AddressOfIndex)
	{
		*(DWORD*)(sourceTls->AddressOfIndex) = 0;

		void* tlses = (void*)__readgsqword(0x58);

		// note: 32-bit!
#if defined(_M_IX86)
		LPVOID tlsBase = *(LPVOID*)__readfsdword(0x2C);
#elif defined(_M_AMD64)
		LPVOID tlsBase = *(LPVOID*)__readgsqword(0x58);
#endif

		memcpy(tlsBase, reinterpret_cast<void*>(sourceTls->StartAddressOfRawData), sourceTls->EndAddressOfRawData - sourceTls->StartAddressOfRawData);
	}
	//memcpy((void*)source->StartAddressOfRawData, reinterpret_cast<void*>(sourceTls->StartAddressOfRawData), sourceTls->EndAddressOfRawData - sourceTls->StartAddressOfRawData);
}

void CoreRT_SetupSEHHandler(void* moduleBase, void* moduleEnd, PRUNTIME_FUNCTION runtimeFunctions, DWORD entryCount);

bool ExecutableLoader::InstallExceptionHandler()
{
	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_executableHandle);

	IMAGE_NT_HEADERS* ntHeader = GetTargetRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);

	IMAGE_DATA_DIRECTORY* exceptionDirectory = &ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];

	RUNTIME_FUNCTION* functionList = GetTargetRVA<RUNTIME_FUNCTION>(exceptionDirectory->VirtualAddress);
	DWORD entryCount = exceptionDirectory->Size / sizeof(RUNTIME_FUNCTION);

	/*if (!RtlAddFunctionTable(functionList, entryCount, (DWORD64)m_executableHandle))
	{
		return false;
	}*/

	CoreRT_SetupSEHHandler(m_executableHandle, (char*)m_executableHandle + ntHeader->OptionalHeader.SizeOfImage, functionList, entryCount);

	return true;
}

bool ExecutableLoader::ApplyRelocations()
{
#if 1
	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_executableHandle);

	IMAGE_NT_HEADERS* ntHeader = GetTargetRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);

	IMAGE_DATA_DIRECTORY* relocationDirectory = &ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

	IMAGE_BASE_RELOCATION* relocation = GetTargetRVA<IMAGE_BASE_RELOCATION>(relocationDirectory->VirtualAddress);
	IMAGE_BASE_RELOCATION* endRelocation = reinterpret_cast<IMAGE_BASE_RELOCATION*>((char*)relocation + relocationDirectory->Size);

	intptr_t relocOffset = reinterpret_cast<intptr_t>(m_executableHandle) - ntHeader->OptionalHeader.ImageBase;

	if (relocOffset == 0)
	{
		return true;
	}

	// loop
	while (true)
	{
		// are we past the size?
		if (relocation >= endRelocation)
		{
			break;
		}

		// is this an empty block?
		if (relocation->SizeOfBlock == 0)
		{
			break;
		}

		// go through each and every relocation
		size_t numRelocations = (relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t);
		uint16_t* relocStart = reinterpret_cast<uint16_t*>(relocation + 1);

		for (size_t i = 0; i < numRelocations; i++)
		{
			uint16_t type = relocStart[i] >> 12;
			uint32_t rva = (relocStart[i] & 0xFFF) + relocation->VirtualAddress;

			void* addr = GetTargetRVA<void>(rva);
			DWORD oldProtect;
			VirtualProtect(addr, 4, PAGE_EXECUTE_READWRITE, &oldProtect);

			if (type == IMAGE_REL_BASED_HIGHLOW)
			{
				*reinterpret_cast<int32_t*>(addr) += relocOffset;
			}
			else if (type == IMAGE_REL_BASED_DIR64)
			{
				*reinterpret_cast<int64_t*>(addr) += relocOffset;
			}
			else if (type != IMAGE_REL_BASED_ABSOLUTE)
			{
				return false;
			}

			VirtualProtect(addr, 4, oldProtect, &oldProtect);
		}

		// on to the next one!
		relocation = reinterpret_cast<IMAGE_BASE_RELOCATION*>((char*)relocation + relocation->SizeOfBlock);
	}

	return true;
#endif
}

HMODULE ExecutableLoader::ResolveLibrary(const char* name)
{
	// attempt loading the library directly
	wchar_t wideName[MAX_PATH];
	mbstowcs(wideName, name, _countof(wideName));

	UNICODE_STRING string;
	RtlInitUnicodeString(&string, wideName);

	HANDLE hModule;
	NTSTATUS status = LdrLoadDll(nullptr, nullptr, &string, &hModule);

	if (status == STATUS_NOT_APPCONTAINER)
	{
		// NOTE: this may break a *lot* of stuff
		NtCurrentPeb()->Diff0.W81.IsAppContainer = 1;

		status = LdrLoadDll(nullptr, nullptr, &string, &hModule);

		NtCurrentPeb()->Diff0.W81.IsAppContainer = 0;
	}

	if (!NT_SUCCESS(status))
	{
		SetLastError(RtlNtStatusToDosError(status));

		return nullptr;
	}

	return reinterpret_cast<HMODULE>(hModule);
}

bool ExecutableLoader::LoadDependentLibraries()
{
	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_executableHandle);

	IMAGE_NT_HEADERS* ntHeader = GetTargetRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);

	IMAGE_DATA_DIRECTORY* importDirectory = &ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	auto descriptor = GetTargetRVA<IMAGE_IMPORT_DESCRIPTOR>(importDirectory->VirtualAddress);

	while (descriptor->Name)
	{
		const char* name = GetTargetRVA<char>(descriptor->Name);

		HMODULE module = ResolveLibrary(name);

		if (!module)
		{
			//FatalError("Could not load dependent module %s. Error code was %i.\n", name, GetLastError());
			return false;
		}

		// "don't load"
		if (reinterpret_cast<uint32_t>(module) == 0xFFFFFFFF)
		{
			descriptor++;
			continue;
		}

		auto nameTableEntry = GetTargetRVA<uintptr_t>(descriptor->OriginalFirstThunk);
		auto addressTableEntry = GetTargetRVA<uintptr_t>(descriptor->FirstThunk);

		// some applications use FirstThunk for original name addresses
		if (!descriptor->OriginalFirstThunk)
		{
			nameTableEntry = GetTargetRVA<uintptr_t>(descriptor->FirstThunk);
		}

		while (*nameTableEntry)
		{
			FARPROC function;
			const char* functionName;
			char nameBuffer[64];

			// is this an ordinal-only import?
			if (IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
			{
				function = GetProcAddress(module, MAKEINTRESOURCEA(IMAGE_ORDINAL(*nameTableEntry)));
				sprintf(nameBuffer, "#%d", IMAGE_ORDINAL(*nameTableEntry));
				functionName = nameBuffer;
			}
			else
			{
				auto import = GetTargetRVA<IMAGE_IMPORT_BY_NAME>(*nameTableEntry);

				function = GetProcAddress(module, import->Name);//(FARPROC)m_functionResolver(module, import->Name);
				functionName = import->Name;
			}

			if (!function)
			{
				char pathName[MAX_PATH];
				GetModuleFileNameA(module, pathName, sizeof(pathName));

				//FatalError("Could not load function %s in dependent module %s (%s).\n", functionName, name, pathName);
				return false;
			}

			DWORD oldProtect;
			VirtualProtect(addressTableEntry, sizeof(void*), PAGE_READWRITE, &oldProtect);

			function = MakeLogWrapper(functionName, function);

			*addressTableEntry = (uintptr_t)function;

			VirtualProtect(addressTableEntry, sizeof(void*), oldProtect, &oldProtect);

			nameTableEntry++;
			addressTableEntry++;
		}

		descriptor++;
	}

	return true;
}

typedef struct _LDR_MODULE
{
	LIST_ENTRY          InLoadOrderModuleList;
	LIST_ENTRY          InMemoryOrderModuleList;
	LIST_ENTRY          InInitializationOrderModuleList;
	void*               BaseAddress;
	void*               EntryPoint;
	ULONG               SizeOfImage;
	UNICODE_STRING      FullDllName;
	UNICODE_STRING      BaseDllName;
	ULONG               Flags;
	SHORT               LoadCount;
	SHORT               TlsIndex;
	HANDLE              SectionHandle;
	ULONG               CheckSum;
	ULONG               TimeDateStamp;
	HANDLE              ActivationContext;
} LDR_MODULE, *PLDR_MODULE;

#define LDR_IMAGE_IS_DLL                0x00000004
#define LDR_LOAD_IN_PROGRESS            0x00001000
#define LDR_UNLOAD_IN_PROGRESS          0x00002000
#define LDR_NO_DLL_CALLS                0x00040000
#define LDR_PROCESS_ATTACHED            0x00080000
#define LDR_MODULE_REBASED              0x00200000

#define AppendToChain(module, list, chain, offset) { \
	(module)->##chain##.Flink = &(list)->##chain##; \
	(module)->##chain##.Blink = (list)->##chain##.Blink; \
	((PLDR_MODULE)(((char *)(list)->##chain##.Blink) - offset))->##chain##.Flink = &(module)->##chain##; \
	(list)->##chain##.Blink = &(module)->##chain##; \
};

void ExecutableLoader::AddToPEB()
{
	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_executableHandle);
	IMAGE_NT_HEADERS* ntHeader = GetTargetRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);

	PLDR_MODULE loaderModule;
	PPEB_LDR_DATA loaderData = NtCurrentPeb()->Ldr;
	DWORD entry = ntHeader->OptionalHeader.AddressOfEntryPoint;

	loaderModule = (PLDR_MODULE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LDR_MODULE));

	if (!loaderModule)
	{
		return;
	}

	loaderModule->BaseAddress = m_executableHandle;
	loaderModule->EntryPoint = (entry) ? GetTargetRVA<void>(entry) : nullptr;
	loaderModule->SizeOfImage = ntHeader->OptionalHeader.SizeOfImage;
	loaderModule->LoadCount = 1;

	PWSTR buffer = (PWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (m_executableName.length() + 1) * 2);
	wcscpy(buffer, m_executableName.c_str());

	RtlInitUnicodeString(&loaderModule->BaseDllName, buffer);

	std::wstring fullName = m_executableRoot + m_executableName;

	buffer = (PWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (fullName.length() + 1) * 2);
	wcscpy(buffer, fullName.c_str());

	RtlInitUnicodeString(&loaderModule->FullDllName, buffer);

	loaderModule->Flags = LDR_PROCESS_ATTACHED;
	loaderModule->TimeDateStamp = ntHeader->FileHeader.TimeDateStamp;

	AppendToChain(loaderModule, loaderData, InLoadOrderModuleList, 0);
	if (loaderModule->EntryPoint == 0)
		loaderModule->InInitializationOrderModuleList.Blink = loaderModule->InInitializationOrderModuleList.Flink = 0;
	else
		AppendToChain(loaderModule, loaderData, InInitializationOrderModuleList, sizeof(LIST_ENTRY) * 2);

	AppendToChain(loaderModule, loaderData, InMemoryOrderModuleList, sizeof(LIST_ENTRY));
}

bool ExecutableLoader::MapSection()
{
	// open a handle to the file
	HeapUnicodeString ntPathName;
	RtlDosPathNameToNtPathName_U((m_executableRoot + m_executableName).c_str(), &ntPathName, nullptr, nullptr);

	OBJECT_ATTRIBUTES fileAttributes;
	InitializeObjectAttributes(&fileAttributes, ntPathName, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

	IO_STATUS_BLOCK statusBlock;

	// actually open
	HANDLE hFile;
	NTSTATUS status = NtOpenFile(&hFile, SYNCHRONIZE | FILE_EXECUTE | FILE_READ_DATA, &fileAttributes, &statusBlock, FILE_SHARE_READ | FILE_SHARE_DELETE, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);

	if (!NT_SUCCESS(status))
	{
		return false;
	}

	// create a section on the file
	status = NtCreateSection(&m_sectionHandle, SECTION_ALL_ACCESS, nullptr, nullptr, PAGE_EXECUTE, SEC_IMAGE, hFile);

	// couldn't load the image?
	if (!NT_SUCCESS(status))
	{
		ULONG_PTR parameters[] = { (ULONG_PTR)&ntPathName };
		HardErrorResponse response;

		NtRaiseHardError(STATUS_INVALID_IMAGE_FORMAT, _countof(parameters), 1, parameters, 1, &response);

		NtClose(hFile);

		return false;
	}

	// map the image into memory
	PVOID viewBase = nullptr;
	SIZE_T viewSize = 0;

	status = NtMapViewOfSection(m_sectionHandle, NtCurrentProcess(), &viewBase, 0, 0, nullptr, &viewSize, ViewShare, 0, PAGE_READWRITE);

	if (!NT_SUCCESS(status))
	{
		NtClose(m_sectionHandle);
		NtClose(hFile);

		return false;
	}

	m_executableHandle = viewBase;

	return true;
}

class UserLibrary
{
private:
	std::vector<uint8_t> m_libraryBuffer;

public:
	UserLibrary(const wchar_t* fileName);

	const uint8_t* GetExportCode(const char* name) const;

	const uint8_t* GetOffsetPointer(uint32_t offset) const;
};

UserLibrary::UserLibrary(const wchar_t* fileName)
{
	FILE* f = _wfopen(fileName, L"rb");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		m_libraryBuffer.resize(ftell(f));

		fseek(f, 0, SEEK_SET);
		fread(&m_libraryBuffer[0], 1, m_libraryBuffer.size(), f);

		fclose(f);
	}
}

const uint8_t* UserLibrary::GetExportCode(const char* getName) const
{
	// get the DOS header
	IMAGE_DOS_HEADER* header = (IMAGE_DOS_HEADER*)&m_libraryBuffer[0];

	if (header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return nullptr;
	}

	// get the NT header
	const IMAGE_NT_HEADERS* ntHeader = (const IMAGE_NT_HEADERS*)&m_libraryBuffer[header->e_lfanew];

	// find the export directory
	auto exportDirectoryData = ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	// get the export directory
	const IMAGE_EXPORT_DIRECTORY* exportDirectory = (const IMAGE_EXPORT_DIRECTORY*)GetOffsetPointer(exportDirectoryData.VirtualAddress);

	const uint32_t* names = (const uint32_t*)GetOffsetPointer(exportDirectory->AddressOfNames);
	const uint16_t* ordinals = (const uint16_t*)GetOffsetPointer(exportDirectory->AddressOfNameOrdinals);
	const uint32_t* functions = (const uint32_t*)GetOffsetPointer(exportDirectory->AddressOfFunctions);

	for (int i = 0; i < exportDirectory->NumberOfNames; i++)
	{
		const char* name = (const char*)GetOffsetPointer(names[i]);

		if (_stricmp(name, getName) == 0)
		{
			return GetOffsetPointer(functions[ordinals[i]]);
		}
	}

	return nullptr;
}

const uint8_t* UserLibrary::GetOffsetPointer(uint32_t offset) const
{
	// get the DOS header
	const IMAGE_DOS_HEADER* header = (const IMAGE_DOS_HEADER*)&m_libraryBuffer[0];

	// get the NT header
	const IMAGE_NT_HEADERS* ntHeader = (const IMAGE_NT_HEADERS*)&m_libraryBuffer[header->e_lfanew];

	// loop through each sections to find where our offset is
	const IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeader);

	for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
	{
		uint32_t curRaw = sections[i].PointerToRawData;
		uint32_t curVirt = sections[i].VirtualAddress;

		if (offset >= curVirt && offset < curVirt + sections[i].SizeOfRawData)
		{
			offset -= curVirt;
			offset += curRaw;

			return &m_libraryBuffer[offset];
		}
	}

	return nullptr;
}

static void* origCloseHandle;

typedef struct _OBJECT_HANDLE_ATTRIBUTE_INFORMATION
{
	BOOLEAN Inherit;
	BOOLEAN ProtectFromClose;
} OBJECT_HANDLE_ATTRIBUTE_INFORMATION, *POBJECT_HANDLE_ATTRIBUTE_INFORMATION;

#pragma comment(lib, "ntdll.lib")

struct NtCloseHook : public jitasm::Frontend
{
	NtCloseHook()
	{

	}

	static NTSTATUS ValidateHandle(HANDLE handle)
	{
		char info[16];

		if (NtQueryObject(handle, (OBJECT_INFORMATION_CLASS)4, &info, sizeof(OBJECT_HANDLE_ATTRIBUTE_INFORMATION), nullptr) >= 0)
		{
			return 0;
		}
		else
		{
			return STATUS_INVALID_HANDLE;
		}
	}

	void InternalMain()
	{
		push(rcx);

		mov(rax, (uint64_t)&ValidateHandle);
		call(rax);

		pop(rcx);

		cmp(eax, STATUS_INVALID_HANDLE);
		je("doReturn");

		mov(rax, (uint64_t)origCloseHandle);
		push(rax); // to return here, as there seems to be no jump-to-rax in jitasm

		L("doReturn");
		ret();
	}
};

class NtdllHooks
{
private:
	UserLibrary m_ntdll;

private:
	void HookHandleClose();

	void HookQueryInformationProcess();

public:
	NtdllHooks(const wchar_t* ntdllPath);

	void Install();
};

NtdllHooks::NtdllHooks(const wchar_t* ntdllPath)
	: m_ntdll(ntdllPath)
{
}

void NtdllHooks::Install()
{
	//HookHandleClose();
	//HookQueryInformationProcess();
}

void NtdllHooks::HookHandleClose()
{
	// hook NtClose (STATUS_INVALID_HANDLE debugger detection)
	uint8_t* code = (uint8_t*)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtClose");

	origCloseHandle = VirtualAlloc(nullptr, 1024, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(origCloseHandle, m_ntdll.GetExportCode("NtClose"), 1024);

	NtCloseHook* hook = new NtCloseHook;
	hook->Assemble();

	DWORD oldProtect;
	VirtualProtect(code, 15, PAGE_EXECUTE_READWRITE, &oldProtect);

	*(uint8_t*)code = 0x48;
	*(uint8_t*)(code + 1) = 0xb8;

	*(uint64_t*)(code + 2) = (uint64_t)hook->GetCode();

	*(uint16_t*)(code + 10) = 0xE0FF;
}

static void* origQIP;
static DWORD explorerPid;

#include <ntstatus.h>

typedef NTSTATUS(*NtQueryInformationProcessType)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL);

static NTSTATUS NtQueryInformationProcessHook(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL)
{
	NTSTATUS status = ((NtQueryInformationProcessType)origQIP)(ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength);

	if (NT_SUCCESS(status))
	{
		if (ProcessInformationClass == ProcessBasicInformation)
		{
			((PPROCESS_BASIC_INFORMATION)ProcessInformation)->InheritedFromUniqueProcessId = (DWORD64)explorerPid;
		}
		else if (ProcessInformationClass == 30) // ProcessDebugObjectHandle
		{
			*(HANDLE*)ProcessInformation = 0;

			return STATUS_PORT_NOT_SET;
		}
		else if (ProcessInformationClass == 7) // ProcessDebugPort
		{
			*(HANDLE*)ProcessInformation = 0;
		}
		else if (ProcessInformationClass == 31)
		{
			*(ULONG*)ProcessInformation = 1;
		}
	}

	return status;
}

void NtdllHooks::HookQueryInformationProcess()
{
	uint8_t* code = (uint8_t*)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtQueryInformationProcess");

	HWND shellWindow = GetShellWindow();
	GetWindowThreadProcessId(shellWindow, &explorerPid);

	origQIP = VirtualAlloc(nullptr, 1024, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(origQIP, m_ntdll.GetExportCode("NtQueryInformationProcess"), 1024);

	/*NtQueryInformationProcessHook* hook = new NtQueryInformationProcessHook;
	hook->Assemble();*/

	DWORD oldProtect;
	VirtualProtect(code, 15, PAGE_EXECUTE_READWRITE, &oldProtect);

	*(uint8_t*)code = 0x48;
	*(uint8_t*)(code + 1) = 0xb8;

	*(uint64_t*)(code + 2) = (uint64_t)NtQueryInformationProcessHook;

	*(uint16_t*)(code + 10) = 0xE0FF;
}

void ExecutableLoader::Run()
{
	// set BeingDebugged
	PPEB peb = (PPEB)__readgsqword(0x60);
	peb->BeingDebugged = false;

	// set GlobalFlags
	*(DWORD*)((char*)peb + 0xBC) &= ~0x70;

	{
		// user library stuff ('safe' ntdll hooking callbacks)
		wchar_t ntdllPath[MAX_PATH];
		GetModuleFileName(GetModuleHandle(L"ntdll.dll"), ntdllPath, _countof(ntdllPath));

		NtdllHooks hooks(ntdllPath);
		hooks.Install();
	}

	//if (CoreIsDebuggerPresent())
	{
		// NOP OutputDebugStringA; the debugger doesn't like multiple async exceptions
		uint8_t* func = (uint8_t*)OutputDebugStringA;

		DWORD oldProtect;
		VirtualProtect(func, 1, PAGE_EXECUTE_READWRITE, &oldProtect);

		//*func = 0xC3;

		VirtualProtect(func, 1, oldProtect, &oldProtect);
	}

	IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_executableHandle);

	IMAGE_NT_HEADERS* ntHeader = GetTargetRVA<IMAGE_NT_HEADERS>(dosHeader->e_lfanew);

	(GetTargetRVA<void()>(ntHeader->OptionalHeader.AddressOfEntryPoint))();
}