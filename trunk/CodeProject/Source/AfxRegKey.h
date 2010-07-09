#if !defined(AFX_REGKEY_H__E451D194_D4C3_11D1_AB17_0000E8425C3E__INCLUDED_)
#define AFX_REGKEY_H__E451D194_D4C3_11D1_AB17_0000E8425C3E__INCLUDED_

#include <winreg.h>
#include <afxtempl.h>

class CAfxRegKey
{

// Construction
public:
	CAfxRegKey();
	CAfxRegKey(CAfxRegKey& regKey);
	virtual ~CAfxRegKey();

	CAfxRegKey& operator=(CAfxRegKey regKey); 

// Operations
public:
	// note: calling open on an already open key will assert
	// you must call Close() first if you want to re-use CAfxRegKey objects
	LONG Open(HKEY hKeyRoot, const TCHAR* pszPath); 
	void Close();
	CString GetPath() const { return m_sPath; }
	HKEY GetKey() const { return m_hKey; }
	HKEY GetKeyRoot() const { return m_hKeyRoot; }
	
	LONG Write(const TCHAR* pszItem, DWORD dwVal);
	LONG Write(const TCHAR* pszItem, const TCHAR* pszVal);
	LONG Write(const TCHAR* pszItem, const BYTE* pData, DWORD dwLength);
	
	LONG Read(const TCHAR* pszItem, DWORD& dwVal) const;
	LONG Read(const TCHAR* pszItem, CString& sVal) const;
	LONG Read(const TCHAR* pszItem, BYTE* pData, DWORD& dwLength) const;
	
	static LONG Delete(HKEY hKeyRoot, const TCHAR* pszPath);
	static BOOL KeyExists(HKEY hKeyRoot, const TCHAR* pszPath);
	
	int GetSubkeyNames(CStringArray& aNames) const;
	int GetValueNames(CStringArray& aNames) const;
	BOOL HasValues() const;
	DWORD GetValueType(const TCHAR* pszItem) const;
	
	BOOL ExportToIni(LPCTSTR szIniPath) const;
	BOOL ImportFromIni(LPCTSTR szIniPath);
	
	static BOOL CopyKey(HKEY hkRootFrom, const CString& sFromPath, HKEY hkRootTo, const CString& sToPath);

	static CString GetAppRegPath(LPCTSTR szAppName = NULL);

protected:
	HKEY 	m_hKey;
	HKEY 	m_hKeyRoot;
	CString m_sPath;

protected:
	static LONG RecurseDeleteKey(HKEY key, LPCTSTR lpszKey);
   BOOL ExportKeyToIni(const TCHAR* pszKey, CStdioFile& file) const;
   BOOL ExportValueToIni(DWORD nIndex, CStdioFile& file) const;
   BOOL ImportSectionFromIni(const CString& sSection, CStdioFile& file, CString& sNextSection);
};

#endif
