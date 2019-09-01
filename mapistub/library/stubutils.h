#pragma once
#include <Windows.h>
#include <functional>
#include <vector>

namespace file
{
	std::wstring GetSystemDirectory();
}

namespace import
{
	_Check_return_ HMODULE LoadFromSystemDir(_In_ const std::wstring& szDLLName);

	// Loads szModule at the handle given by hModule, then looks for szEntryPoint.
	// Will not load a module or entry point twice
	template <class T> void LoadProc(_In_ const std::wstring& szModule, HMODULE& hModule, LPCSTR szEntryPoint, T& lpfn)
	{
		if (!szEntryPoint) return;
		if (!hModule && !szModule.empty())
		{
			hModule = import::LoadFromSystemDir(szModule);
		}

		if (!hModule) return;

		lpfn = reinterpret_cast<T>(GetProcAddress(hModule, szEntryPoint));
		if (!lpfn)
		{
			output::logLoadLibrary(L"LoadProc: failed to load \"%ws\" from \"%ws\"\n", szEntryPoint, szModule.c_str());
		}
	}
} // namespace import

namespace output
{
	extern std::function<void(LPCWSTR szMsg, va_list argList)> logLoadMapiCallback;
	extern std::function<void(LPCWSTR szMsg, va_list argList)> logLoadLibraryCallback;

	void __cdecl logLoadMapi(LPCWSTR szMsg, ...);
	void __cdecl logLoadLibrary(LPCWSTR szMsg, ...);
	template <class T> void LogError(LPWSTR function, T error)
	{
		if (error) logLoadMapi(L"%ws failed with 0x%08X", function, error);
	}
}

namespace mapistub
{
	extern volatile ULONG g_ulDllSequenceNum;

	// Keep this in sync with g_pszOutlookQualifiedComponents
#define oqcOfficeBegin 0
#define oqcOffice16 (oqcOfficeBegin + 0)
#define oqcOffice15 (oqcOfficeBegin + 1)
#define oqcOffice14 (oqcOfficeBegin + 2)
#define oqcOffice12 (oqcOfficeBegin + 3)
#define oqcOffice11 (oqcOfficeBegin + 4)
#define oqcOffice11Debug (oqcOfficeBegin + 5)
#define oqcOfficeEnd oqcOffice11Debug

	HMODULE GetMAPIHandle();
	HMODULE GetPrivateMAPI();
	void UnloadPrivateMAPI();
	void ForceOutlookMAPI(bool fForce);
	void ForceSystemMAPI(bool fForce);
	void SetMAPIHandle(HMODULE hinstMAPI);
	std::wstring GetComponentPath(const std::wstring& szComponent, const std::wstring& szQualifier, bool fInstall);
	extern WCHAR g_pszOutlookQualifiedComponents[][MAX_PATH];
	std::vector<std::wstring> GetMAPIPaths();
	// Looks up Outlook's path given its qualified component guid
	std::wstring GetOutlookPath(_In_ const std::wstring& szCategory, _Out_opt_ bool* lpb64);
	std::wstring GetInstalledOutlookMAPI(int iOutlook);
	std::wstring GetMAPISystemDir();
} // namespace mapistub