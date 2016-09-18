#pragma once

class PackageIdentity
{
public:
	PackageIdentity(const std::wstring& manifestFile);

	inline const std::wstring& GetName() const
	{
		return m_name;
	}

	inline const std::wstring& GetFullName() const
	{
		return m_fullName;
	}

	inline const std::wstring& GetFamilyName() const
	{
		return m_familyName;
	}
	
	inline const std::vector<std::wstring>& GetCapabilities() const
	{
		return m_capabilities;
	}

	inline const std::wstring& GetPrimaryAppId() const
	{
		return m_primaryAppId;
	}

	inline const std::wstring& GetPublisher() const
	{
		return m_publisherName;
	}

	inline const std::wstring& GetPublisherId() const
	{
		return m_publisherId;
	}

	inline const std::wstring& GetApplicationRoot() const
	{
		return m_rootPath;
	}

	inline const std::wstring& GetApplicationFileName() const
	{
		return m_applicationExecutable;
	}

	inline const std::wstring& GetDisplayName() const
	{
		return m_displayName;
	}

	inline int GetVersionPart(int part) const
	{
		if (part < 0 || part >= 4)
		{
			return 0;
		}

		return m_version[part];
	}

private:
	void LoadFile(const std::wstring& fileName);

	void BuildNames();

private:
	std::wstring m_name;
	std::wstring m_publisherName;
	std::wstring m_publisherId;
	int m_version[4];

	std::wstring m_fullName;
	std::wstring m_familyName;

	std::wstring m_displayName;
	std::vector<std::wstring> m_capabilities;

	std::wstring m_primaryAppId;

	std::wstring m_rootPath;
	std::wstring m_applicationExecutable;
};

std::shared_ptr<PackageIdentity> GetCurrentPackageIdentity();
void SetCurrentPackageIdentity(const std::shared_ptr<PackageIdentity>& packageIdentity);