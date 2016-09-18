#include "StdInc.h"
#include "PackageIdentity.h"

#include <appmodel.h>

#include <fstream>
#include <sstream>

#include <pathcch.h>

#include <pugixml.hpp>

#pragma comment(lib, "pathcch.lib")

PackageIdentity::PackageIdentity(const std::wstring& manifestFile)
{
	memset(m_version, 0, sizeof(m_version));

	LoadFile(manifestFile);
	BuildNames();
}

void PackageIdentity::LoadFile(const std::wstring& fileName)
{
	{
		pugi::xml_document document;
		document.load_file(fileName.c_str());

		auto package = document.child(L"Package");
		auto identity = package.child(L"Identity");

		m_name = identity.attribute(L"Name").as_string();
		m_publisherName = identity.attribute(L"Publisher").as_string();

		std::wstring versionString = identity.attribute(L"Version").as_string();
		swscanf(versionString.c_str(), L"%d.%d.%d.%d", &m_version[0], &m_version[1], &m_version[2], &m_version[3]);

		m_displayName = package.child(L"Properties").child_value(L"DisplayName");

		for (auto& child : package.child(L"Capabilities").children())
		{
			m_capabilities.push_back(child.attribute(L"Name").as_string());
		}

		auto primaryApp = package.child(L"Applications").child(L"Application");
		m_primaryAppId = primaryApp.attribute(L"Id").value();
		m_applicationExecutable = primaryApp.attribute(L"Executable").value();
	}

	// calculate the root path
	{
		PWSTR fullPath;
		if (SUCCEEDED(PathAllocCanonicalize(fileName.c_str(), 0, &fullPath)))
		{
			std::wstring path = fullPath;
			m_rootPath = path.substr(0, path.find_last_of(L'\\'));

			LocalFree(fullPath);
		}
	}
}

void PackageIdentity::BuildNames()
{
	// calculate the publisher ID
	{
		PACKAGE_ID fakeId = { 0 };
		fakeId.name = const_cast<wchar_t*>(m_name.c_str());
		fakeId.publisher = const_cast<wchar_t*>(m_publisherName.c_str());

		uint32_t familyNameLength = 0;
		if (PackageFamilyNameFromId(&fakeId, &familyNameLength, nullptr) == ERROR_INSUFFICIENT_BUFFER)
		{
			std::vector<wchar_t> familyNameString(familyNameLength);

			if (PackageFamilyNameFromId(&fakeId, &familyNameLength, &familyNameString[0]) == ERROR_SUCCESS)
			{
				m_familyName = &familyNameString[0];
				m_publisherId = m_familyName.substr(m_familyName.find_last_of(L'_') + 1);

				{
					std::wstringstream fullNameStream;
					fullNameStream << m_name << L"_" << m_version[0] << L"." << m_version[1] << L"." << m_version[2] << L"." << m_version[3] << L"_x64__" << m_publisherId;

					m_fullName = fullNameStream.str();
				}
			}
		}
	}
}

static std::shared_ptr<PackageIdentity> g_currentPackage;

std::shared_ptr<PackageIdentity> GetCurrentPackageIdentity()
{
	return g_currentPackage;
}

void SetCurrentPackageIdentity(const std::shared_ptr<PackageIdentity>& packageIdentity)
{
	g_currentPackage = packageIdentity;
}