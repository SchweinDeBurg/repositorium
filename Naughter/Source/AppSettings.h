/*
Module : AppSettings.H
Purpose: Implementation for a collection of MFC classes which provides for reading and writing application settings
         to a number of locations and formats:
         
         The supported output formats are:
         
         1. Per user in the Registry (HKEY_CURRENT_USER\Software\"Company Name"\"Product Name"\"Product Version") via the class CHKCUAppSettings
         2. Per machine in the Registry (HKEY_LOCAL_MACHINE\Software\"Company Name"\"Product Name"\"Product Version") via the class CHKLMAppSettings
         3. Service parameters in the registry (HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\"Service Name"\Parameters) via the class CServicesAppSettings
         4. A standard windows ini file via the class CIniAppSettings
         5. An XML file via the class CXMLAppSettings


         The ini and xml file locations can be:

         1. A per user roaming file stored in "\Documents and Settings\<User Name>\Application Data\"Company Name"\"Product Name"\"Product Version" via the method
         CIniAppSettings::GetRoamingDataFileLocation. Note that this method will only work if IE4 integrated shell is installed on Windows 95. Otherwise out of the box 
         Windows 98 or later.
         2. A per user non-roaming file stored in "\Documents and Settings\<User Name>\Local Settings\Application Data\"Company Name"\"Product Name"\"Product Version"
         via the method CIniAppSettings::GetNonRoamingDataFileLocation. Note that this method will only work on Windows 2000 or later.
         3. A per computer (non-user specific and non-roaming) file stored in "\Documents and Settings\All Users\Application Data\"Company Name"\"Product Name"\"Product Version"
         via the method CIniAppSettings::GetPerComputerDataFileLocation. Note that this method will only work on Windows 2000 or later.
         4. A file in the same directory as the current process via the method GetCurrentProcessIniFileLocation. Note, that using this location for an ini or xml file is not 
         a good idea for security reasons

         Other features

         1. The registry and xml output format can also be optionally flushed upon a write or left to be lazy written 

Created: PJN / 07-06-2006
History: PJN / 12-06-2006 1. Minor update to a comment
         PJN / 29-07-2006 1. All AppSettings classes now support "GetSection" and "WriteSection" methods. These new functions are modelled on the "GetPrivateProfileSection" and 
                          "WritePrivateProfileSection" Win32 API calls. Thanks to Brad Bruce for suggesting these updates.
                          2. Made the AfxThrowWin32AppSettingsException function a member of IAppSettings and renamed it to ThrowWin32AppSettingsException
                          3. Made the AfxThrowCOMAppSettingsException function a member of IAppSettings and renamed it to ThrowCOMAppSettingsException
                          4. Code now uses newer C++ style casts instead of C style casts.
                          5. Made all the WriteBinary methods now accept a const BYTE* parameter instead of LPBYTE
                          6. Updated the code to clean compile on VC 2005
                          7. Fixed a bug in CXMLAppSettings::GetBinary where returned binary data was being returned incorrectly.
         PJN / 19-08-2006 1. Fixed compile problems when code is compiled in a mixed MFC / ATL project.
                          2. Addition of "WriteProfile..." functions to IAppSettings which provides similiar functionality to the MFC CWinApp functions of the same
                          name. These new functions allow you to ignore errors reading a value and instead return a default value.
                          3. Removed the MFC C++ runtime macros from CAppSettingsException as they were not really required.
         PJN / 02-01-2007 1. Updated copyright details.
                          2. Fixed a bug in CRegistryAppSettings::SetStringArrayIntoRegistry related to use of _tcscpy_s
                          3. Fixed a bug in CIniAppSettings::WriteStringArray related to use of _tcscpy_s
                          4. Fixed a bug in CXMLAppSettings::WriteStringArray related to use of _tcscpy_s
         PJN / 05-07-2007 1. Added support for "Pretty Printing" to the CXMLAppSettings class. This is achieved through an additional parameter to the constructor
         PJN / 24-12-2007 1. CAppSettingsException::GetErrorMessage now uses the FORMAT_MESSAGE_IGNORE_INSERTS flag. For more information please see Raymond
                          Chen's blog at http://blogs.msdn.com/oldnewthing/archive/2007/11/28/6564257.aspx. Thanks to Alexey Kuznetsov for reporting this
                          issue.
                          2. Minor update to display HRESULT's correctly.
         PJN / 28-12-2007 1. CAppSettingsException::GetErrorMessage now uses Checked::tcsncpy_s if compiled using VC 2005 or later.
         PJN / 06-06-2008 1. Updated copyright details.
                          2. Updated sample app to clean compile on VC 2008
                          3. The code has now been updated to support VC 2005 or later only. 
                          4. Code now compiles cleanly using Code Analysis (/analyze) 
         PJN / 19-03-2009 1. Updated copyright details.
                          2. Updated the sample app solution settings to use more reasonable defaults
                          3. The IAppSettings interface now supports a new GetSections method. This returns all the root sections which a configuration file has.
                          Thanks to Rolf Kristensen for prompting this nice addition to the classes.
                          4. Updated the IAppSettings::GetSection method to support a boolean "bWithValues" parameter which determines if the returned string array 
                          should or should name include the values. This allows client code to easily get all the sections names. Thanks to Rolf Kristensen for 
                          providing this nice addition to the classes.
                          5. Reworked most of the internal code which does heap allocations using "new" to instead now use ATL::CHeapPtr.

Copyright (c) 2006 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////////////// Macros / Defines //////////////////////

#pragma once

#ifndef __APPSETTINGS_H__
#define __APPSETTINGS_H__


/////////////////////////////////////// Includes //////////////////////////////

#ifndef _SHLOBJ_H_
#pragma message("To avoid this message, please put shlobj.h in your pre compiled header (normally stdafx.h)")
#include <shlobj.h>
#endif
#ifndef __ATLBASE_H__
#pragma message("To avoid this message, please put atlbase.h in your pre compiled header (normally stdafx.h)")
#include <atlbase.h>
#endif
#include <msxml.h>


/////////////////////////////////////// Classes ///////////////////////////////

//The exception class which an AppSettings class can throw
class CAppSettingsException : public CException
{
public:
//Constructors / Destructors
  CAppSettingsException(HRESULT hr) : m_hr(hr), 
                                      m_bNativeWin32Error(FALSE)
  {
  }

  CAppSettingsException(DWORD dwError, DWORD dwFacility) : m_hr(MAKE_HRESULT(SEVERITY_ERROR, dwFacility, dwError)),
                                                           m_bNativeWin32Error(TRUE)
  {
  }

//Methods
#ifdef _DEBUG
  void Dump(CDumpContext& dc) const
  {
    //Let the base class do its thing
	  CObject::Dump(dc);

	  dc << _T("m_hr = ") << m_hr << _T(", m_bNativeWin32Error = ") << m_bNativeWin32Error << _T("\n");
  }
#endif

  virtual BOOL GetErrorMessage(__out_ecount_z(nMaxError) LPTSTR pstrError, __in UINT nMaxError, __out_opt PUINT pnHelpContext = NULL)
  {
    //Validate our parameters
	  ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	  if (pnHelpContext != NULL)
		  *pnHelpContext = 0;

    //What will be the return value from this function (Assume the worst)
    BOOL bSuccess = FALSE;

    if (m_bNativeWin32Error)
    {
      //Lookup the error using FormatMessage
  	  LPTSTR lpBuffer;
	    DWORD dwReturn = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			                               NULL, HRESULT_CODE(m_hr), MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			                               reinterpret_cast<LPTSTR>(&lpBuffer), 0, NULL);

	    if (dwReturn == 0)
		    *pstrError = _T('\0');
	    else
	    {
		    bSuccess = TRUE;
		    Checked::tcsncpy_s(pstrError, nMaxError, lpBuffer, _TRUNCATE);
		    LocalFree(lpBuffer);
	    }
    }
    else
    {
      ATL::CComPtr<IErrorInfo> pError;
      if (SUCCEEDED(GetErrorInfo(0, &pError)) && pError)
      {
        bSuccess = TRUE;
        CComBSTR bstrDescription;
        pError->GetDescription(&bstrDescription);

		    Checked::tcsncpy_s(pstrError, nMaxError, COLE2T(bstrDescription), _TRUNCATE);
      }
      else
        *pstrError = _T('\0');
    }

	  return bSuccess;
  }

  CString GetErrorMessage()
  {
    CString rVal;
    LPTSTR pstrError = rVal.GetBuffer(4096);
    GetErrorMessage(pstrError, 4096, NULL);
    rVal.ReleaseBuffer();
    return rVal;
  }

//Data members
	HRESULT m_hr;
  BOOL    m_bNativeWin32Error;
};

//The "interface" which an AppSettings class must implement
class IAppSettings
{
public:
//Virtual methods
  virtual int     GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry) = 0;
  virtual CString GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry) = 0;
  virtual DWORD   GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData) = 0;
  virtual void    GetStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, CStringArray& array) = 0;
  
//Versions of the AppSettings functions which mimic as closely as possible the functions of the same name in CWinApp 
//or the authors CNTService class framework
  virtual int GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
  {
    try
    {
      return GetInt(lpszSection, lpszEntry);
    }
    catch(CAppSettingsException* pEx)
    {
      pEx->Delete();
      
      return nDefault;      
    }
  }
  
  virtual CString GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL)
  {
    try
    {
      return GetString(lpszSection, lpszEntry);
    }
    catch(CAppSettingsException* pEx)
    {
      pEx->Delete();
      
      return lpszDefault;      
    }
  }
  
  virtual BOOL GetProfileBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData, DWORD* pBytes)
  {
    //What will be the the return value (assume the best)
    BOOL bSuccess = TRUE;
  
    try
    {
      *pBytes = GetBinary(lpszSection, lpszEntry, ppData);
    }
    catch(CAppSettingsException* pEx)
    {
      pEx->Delete();
      bSuccess = FALSE;
    }
    
    return bSuccess;
  }
  
  virtual BOOL GetProfileStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, CStringArray& array)
  {
    //What will be the the return value (assume the best)
    BOOL bSuccess = TRUE;
  
    try
    {
      GetStringArray(lpszSection, lpszEntry, array);
    }
    catch(CAppSettingsException* pEx)
    {
      pEx->Delete();
      bSuccess = FALSE;
    }
    
    return bSuccess;
  }
  
  virtual void GetSections(CStringArray& sections) = 0;
  virtual void GetSection(LPCTSTR lpszSection, CStringArray& sectionEntries, BOOL bWithValues = TRUE) = 0;
  virtual void WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nValue) = 0;
  virtual void WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) = 0;
  virtual void WriteBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, const BYTE* pData, DWORD dwBytes) = 0;
  virtual void WriteStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, const CStringArray& array) = 0;
  virtual void WriteSection(LPCTSTR lpszSection, const CStringArray& sectionEntries) = 0;

//Helper methods
  static void ThrowWin32AppSettingsException(DWORD dwError = 0)
  {
	  if (dwError == 0)
		  dwError = ::GetLastError();

	  CAppSettingsException* pException = new CAppSettingsException(dwError, FACILITY_WIN32);

	  TRACE(_T("Warning: throwing CAppSettingsException for error %08X\n"), pException->m_hr);
	  THROW(pException);
  }

  static void ThrowCOMAppSettingsException(HRESULT hr)
  {
	  CAppSettingsException* pException = new CAppSettingsException(hr);

	  TRACE(_T("Warning: throwing CAppSettingsException for error %08X\n"), pException->m_hr);
	  THROW(pException);
  }
};


//The base app settings class which reads / writes application setttings to the windows registry
class CRegistryAppSettings : public IAppSettings
{
public:
//Constructors / Destructors
  CRegistryAppSettings() : m_bWriteFlush(FALSE)
  {
  }

//Accessors / Mutators
  void SetWriteFlush(BOOL bWriteFlush) 
  {
    m_bWriteFlush = bWriteFlush;
  }

  BOOL GetWriteFlush() const
  {
    return m_bWriteFlush;
  }

//IAppSettings
  virtual int GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry)
  {
    //Validate our parameters
	  ASSERT(lpszSection);
	  ASSERT(lpszEntry);

    //Try to get the section key
	  HKEY hSecKey = GetSectionKey(lpszSection, TRUE);
	  AFXASSUME(hSecKey);

    //Then query for the value below the section
	  DWORD dwValue;
	  DWORD dwType;
	  DWORD dwCount = sizeof(DWORD);
	  LONG lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &dwCount);
	  RegCloseKey(hSecKey);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

		ASSERT(dwType == REG_DWORD);
		ASSERT(dwCount == sizeof(dwValue));
		return static_cast<int>(dwValue);
  }

  virtual CString GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry)
  {
    //Validate our parameters
	  ASSERT(lpszSection);
	  ASSERT(lpszEntry);

    //Try to get the section key
	  HKEY hSecKey = GetSectionKey(lpszSection, TRUE);
	  AFXASSUME(hSecKey);

    //Then query for the value below the section
	  CString strValue;
	  DWORD dwType, dwCount;
	  LONG lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType, NULL, &dwCount);
    if (lResult != ERROR_SUCCESS)
    {
      //Close the section key before we throw the exception
      RegCloseKey(hSecKey);

      ThrowWin32AppSettingsException(lResult);
    }

		lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType, reinterpret_cast<LPBYTE>(strValue.GetBuffer(dwCount/sizeof(TCHAR))), &dwCount);
		strValue.ReleaseBuffer();
    RegCloseKey(hSecKey);

		ASSERT(dwType == REG_SZ);

    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

    return strValue;
  }

  virtual DWORD GetBinary(__in LPCTSTR lpszSection, __in LPCTSTR lpszEntry, __out BYTE** ppData)
  {
    //Validate our parameters
	  ASSERT(lpszSection);
	  ASSERT(lpszEntry);
	  ASSERT(ppData);
	  
	  //Set the output parameter to a default value
	  *ppData = NULL;

    //Try to get the section key
	  HKEY hSecKey = GetSectionKey(lpszSection, TRUE);
	  AFXASSUME(hSecKey);

    //First get the size of the binary data
	  DWORD dwType, dwCount;
	  LONG lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType, NULL, &dwCount);
    if (lResult != ERROR_SUCCESS)
    {
      //Close the section key before we throw the exception
      RegCloseKey(hSecKey);

      ThrowWin32AppSettingsException(lResult);
    }

		ASSERT(dwType == REG_BINARY);
		*ppData = new BYTE[dwCount];
		lResult = RegQueryValueEx(hSecKey, lpszEntry, NULL, &dwType, *ppData, &dwCount);
    RegCloseKey(hSecKey);
    if (lResult != ERROR_SUCCESS)
    {
      //Tidy up the heap memory before we throw the exception
      delete [] *ppData;

      ThrowWin32AppSettingsException(lResult);
    }

		ASSERT(dwType == REG_BINARY);

    return dwCount;
  }

  virtual void GetStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, CStringArray& array)
  {
    //Validate our parameters
	  ASSERT(lpszSection);

    //Empty out the array before we go any further
    array.RemoveAll();

    //Try to get the section key
	  HKEY hSecKey = GetSectionKey(lpszSection, TRUE);
	  AFXASSUME(hSecKey);

    //Call the helper function which does the heavy lifting
    try
    {
  	  GetStringArrayFromRegistry(hSecKey, lpszEntry, array);
    }
    catch(CAppSettingsException* pEx)
    {
      //Close the section key before we rethrow the exception
      RegCloseKey(hSecKey);

    	THROW(pEx);
    }

    //Close the key before we return
	  RegCloseKey(hSecKey);
  }

  virtual void GetSections(CStringArray& sections)
  {
    //Empty out the array before we go any further
    sections.RemoveAll();

    //Try to get the application key
	  HKEY hAppKey = GetAppKey(TRUE);
    AFXASSUME(hAppKey);
  
    //Enumerate all the section keys
    TCHAR szValueName[4096];
    DWORD dwValueNameSize = sizeof(szValueName)/sizeof(TCHAR);
    DWORD dwIndex = 0;
	  LONG nEnum = RegEnumKeyEx(hAppKey, dwIndex, szValueName, &dwValueNameSize, NULL, NULL, NULL, NULL);
	  while (nEnum == ERROR_SUCCESS)
	  {
	    //Add the section name to the array
	    sections.Add(szValueName);

	    //Prepare for the next time around
	    dwValueNameSize = sizeof(szValueName)/sizeof(TCHAR);
	    ++dwIndex;
	    nEnum = RegEnumKeyEx(hAppKey, dwIndex, szValueName, &dwValueNameSize, NULL, NULL, NULL, NULL);
	  }
  }

  virtual void GetSection(LPCTSTR lpszSection,  CStringArray& sectionEntries, BOOL bWithValues)
  {
    //Note that because we are trying to emulate the functionality of "GetPrivateProfileSection"
    //which returns a homogenous array of strings, either natively or thro CRegistryAppSetting
    //The supported data types this function supports is documented in the "switch (dwType)" code 
    //below. Note the 2 biggest omissions are REG_MULTI_SZ and REG_BINARY which both just return 
    //place holder values

    //Validate our parameters
    ASSERT(lpszSection);    

    //Empty out the array before we go any further
    sectionEntries.RemoveAll();

    //Try to get the section key
    HKEY hSecKey = GetSectionKey(lpszSection, TRUE);
    AFXASSUME(hSecKey);

    //Now enumerate all the values for the section key
    TCHAR szValueName[4096];
    DWORD dwValueNameSize = sizeof(szValueName)/sizeof(TCHAR);
    DWORD dwIndex = 0;
    DWORD dwType;
    LONG nEnum = RegEnumValue(hSecKey, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, NULL, NULL);
    while (nEnum == ERROR_SUCCESS)
    {
      if (bWithValues)
	    {
        switch (dwType)
        {
          case REG_EXPAND_SZ: //deliberate fallthrough
          case REG_SZ: 
          {
            CString sEntry;
            sEntry.Format(_T("%s=%s"), szValueName, GetString(lpszSection, szValueName).operator LPCTSTR());
            sectionEntries.Add(sEntry);
            break;
          }
          case REG_DWORD: 
          {
            CString sEntry;
            sEntry.Format(_T("%s=%d"), szValueName, GetInt(lpszSection, szValueName));
            sectionEntries.Add(sEntry);
            break;
          }
          case REG_BINARY: 
          {
            CString sEntry;
            sEntry.Format(_T("%s=<REG_BINARY VALUE>"), szValueName);
            sectionEntries.Add(sEntry);
            break;
          }
          case REG_MULTI_SZ: 
          {
            CString sEntry;
            sEntry.Format(_T("%s=<REG_MULTI_SZ VALUE>"), szValueName);
            sectionEntries.Add(sEntry);
            break;
          }
          default:
          {
            TRACE(_T("CRegistryAppSettings::GetSection, Encountered a data type which will not be included in returned data, DataType:%d\n"), dwType);
            break;
          }
        }
      }
      else
      {
        //Simply use the value name
        sectionEntries.Add(szValueName);
      }

      //Prepare for the next time around
      dwValueNameSize = sizeof(szValueName)/sizeof(TCHAR);
      ++dwIndex;
      nEnum = RegEnumValue(hSecKey, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, NULL, NULL);
    }
  }

  virtual void WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nValue)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteInt(lpszSection, lpszEntry, nValue, m_bWriteFlush);
  }

  virtual void WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteString(lpszSection, lpszEntry, lpszValue, m_bWriteFlush);
  }

  virtual void WriteBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, const BYTE* pData, DWORD dwBytes)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteBinary(lpszSection, lpszEntry, pData, dwBytes, m_bWriteFlush);
  }

  virtual void WriteStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, const CStringArray& array)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteStringArray(lpszSection, lpszEntry, array, m_bWriteFlush);
  }

  virtual void WriteSection(LPCTSTR lpszSection, const CStringArray& sectionEntries)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteSection(lpszSection, sectionEntries, m_bWriteFlush);
  }


//Versions of the Write* methods which allow you to specify the flush setting as a parameter
  void WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nValue, BOOL bFlush)
  {
    //Validate our parameters
	  ASSERT(lpszSection);
	  ASSERT(lpszEntry);

    //Try to get the section key
	  HKEY hSecKey = GetSectionKey(lpszSection, FALSE);
	  AFXASSUME(hSecKey);

    //Then write out the value below the section
	  LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_DWORD, reinterpret_cast<LPBYTE>(&nValue), sizeof(nValue));
    if (lResult != ERROR_SUCCESS)
    {
      //Close the section key before we throw the exception
      RegCloseKey(hSecKey);

      ThrowWin32AppSettingsException(lResult);
    }

    //Flush if required
	  if (bFlush)
		  RegFlushKey(hSecKey);

    //Close the key before we return
	  RegCloseKey(hSecKey);
  }

  void WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue, BOOL bFlush)
  {
    //Validate our parameters
	  AFXASSUME(lpszSection);

	  if (lpszEntry == NULL) //delete whole section
	  {
      //Try to get the application key
		  HKEY hAppKey = GetAppKey(FALSE);
		  AFXASSUME(hAppKey);

      //Nuke the specified section
		  LONG lResult = ::RegDeleteKey(hAppKey, lpszSection);
      if (lResult != ERROR_SUCCESS)
      {
        //Close the section key before we throw the exception
        RegCloseKey(hAppKey);

        ThrowWin32AppSettingsException(lResult);
      }

      //Flush if required
	    if (bFlush)
		    RegFlushKey(hAppKey);

      //Close the key before we return
		  RegCloseKey(hAppKey);
	  }
	  else if (lpszValue == NULL)
	  {
      //Try to get the section key
		  HKEY hSecKey = GetSectionKey(lpszSection, FALSE);
		  AFXASSUME(hSecKey);

		  //Delete the section key (Note it is necessary to cast away constness below)
		  LONG lResult = ::RegDeleteValue(hSecKey, lpszEntry);
      if (lResult != ERROR_SUCCESS)
      {
        //Close the section key before we throw the exception
        RegCloseKey(hSecKey);

        ThrowWin32AppSettingsException(lResult);
      }

      //Flush if required
	    if (bFlush)
		    RegFlushKey(hSecKey);

      //Close the key before we return
		  RegCloseKey(hSecKey);
	  }
	  else
	  {
      //Try to get the section key
		  HKEY hSecKey = GetSectionKey(lpszSection, FALSE);
		  AFXASSUME(hSecKey);

      //Then write out the value below the section
		  LONG lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_SZ, reinterpret_cast<const BYTE*>(lpszValue), (lstrlen(lpszValue)+1)*sizeof(TCHAR));
      if (lResult != ERROR_SUCCESS)
      {
        //Close the section key before we throw the exception
        RegCloseKey(hSecKey);

        ThrowWin32AppSettingsException(lResult);
      }

      //Flush if required
	    if (bFlush)
		    RegFlushKey(hSecKey);

      //Close the key before we return
		  RegCloseKey(hSecKey);
	  }
  }

  void WriteBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, const BYTE* pData, DWORD dwBytes, BOOL bFlush)
  {
    //Validate our parameters
	  ASSERT(lpszSection);

    //Try to get the section key
	  HKEY hSecKey = GetSectionKey(lpszSection, FALSE);
	  AFXASSUME(hSecKey);

    //Then write out the value below the section
	  long lResult = RegSetValueEx(hSecKey, lpszEntry, NULL, REG_BINARY, pData, dwBytes);
    if (lResult != ERROR_SUCCESS)
    {
      //Close the section key before we throw the exception
      RegCloseKey(hSecKey);

      ThrowWin32AppSettingsException(lResult);
    }

    //Flush if required
	  if (bFlush)
		  RegFlushKey(hSecKey);

    //Close the key before we return
	  RegCloseKey(hSecKey);
  }

  void WriteStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, const CStringArray& array, BOOL bFlush)
  {
    //Validate our parameters
	  ASSERT(lpszSection);

    //Try to get the section key
	  HKEY hSecKey = GetSectionKey(lpszSection, FALSE);
	  AFXASSUME(hSecKey);

    //Call the helper function which does the heavy lifting
    try
    {
	    SetStringArrayIntoRegistry(hSecKey, lpszEntry, array);
    }
    catch(CAppSettingsException* pEx)
    {
      //Close the section key before we rethrow the exception
      RegCloseKey(hSecKey);

    	THROW(pEx);
    }

    //Flush if required
	  if (bFlush)
		  RegFlushKey(hSecKey);

    //Close the key before we return
	  RegCloseKey(hSecKey);
  }

  void WriteSection(LPCTSTR lpszSection, const CStringArray& sectionEntries, BOOL bFlush)
  {
    //Validate our parameters
	  ASSERT(lpszSection);

    //First nuke the registry key entirely (using the WriteString delete section functionality!)
    WriteString(lpszSection, NULL, NULL, bFlush);

    //Now get the (newly created) section key
	  HKEY hSecKey = GetSectionKey(lpszSection, FALSE);
	  AFXASSUME(hSecKey);

    try
    {
      //now write out all the new entries
      INT_PTR nEntries = sectionEntries.GetSize();
      for (INT_PTR i=0; i<nEntries; i++)
      {
        CString sEntry(sectionEntries.GetAt(i));
        CString sName;
        CString sValue;
        int nSeparator = sEntry.Find(_T('='));
        if (nSeparator != -1)
        {
          sName = sEntry.Left(nSeparator);
          sValue = sEntry.Right(sEntry.GetLength() - 1 - nSeparator);
        }
        else
          sName = sEntry;

        WriteString(lpszSection, sName, sValue, FALSE);
      }
    }
    catch(CAppSettingsException* pEx)
    {
      //Close the section key before we rethrow the exception
      RegCloseKey(hSecKey);

    	THROW(pEx);
    }
    
    //Flush if required
	  if (bFlush)
		  RegFlushKey(hSecKey);

    //Close the key before we return
	  RegCloseKey(hSecKey);
  }

protected:
//Helper functions

  //returns key one level down from GetAppKey. It is the responsibility of the caller to call 
  //RegCloseKey() on the returned HKEY
  HKEY GetSectionKey(LPCTSTR lpszSection, BOOL bReadOnly)
  {
    //Validate our parameters
	  AFXASSUME(lpszSection);

    //Try to get the application key
	  HKEY hAppKey = GetAppKey(bReadOnly);
    AFXASSUME(hAppKey);

    //Try to open/create the key
	  DWORD dw;
	  HKEY hSectionKey = NULL;
	  LONG lResult = RegCreateKeyEx(hAppKey, lpszSection, 0, REG_NONE, REG_OPTION_NON_VOLATILE, bReadOnly ? KEY_READ : KEY_WRITE | KEY_READ, NULL, &hSectionKey, &dw);
	  RegCloseKey(hAppKey);

    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

	  return hSectionKey;
  }

  static void GetStringArrayFromRegistry(HKEY hKey, LPCTSTR lpszEntry, CStringArray& array)
  {
    //Validate our parameters
	  AFXASSUME(hKey);
	  ASSERT(lpszEntry);

    //Empty the array before we go any further
    array.RemoveAll();

    //Get the type and size of the data
	  DWORD dwType, dwCount;
	  LONG lResult = RegQueryValueEx(hKey, lpszEntry, NULL, &dwType, NULL, &dwCount);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

		ASSERT(dwType == REG_MULTI_SZ);
		
		//Allocate some heap memory to hold the data
		ATL::CHeapPtr<BYTE> dataBuffer;
		if (!dataBuffer.Allocate(dwCount))
		  ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);
		
		lResult = RegQueryValueEx(hKey, lpszEntry, NULL, &dwType, dataBuffer, &dwCount);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

    //Iterate thro the multi SZ string and add each one to the string array
    LPTSTR lpszStrings = reinterpret_cast<LPTSTR>(dataBuffer.m_pData);
    while (lpszStrings[0] != 0)
    {
      array.Add(lpszStrings);
      lpszStrings += (_tcslen(lpszStrings ) + 1);
    }
  }

  static void SetStringArrayIntoRegistry(HKEY hKey, LPCTSTR lpszEntry, const CStringArray& array)
  {   
    //Validate our input parameters
    AFXASSUME(hKey);
    ASSERT(lpszEntry);

    //Work out the size of the buffer we will need
    DWORD dwSize = 0;
    INT_PTR nStrings = array.GetSize();
    for (INT_PTR i=0; i<nStrings; i++)
      dwSize += array.GetAt(i).GetLength() + 1; //1 extra for each NULL terminator

    //Need one second NULL for the double NULL at the end
    dwSize++;

    //Allocate the memory we want
    ATL::CHeapPtr<TCHAR> dataBuffer;
    if (!dataBuffer.Allocate(dwSize))
      ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);

    //Now copy the strings into the buffer
    int nCurOffset = 0;
    LPTSTR lpszString = dataBuffer.m_pData;
    for (INT_PTR i=0; i<nStrings; i++)
    {
      CString sText(array.GetAt(i));
      int nCurrentStringLength = sText.GetLength();
      _tcscpy_s(&lpszString[nCurOffset], nCurrentStringLength + 1, sText);
      nCurOffset += nCurrentStringLength;
      nCurOffset++;
    }
    //Don't forgot to doubly NULL terminate
    lpszString[nCurOffset] = _T('\0');

    //Finally write it into the registry
	  LONG lResult = RegSetValueEx(hKey, lpszEntry, NULL, REG_MULTI_SZ, reinterpret_cast<const BYTE*>(dataBuffer.m_pData), dwSize);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);
  }

//pure virtual function which derived classes must implement to make a concrete class definition
  virtual HKEY GetAppKey(BOOL bReadOnly) = 0;

//member variables
  BOOL m_bWriteFlush;
};


//The base app settings class for CHKLMAppSettings and CHCURMAppSettings
class CRegistryApplicationAppSettings : public CRegistryAppSettings
{
public:
//Constructors / Destructors
  CRegistryApplicationAppSettings()
  {
  }

//Accessors / Mutators
  void SetProductName(const CString& sProductName)
  {
    m_sProductName = sProductName;
  }

  CString GetProductName() const
  {
    return m_sProductName;
  }

  void SetCompanyName(const CString& sCompanyName)
  {
    m_sCompanyName = sCompanyName;
  }

  CString GetCompanyName() const
  {
    return m_sCompanyName;
  }

  void SetProductVersion(const CString& sProductVersion)
  {
    m_sProductVersion = sProductVersion;
  }

  CString GetProductVersion() const
  {
    return m_sProductVersion;
  }

protected:
//Helper functions
  virtual HKEY GetAppKey(BOOL bReadOnly)
  {
    //Validate our parameters
    ASSERT(m_sCompanyName.GetLength()); //Did you forget to call "SetCompanyName"?
	  ASSERT(m_sProductName.GetLength());

    //Setup the access type
    REGSAM samDesired = bReadOnly ? KEY_READ : KEY_WRITE | KEY_READ;

    //Open the software key
	  HKEY hSoftKey = NULL;
    LONG lResult = RegOpenKeyEx(GetRootKey(), _T("Software"), 0, samDesired,	&hSoftKey);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

    //Open the company key
		DWORD dwDisposition;
	  HKEY hCompanyKey = NULL;
		lResult = RegCreateKeyEx(hSoftKey, m_sCompanyName, 0, REG_NONE,	REG_OPTION_NON_VOLATILE, samDesired, NULL, &hCompanyKey, &dwDisposition);
    RegCloseKey(hSoftKey);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

    //Open the product key
    HKEY hProductKey = NULL;
		lResult = RegCreateKeyEx(hCompanyKey, m_sProductName, 0, REG_NONE,	REG_OPTION_NON_VOLATILE, samDesired, NULL, &hProductKey, &dwDisposition);
    RegCloseKey(hCompanyKey);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

    //Open the ProductVersion key (if necessary)  
    if (m_sProductVersion.GetLength())
    {
  	  HKEY hProductVersionKey = NULL;
      lResult = RegCreateKeyEx(hProductKey, m_sProductVersion, 0, REG_NONE,	REG_OPTION_NON_VOLATILE, samDesired, NULL, &hProductVersionKey, &dwDisposition);
      RegCloseKey(hProductKey);
      if (lResult != ERROR_SUCCESS)
        ThrowWin32AppSettingsException(lResult);

      return hProductVersionKey;
    }
    else
      return hProductKey;
  }
  virtual HKEY GetRootKey() = 0;

//member variables
  CString m_sCompanyName;
  CString m_sProductName;
  CString m_sProductVersion;
};


//The app settings class which reads / writes application setttings to the HKLM node in the windows registry
class CHKLMAppSettings : public CRegistryApplicationAppSettings
{
public:
//Constructors / Destructors
  CHKLMAppSettings(const CString& sCompanyName, const CString& sProductName = AfxGetApp()->m_pszAppName, 
                   const CString& sProductVersion = _T("1.0"), BOOL bWriteFlush = FALSE)
  {
    m_sCompanyName = sCompanyName;
    m_sProductName = sProductName;
    m_sProductVersion = sProductVersion;
    m_bWriteFlush = bWriteFlush;
  }

protected:
//virtual methods
   virtual HKEY GetRootKey() 
  { 
    return HKEY_LOCAL_MACHINE; 
  }
};


//The app settings class which reads / writes application setttings to the HKLM node in the windows registry
class CHKCUAppSettings : public CRegistryApplicationAppSettings
{
public:
//Constructors / Destructors
  CHKCUAppSettings(const CString& sCompanyName, const CString& sProductName = AfxGetApp()->m_pszAppName, 
                   const CString& sProductVersion = _T("1.0"), BOOL bWriteFlush = FALSE)
  {
    m_sCompanyName = sCompanyName;
    m_sProductName = sProductName;
    m_sProductVersion = sProductVersion;
    m_bWriteFlush = bWriteFlush;
  }

protected:
//virtual methods
  virtual HKEY GetRootKey() 
  {
    return HKEY_CURRENT_USER; 
  }
};

//The app settings class which reads / writes setttings to the HKLM\System\CurrentControlSet\Services\"ServiceName"\Parameters node in the windows registry
class CServicesAppSettings : public CRegistryAppSettings
{
public:
//Constructors / Destructors
  CServicesAppSettings(const CString& sServiceName, BOOL bWriteFlush = FALSE)
  {
    m_bWriteFlush = bWriteFlush;
    m_sServiceName = sServiceName;
  }

//Accessors / Mutators
  void SetServiceName(const CString& sServiceName)
  {
    m_sServiceName = sServiceName;
  }

  CString GetServiceName() const
  {
    return m_sServiceName;
  }

protected:
//virtual methods
  virtual HKEY GetAppKey(BOOL bReadOnly)
  {
    //Validate our parameters
    ASSERT(m_sServiceName.GetLength()); //Did you forget to call "SetServiceName"?

    //Setup the access type
    REGSAM samDesired = bReadOnly ? KEY_READ : KEY_WRITE | KEY_READ;

    //Open the Services key
	  HKEY hServicesKey = NULL;
    LONG lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services"), 0, samDesired,	&hServicesKey);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

    //Open the service key
		DWORD dwDisposition;
	  HKEY hServiceKey = NULL;
		lResult = RegCreateKeyEx(hServicesKey, m_sServiceName, 0, REG_NONE,	REG_OPTION_NON_VOLATILE, samDesired, NULL, &hServiceKey, &dwDisposition);
    RegCloseKey(hServicesKey);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

    //Open the Parmameters key
	  HKEY hParametersKey = NULL;
		lResult = RegCreateKeyEx(hServiceKey, _T("Parameters"), 0, REG_NONE,	REG_OPTION_NON_VOLATILE, samDesired, NULL, &hParametersKey, &dwDisposition);
    RegCloseKey(hServiceKey);
    if (lResult != ERROR_SUCCESS)
      ThrowWin32AppSettingsException(lResult);

    return hParametersKey;
  }

//Member variables
  CString m_sServiceName;
};


//The App settings class which reads / writes application setttings to an windows ini file
class CIniAppSettings : public IAppSettings
{
public:
//Constructors / Destructors
  CIniAppSettings(const CString& sIniFile) : m_sIniFile(sIniFile)
  {
  }

//Accessors / Mutators
  void SetIniFile(const CString& sIniFile)
  {
    m_sIniFile = sIniFile;
  }

  CString GetIniFile() const
  {
    return m_sIniFile;
  }

//IAppSettings
  virtual int GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry)
  {
    CString sValue = GetString(lpszSection, lpszEntry);
		return _ttoi(sValue);
  }

  virtual CString GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry)
  {
    //Validate our parameters
    ASSERT(m_sIniFile.GetLength()); 
    
    //Generate a GUID for the default string so that we will absolutely know when we have been returned a default 
    //string from GetPrivateProfileString
    GUID guid;
    HRESULT hr = CoCreateGuid(&guid);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);
    LPOLESTR wszCLSID = NULL;
    StringFromCLSID(guid, &wszCLSID);
    COLE2T sCLSID(wszCLSID);
    CoTaskMemFree(wszCLSID);

		TCHAR szT[4096];
		szT[0] = _T('\0');
		GetPrivateProfileString(lpszSection, lpszEntry, sCLSID, szT, sizeof(szT)/sizeof(TCHAR), m_sIniFile);

    //Check to see if the return value matches the default, and if it did throw an exception
    if (_tcscmp(szT, sCLSID) == 0)
      ThrowWin32AppSettingsException(ERROR_NOT_FOUND);

		return szT;
  }

  virtual DWORD GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData)
  {
		CString sBinary(GetString(lpszSection, lpszEntry));
		if (sBinary.IsEmpty())
			return 0;
		ASSERT(sBinary.GetLength() % 2 == 0); //Data length returned should be a multiple of 2
		INT_PTR nLen = sBinary.GetLength();
		DWORD dwBytes = UINT(nLen)/2;
		*ppData = new BYTE[dwBytes];
		for (int i=0; i<nLen; i+=2)
			(*ppData)[i/2] = static_cast<BYTE>(((sBinary[i+1] - 'A') << 4) + (sBinary[i] - 'A'));
			
		return dwBytes;
  }

  virtual void GetStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, CStringArray& array)
  {
    //Empty out all the elements of the array
    array.RemoveAll();
    
    //Try to get the binary data first
    BYTE* pData = NULL;
    GetBinary(lpszSection, lpszEntry, &pData);
    LPTSTR lpszStrings = reinterpret_cast<LPTSTR>(pData);
    while (lpszStrings[0] != 0)
    {
      array.Add(lpszStrings);
      lpszStrings += (_tcslen(lpszStrings ) + 1);
    }

    //Tidy up the heap memory before we exit
    delete [] pData;
  }
  
  virtual void GetSections(CStringArray& sections)
  {
    //Empty out the array before we go any further
    sections.RemoveAll();

    DWORD dwSize = 1024;
    BOOL bSuccess = FALSE;
    while (!bSuccess)
    {
      //Allocate some heap memory for the SDK call
      ATL::CHeapPtr<TCHAR> sectionsBuffer;
      if (!sectionsBuffer.Allocate(dwSize))
        ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);

      //Call the SDK function
      DWORD dwRetreived = GetPrivateProfileString(NULL, NULL, NULL, sectionsBuffer.m_pData, dwSize, m_sIniFile);
      if (dwRetreived == (dwSize - 2))
      {
        //Realloc the array by doubling its size ready for the next loop around
        dwSize *= 2;
      }
      else if (dwRetreived == 0)
      {
        DWORD dwError = GetLastError();

        //Only consider it an error if no data was retreived and GetLastError flagged an error
        if (dwError)
          ThrowWin32AppSettingsException(dwError);
        else
        {
          //Just return with an empty array
          bSuccess = TRUE;
        }
      }
      else
      {
        //Convert from a multi sz string format to the CStringArray out parameter
        bSuccess = TRUE;

        LPTSTR pszCurrentString = sectionsBuffer.m_pData;
        while (pszCurrentString[0] != _T('\0'))
        {
          sections.Add(pszCurrentString);
          pszCurrentString += (_tcslen(pszCurrentString) + 1);
        }
      }
    }
  }    

  virtual void GetSection(LPCTSTR lpszSection, CStringArray& sectionEntries, BOOL bWithValues)
  {
    //Validate our parameters
    AFXASSUME(lpszSection);

    //Empty out the array before we go any further
    sectionEntries.RemoveAll();

    DWORD dwSize = 1024;
    BOOL bSuccess = FALSE;
    while (!bSuccess)
    {
      //Allocate some heap memory for the SDK call
      ATL::CHeapPtr<TCHAR> sectionEntriesBuffer;
      if (!sectionEntriesBuffer.Allocate(dwSize))
        ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);

      //Call the SDK function
      DWORD dwRetreived = 0;
      if (bWithValues)
        dwRetreived = GetPrivateProfileSection(lpszSection, sectionEntriesBuffer.m_pData, dwSize, m_sIniFile);
      else
        dwRetreived = GetPrivateProfileString(lpszSection, NULL, NULL, sectionEntriesBuffer.m_pData, dwSize, m_sIniFile);
      if (dwRetreived == (dwSize - 2))
      {
        //Realloc the array by doubling its size ready for the next loop around
        dwSize *= 2;
      }
      else if (dwRetreived == 0)
      {
        DWORD dwError = GetLastError();

        //Only consider it an error if no data was retreived and GetLastError flagged an error
        if (dwError)
          ThrowWin32AppSettingsException(dwError);
        else
        {
          //Just return with an empty array
          bSuccess = TRUE;
        }
      }
      else
      {
        //Convert from a multi sz string format to the CStringArray out parameter
        bSuccess = TRUE;

        LPTSTR pszCurrentString = sectionEntriesBuffer.m_pData;
        while (pszCurrentString[0] != _T('\0'))
        {
          sectionEntries.Add(pszCurrentString);
          pszCurrentString += (_tcslen(pszCurrentString) + 1);
        }
      }
    }
  }

  virtual void WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nValue)
  {
    //Simply convert to a string and delegate to WriteString
    CString sValue;
    sValue.Format(_T("%d"), nValue);
		WriteString(lpszSection, lpszEntry, sValue);
  }

  virtual void WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
  {
    //Validate our parameters
    ASSERT(m_sIniFile.GetLength());  

		if (!::WritePrivateProfileString(lpszSection, lpszEntry, lpszValue, m_sIniFile))
      ThrowWin32AppSettingsException();
  }

  virtual void WriteBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, const BYTE* pData, DWORD dwBytes)
  {
	  //Allocate some heap memory for the data we will be writing out
	  ATL::CHeapPtr<TCHAR> dataBuffer;
	  if (!dataBuffer.Allocate((dwBytes*2) + 1))
	    ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);

	  //convert the data to write out to string format
	  DWORD i;
	  for (i=0; i<dwBytes; i++)
	  {
		  dataBuffer.m_pData[i*2] = static_cast<TCHAR>((pData[i] & 0x0F) + 'A'); //low nibble
		  dataBuffer.m_pData[i*2+1] = static_cast<TCHAR>(((pData[i] >> 4) & 0x0F) + 'A'); //high nibble
	  }
	  dataBuffer.m_pData[i*2] = 0;

    //Call the WriteString method to write out the data for us
    WriteString(lpszSection, lpszEntry, dataBuffer.m_pData);
  }

  virtual void WriteStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, const CStringArray& array)
  {
    //Work out the size of the buffer we will need
    DWORD dwSize = 0;
    INT_PTR nStrings = array.GetSize();
    for (INT_PTR i=0; i<nStrings; i++)
      dwSize += array.GetAt(i).GetLength() + 1; //1 extra for each NULL terminator

    //Need one second NULL for the double NULL at the end
    dwSize++;

    //Allocate the memory we want
    ATL::CHeapPtr<TCHAR> dataBuffer;
    if (!dataBuffer.Allocate(dwSize))
      ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);

    //Now copy the strings into the buffer
    int nCurOffset = 0;
    LPTSTR lpszString = dataBuffer.m_pData;
    for (INT_PTR i=0; i<nStrings; i++)
    {
      CString sText(array.GetAt(i));
      int nCurrentStringLength = sText.GetLength();
      _tcscpy_s(&lpszString[nCurOffset], nCurrentStringLength + 1, sText);
      nCurOffset += nCurrentStringLength;
      nCurOffset++;
    }
    //Don't forgot to doubly NULL terminate
    lpszString[nCurOffset] = _T('\0');

    //Call the WriteBinary method to write out the data for us
    WriteBinary(lpszSection, lpszEntry, reinterpret_cast<const BYTE*>(dataBuffer.m_pData), dwSize*sizeof(TCHAR));
  }

  virtual void WriteSection(LPCTSTR lpszSection, const CStringArray& sectionEntries)
  {
    //Validate our parameters
    ASSERT(m_sIniFile.GetLength());  

    //Work out the size of the doubly NULL terminated string we need
    INT_PTR nSize = sectionEntries.GetSize();
    int nStringSize = 0;
    for (INT_PTR i=0; i<nSize; i++)
    {
      nStringSize += sectionEntries.GetAt(i).GetLength();     
      nStringSize++; //Include the NULL terminator
    }
    nStringSize++; //Include the final NULL terminator

    //Allocate the memory we want
    ATL::CHeapPtr<TCHAR> keyValuesBuffer;
    if (!keyValuesBuffer.Allocate(nStringSize))
      ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);
      
    //Now copy the strings into the buffer
    int nCurOffset = 0;
    LPTSTR lpszString = keyValuesBuffer.m_pData;
    for (INT_PTR i=0; i<nSize; i++)
    {
      CString sText(sectionEntries.GetAt(i));
      int nCurrentStringLength = sText.GetLength();
      _tcscpy_s(&lpszString[nCurOffset], nCurrentStringLength + 1, sText); 
      nCurOffset += nCurrentStringLength;
      ++nCurOffset;
    }
    lpszString[nCurOffset] = _T('\0'); //Include the final NULL terminator

    //Call the SDK function
    BOOL bSuccess = WritePrivateProfileSection(lpszSection, keyValuesBuffer.m_pData, m_sIniFile);
    if (!bSuccess)
    {
      DWORD dwError = GetLastError();
      ThrowWin32AppSettingsException(dwError);          
    }
  }

//Static methods
  //Note, using this function for an ini path is not a good idea for security reasons, but if you really want to you can use this function to return a ini 
  //path which has a filename the same name as the current process and in the same directory as it.
  static CString GetCurrentProcessIniFileLocation()
  {
    TCHAR szIni[_MAX_PATH];
    if (!GetModuleFileName(NULL, szIni, _MAX_PATH))
      ThrowWin32AppSettingsException();
    TCHAR szDrive[_MAX_DRIVE];
    TCHAR szDir[_MAX_DIR];
    TCHAR szFname[_MAX_FNAME];
    _tsplitpath_s(szIni, szDrive, sizeof(szDrive)/sizeof(TCHAR), szDir, sizeof(szDir)/sizeof(TCHAR), szFname, sizeof(szFname)/sizeof(TCHAR), NULL, 0);
    _tmakepath_s(szIni, sizeof(szIni)/sizeof(TCHAR), szDrive, szDir, szFname, _T("ini"));

    return szIni; 
  }

  //Note that GetRoamingDataFileLocation will only work if IE4 integrated shell is installed. This means out of the box Windows 98 or later
  //or Windows 95 with IE4 with integrated shell installed.
  static CString GetRoamingDataFileLocation(const CString& sCompanyName, const CString& sProductName, const CString& sProductVersion, const CString& sIniFileName)
  {
    return GetSpecialFolder(CSIDL_APPDATA, sCompanyName, sProductName, sProductVersion, sIniFileName);
  }

  //Note that GetNonRoamingDataFileLocation will only work on Windows 2000 or later
  static CString GetNonRoamingDataFileLocation(const CString& sCompanyName, const CString& sProductName, const CString& sProductVersion, const CString& sIniFileName)
  {
    return GetSpecialFolder(CSIDL_LOCAL_APPDATA, sCompanyName, sProductName, sProductVersion, sIniFileName);
  }

  //Note that GetPerComputerDataFileLocation will only work on Windows 2000 or later  
  static CString GetPerComputerDataFileLocation(const CString& sCompanyName, const CString& sProductName, const CString& sProductVersion, const CString& sIniFileName)
  {
    return GetSpecialFolder(CSIDL_COMMON_APPDATA, sCompanyName, sProductName, sProductVersion, sIniFileName);
  }

protected:
//Helper functions
  static void MakeEnsureDirectory(LPCTSTR pszDirectory)
  {
    //Check to see if it already exists
    DWORD dwAttributes = GetFileAttributes(pszDirectory);
    if (dwAttributes == INVALID_FILE_ATTRIBUTES) //Does not exist, then attempt to create it
    {
      if (!CreateDirectory(pszDirectory, NULL))
        ThrowWin32AppSettingsException();
    }
    else
    {
      if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) //Check that what exists is a directory
        ThrowWin32AppSettingsException(ERROR_CANNOT_MAKE);
    }
  }

  static CString GetSpecialFolder(int nFolder, const CString& sCompanyName, const CString& sProductName, const CString& sProductVersion, const CString& sIniFileName)
  {
    //Validate our parameters
    ASSERT(sCompanyName.GetLength());
    ASSERT(sProductName.GetLength());
    ASSERT(sIniFileName.GetLength());

    //Use SHGetSpecialFolderPath instead of SHGetFolderPath because that means that the code can support down level operating systems i.e.
    //< Windows 2000, if you ensure that you redistribute ShFolder.dll.
    TCHAR szPath[_MAX_PATH];
    if (!SHGetSpecialFolderPath(NULL, szPath, nFolder, FALSE))
      ThrowWin32AppSettingsException();

    //Now create/open the CompanyName folder below the required CSIDL folder
    _tcscat_s(szPath, sizeof(szPath)/sizeof(TCHAR), _T("\\"));
    _tcscat_s(szPath, sizeof(szPath)/sizeof(TCHAR), sCompanyName);
    MakeEnsureDirectory(szPath);
      
    //Now create/open the ProductName folder below the CompanyName folder
    _tcscat_s(szPath, sizeof(szPath)/sizeof(TCHAR), _T("\\"));
    _tcscat_s(szPath, sizeof(szPath)/sizeof(TCHAR), sProductName);
    MakeEnsureDirectory(szPath);
      
    if (sProductVersion.GetLength()) //ProductVersion string is optional
    {
      _tcscat_s(szPath, sizeof(szPath)/sizeof(TCHAR), _T("\\"));
      _tcscat_s(szPath, sizeof(szPath)/sizeof(TCHAR), sProductVersion);
      MakeEnsureDirectory(szPath);
    }

    //Finally tack on the filename required to the calculated path
    _tcscat_s(szPath, sizeof(szPath)/sizeof(TCHAR), _T("\\"));
    _tcscat_s(szPath, sizeof(szPath)/sizeof(TCHAR), sIniFileName);
    
    //If wer got here, then everything is cool
    return szPath;
  }

//Member variables
  CString m_sIniFile;
};


//The App settings class which reads / writes application setttings to an windows XML file
class CXMLAppSettings : public IAppSettings
{
public:
//Constructors / Destructors
  CXMLAppSettings(const CString& sXMLFile, BOOL bWriteFlush = FALSE, BOOL bPrettyPrint = FALSE) : m_sXMLFile(sXMLFile),
                                                                                                  m_bDirty(FALSE),
                                                                                                  m_bWriteFlush(bWriteFlush),
                                                                                                  m_bPrettyPrint(bPrettyPrint)
  {
  }

  virtual ~CXMLAppSettings()
  {
    //Note we avoid throwing exceptions from the destructor
    try
    {
      Flush();
    }
    catch(CAppSettingsException* pEx)
    {
      pEx->Delete();
    }
  }

//Accessors / Mutators
  void SetXMLFile(const CString& sXMLFile)
  {
    //Flush the document to ensure that we do not have remants of the old document when we start with a new filename
    Flush();
    m_XMLDOM.Release();

    m_sXMLFile = sXMLFile;
  }

  CString GetXMLFile() const 
  {
    return m_sXMLFile;
  }

  void SetWriteFlush(BOOL bWriteFlush) 
  {
    m_bWriteFlush = bWriteFlush;
  }

  BOOL GetWriteFlush() const
  {
    return m_bWriteFlush;
  }

//IAppSettings
  virtual int GetInt(LPCTSTR lpszSection, LPCTSTR lpszEntry)
  {
    CString sValue(GetString(lpszSection, lpszEntry));
		return _ttoi(sValue);
  }

  virtual CString GetString(LPCTSTR lpszSection, LPCTSTR lpszEntry)
  {
    //Get the entry node
    ATL::CComPtr<IXMLDOMNode> entryNode = GetEntryNode(lpszSection, lpszEntry, TRUE);

    //Pull out the text of the node
    CComBSTR bstrText;
    HRESULT hr = entryNode->get_text(&bstrText);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);

    return CString(bstrText);   
  }

  virtual DWORD GetBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, BYTE** ppData)
  {
		CString sBinary(GetString(lpszSection, lpszEntry));
		if (sBinary.IsEmpty())
			return 0;
		ASSERT(sBinary.GetLength()%2 == 0); //Data length returned should be a multiple of 2
		INT_PTR nLen = sBinary.GetLength();
		DWORD dwBytes = UINT(nLen)/2;
		*ppData = new BYTE[dwBytes];
		for (int i=0; i<nLen; i+=2)
			(*ppData)[i/2] = static_cast<BYTE>(((sBinary[i+1] - 'A') << 4) + (sBinary[i] - 'A'));
		return dwBytes;
  }

  virtual void GetStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, CStringArray& array)
  {
    //Empty out all the elements of the array
    array.RemoveAll();
    
    //Try to get the binary data first
    BYTE* pData = NULL;
    GetBinary(lpszSection, lpszEntry, &pData);
    LPTSTR lpszStrings = reinterpret_cast<LPTSTR>(pData);
    if (lpszStrings)
    {
      while (lpszStrings[0] != 0)
      {
        array.Add(lpszStrings);
        lpszStrings += (_tcslen(lpszStrings ) + 1);
      }

      //Tidy up the heap memory before we exit
      delete [] pData;
    }
  }
  
  virtual void GetSections(CStringArray& sections)
  {
    //empty out the array before we go any further
    sections.RemoveAll();
    
    //Get the document node
    ATL::CComPtr<IXMLDOMNode> documentNode = GetDocumentNode(TRUE);
  
    //Get all the child nodes of the document node
    ATL::CComPtr<IXMLDOMNodeList> childNodes;
    HRESULT hr = documentNode->get_childNodes(&childNodes);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);

    //Now get the name and value of each of the child nodes
    long lLength;
    hr = childNodes->get_length(&lLength);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);
    for (long i=0; i<lLength; i++)
    {
      ATL::CComPtr<IXMLDOMNode> childNode;
      hr = childNodes->get_item(i, &childNode);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      //Get the name of the node
      CComBSTR bstrValueName;
      hr = childNode->get_nodeName(&bstrValueName);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);
      CString sValueName(bstrValueName);
      sections.Add(sValueName);
    }
  }  

  virtual void GetSection(LPCTSTR lpszSection, CStringArray& sectionEntries, BOOL bWithValues)
  {
    //Empty out the array before we go any further
    sectionEntries.RemoveAll();

    //get the section node
    ATL::CComPtr<IXMLDOMNode> sectionNode = GetSectionNode(lpszSection, TRUE);

    //Get all the child nodes of the section node
    ATL::CComPtr<IXMLDOMNodeList> childNodes;
    HRESULT hr = sectionNode->get_childNodes(&childNodes);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);

    //Now get the name and value of each of the child nodes
    long lLength;
    hr = childNodes->get_length(&lLength);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);
    for (long i=0; i<lLength; i++)
    {
      ATL::CComPtr<IXMLDOMNode> childNode;
      hr = childNodes->get_item(i, &childNode);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      //Get the name of the node
      CComBSTR bstrValueName;
      hr = childNode->get_nodeName(&bstrValueName);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);
      CString sValueName(bstrValueName);

      //Add the appropriate value to the array
      CString sEntry;
      if (bWithValues)
        sEntry.Format(_T("%s=%s"), sValueName.operator LPCTSTR(), GetString(lpszSection, sValueName).operator LPCTSTR());
      else
        sEntry = sValueName;
      sectionEntries.Add(sEntry);
    }
  }

  virtual void WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nValue)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteInt(lpszSection, lpszEntry, nValue, m_bWriteFlush);
  }

  virtual void WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteString(lpszSection, lpszEntry, lpszValue, m_bWriteFlush);
  }

  virtual void WriteBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, const BYTE* pData, DWORD dwBytes)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteBinary(lpszSection, lpszEntry, pData, dwBytes, m_bWriteFlush);
  }

  virtual void WriteStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, const CStringArray& array)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteStringArray(lpszSection, lpszEntry, array, m_bWriteFlush);
  }

  virtual void WriteSection(LPCTSTR lpszSection, const CStringArray& sectionEntries)
  {
    //Just delegate to the version which specfies the flush setting as a parameter
    WriteSection(lpszSection, sectionEntries, m_bWriteFlush);
  }

//Versions of the Write* methods which allow you to specify the flush setting as a parameter
  void WriteInt(LPCTSTR lpszSection, LPCTSTR lpszEntry,	int nValue, BOOL bWriteFlush)
  {
    //Simply convert to a string and delegate to WriteString
    CString sValue;
    sValue.Format(_T("%d"), nValue);
		WriteString(lpszSection, lpszEntry, sValue, bWriteFlush);
  }

  void WriteString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue, BOOL bWriteFlush)
  {
	  if (lpszEntry == NULL) //delete the section node
	  {
      //Get the document node
      ATL::CComPtr<IXMLDOMNode> documentNode = GetDocumentNode(TRUE);

      //Also get the section node
      ATL::CComPtr<IXMLDOMNode> sectionNode = GetSectionNode(lpszSection, TRUE);

      //now try to delete the node
      HRESULT hr = documentNode->removeChild(sectionNode, NULL);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      m_bDirty = TRUE; //Set the dirty flag
	  }
	  else if (lpszValue == NULL) //Delete the entry node
	  {
      //Get the section node
      ATL::CComPtr<IXMLDOMNode> sectionNode = GetSectionNode(lpszSection, TRUE);

      //Also get the entry node
      ATL::CComPtr<IXMLDOMNode> entryNode = GetEntryNode(lpszSection, lpszEntry, TRUE);

      //now try to delete the node
      HRESULT hr = sectionNode->removeChild(entryNode, NULL);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      m_bDirty = TRUE; //Set the dirty flag
	  }
	  else
	  {
      //Get the entry node
      ATL::CComPtr<IXMLDOMNode> entryNode = GetEntryNode(lpszSection, lpszEntry, FALSE);

      //update the text of the entry node
      HRESULT hr = entryNode->put_text(CComBSTR(lpszValue));
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      m_bDirty = TRUE; //Set the dirty flag
    }

    //Now save the settings
    if (bWriteFlush)
      Flush();
  }

  void WriteBinary(LPCTSTR lpszSection, LPCTSTR lpszEntry, const BYTE* pData, DWORD dwBytes, BOOL bWriteFlush)
  {
	  //Allocate some heap memory for the data we will be writing out
	  ATL::CHeapPtr<TCHAR> dataBuffer;
	  if (!dataBuffer.Allocate((dwBytes*2) + 1))
	    ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);
  
	  //convert the data to write out to string format
	  DWORD i;
	  for (i=0; i<dwBytes; i++)
	  {
		  dataBuffer.m_pData[i*2] = static_cast<TCHAR>((pData[i] & 0x0F) + 'A'); //low nibble
		  dataBuffer.m_pData[i*2+1] = static_cast<TCHAR>(((pData[i] >> 4) & 0x0F) + 'A'); //high nibble
	  }
	  dataBuffer.m_pData[i*2] = 0;

    //Call the WriteString method to write out the data for us
    WriteString(lpszSection, lpszEntry, dataBuffer.m_pData, bWriteFlush);
  }

  void WriteStringArray(LPCTSTR lpszSection, LPCTSTR lpszEntry, const CStringArray& array, BOOL bWriteFlush)
  {
    //Work out the size of the buffer we will need
    DWORD dwSize = 0;
    INT_PTR nStrings = array.GetSize();
    for (INT_PTR i=0; i<nStrings; i++)
      dwSize += array.GetAt(i).GetLength() + 1; //1 extra for each NULL terminator

    //Need one second NULL for the double NULL at the end
    dwSize++;

    //Allocate the memory we want
    ATL::CHeapPtr<TCHAR> dataBuffer;
    if (!dataBuffer.Allocate(dwSize))
      ThrowWin32AppSettingsException(ERROR_OUTOFMEMORY);

    //Now copy the strings into the buffer
    int nCurOffset = 0;
    LPTSTR lpszString = dataBuffer.m_pData;
    for (INT_PTR i=0; i<nStrings; i++)
    {
      CString sText(array.GetAt(i));
      int nCurrentStringLength = sText.GetLength();
      _tcscpy_s(&lpszString[nCurOffset], nCurrentStringLength + 1, sText);
      nCurOffset += nCurrentStringLength;
      nCurOffset++;
    }
    //Don't forgot to doubly NULL terminate
    lpszString[nCurOffset] = _T('\0');

    //Call the WriteBinary method to write out the data for us
    WriteBinary(lpszSection, lpszEntry, reinterpret_cast<const BYTE*>(dataBuffer.m_pData), dwSize*sizeof(TCHAR), bWriteFlush);
  }

  void WriteSection(LPCTSTR lpszSection, const CStringArray& sectionEntries, BOOL bWriteFlush)
  {
    //Validate our parameters
	  ASSERT(lpszSection);

    //First nuke the registry key entirely (using the WriteString delete section functionality!)
    WriteString(lpszSection, NULL, NULL, FALSE);

    //now write out all the new entries
    INT_PTR nEntries = sectionEntries.GetSize();
    for (INT_PTR i=0; i<nEntries; i++)
    {
      CString sEntry(sectionEntries.GetAt(i));
      CString sName;
      CString sValue;
      int nSeparator = sEntry.Find(_T('='));
      if (nSeparator != -1)
      {
        sName = sEntry.Left(nSeparator);
        sValue = sEntry.Right(sEntry.GetLength() - 1 - nSeparator);
      }
      else
        sName = sEntry;

      WriteString(lpszSection, sName, sValue, FALSE);
    }
    
    //Now save the settings
    if (bWriteFlush)
      Flush();
  }
  
  void Flush()
  {
    //Save to disk if we are dirty
    if (m_bDirty && (m_XMLDOM != NULL))
    {
      //Pretty print the DOM if necessary
      if (m_bPrettyPrint)
      {
        //Get the document element
        ATL::CComPtr<IXMLDOMElement> documentElement;
        HRESULT hr = m_XMLDOM->get_documentElement(&documentElement);
        if (FAILED(hr))
          ThrowCOMAppSettingsException(hr);

        //Remove any existing indentation nodes
        ATL::CComPtr<IXMLDOMNode> documentElementNode(documentElement);
        RemoveIndentationNodes(documentElementNode);
     
        //Call PrettyPrint on the document element
        PrettyPrint(documentElementNode, _T(""), _T(" "));
      }
    
      HRESULT hr = m_XMLDOM->save(ATL::CComVariant(m_sXMLFile));
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      m_bDirty = FALSE; //Reset the dirty flag
    }
  }

protected:
//Helper functions
  ATL::CComPtr<IXMLDOMNode> GetDocumentNode(BOOL bReadOnly)
  {
    //Create and load the DOM
    CreateDOMIfNeccessary(bReadOnly);

    //get the root XML node
    ATL::CComPtr<IXMLDOMElement> documentElement;
    HRESULT hr = m_XMLDOM->get_documentElement(&documentElement);
    if (bReadOnly && FAILED(hr))
      ThrowCOMAppSettingsException(hr);
    else if (bReadOnly && (documentElement == NULL))
      ThrowWin32AppSettingsException(ERROR_NOT_FOUND);

    if (FAILED(hr) || (documentElement == NULL))
    {
      //Create the new section node
      ATL::CComPtr<IXMLDOMNode> rootNode;
      hr = m_XMLDOM->createNode(ATL::CComVariant(static_cast<long>(NODE_ELEMENT), VT_I4), CComBSTR(L"xml"), CComBSTR(L""), &rootNode);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      //Append the root node to the DOM
      ATL::CComPtr<IXMLDOMNode> newRootNode;
      hr = m_XMLDOM->appendChild(rootNode, &newRootNode);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      m_bDirty = TRUE; //set the dirty flag now that we have updated the DOM

      return newRootNode;
    }
    else
    {
      //Convert the IXMLDOMElement interface to a IXMLDOMNode interface
      ATL::CComPtr<IXMLDOMNode> documentElementNode(documentElement);
      return documentElementNode;
    }
  }

  ATL::CComPtr<IXMLDOMNode> GetSectionNode(LPCTSTR lpszSection, BOOL bReadOnly)
  {
    //For compatibility reasons, we allow a lpszSection of an empty string. Normally
    //this would not be allowed in XML.
    CString sSection(lpszSection);
    if (sSection.IsEmpty())
      sSection = _T("IAPPSettings_Empty");
  
    //Get the document node
    ATL::CComPtr<IXMLDOMNode> documentNode = GetDocumentNode(bReadOnly);

    //First try to use the existing node
    ATL::CComPtr<IXMLDOMNode> node;
    HRESULT hr = documentNode->selectSingleNode(CComBSTR(sSection), &node);
    if (bReadOnly && FAILED(hr))
      ThrowCOMAppSettingsException(hr);
    else if (bReadOnly && (node == NULL))
      ThrowWin32AppSettingsException(ERROR_NOT_FOUND);

    if (FAILED(hr) || node == NULL)
    {
      //Ok, we couldn't get the existing node, so create a new one from scratch
      ATL::CComPtr<IXMLDOMNode> sectionNode;
      hr = m_XMLDOM->createNode(ATL::CComVariant(static_cast<long>(NODE_ELEMENT), VT_I4), CComBSTR(sSection), CComBSTR(L""), &sectionNode);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      //Append the section node to the DOM
      ATL::CComPtr<IXMLDOMNode> newSectionNode;
      hr = documentNode->appendChild(sectionNode, &newSectionNode);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      m_bDirty = TRUE; //set the dirty flag now that we have updated the DOM

      return newSectionNode;
    }
    else
      return node;
  }

  ATL::CComPtr<IXMLDOMNode> GetEntryNode(LPCTSTR lpszSection, LPCTSTR lpszEntry, BOOL bReadOnly)
  {
    //Validate our parameters
	  ASSERT(lpszEntry);

    //Get the section node
    ATL::CComPtr<IXMLDOMNode> sectionNode = GetSectionNode(lpszSection, bReadOnly);

    //First try to use the existing node
    ATL::CComPtr<IXMLDOMNode> node;
    HRESULT hr = sectionNode->selectSingleNode(CComBSTR(lpszEntry), &node);
    if (bReadOnly && FAILED(hr))
      ThrowCOMAppSettingsException(hr);
    else if (bReadOnly && (node == NULL))
      ThrowWin32AppSettingsException(ERROR_NOT_FOUND);

    if (FAILED(hr) || node == NULL)
    {
      //Ok, we couldn't get the existing node, so create a new one from scratch
      hr = m_XMLDOM->createNode(ATL::CComVariant(static_cast<long>(NODE_ELEMENT), VT_I4), CComBSTR(lpszEntry), CComBSTR(L""), &node);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);
 
      //Finally append the new node to the DOM
      ATL::CComPtr<IXMLDOMNode> newNode;
      hr = sectionNode->appendChild(node, &newNode);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      m_bDirty = TRUE;

      return newNode;
    }
    else
      return node;
  }

  void CreateDOMIfNeccessary(BOOL bFailIfNotPresent)
  {
    //If the DOM document does not exist, then create it
    if (m_XMLDOM == NULL)
    {
      HRESULT hr = m_XMLDOM.CoCreateInstance(__uuidof(DOMDocument), NULL, CLSCTX_INPROC_SERVER);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);
      hr = m_XMLDOM->put_async(VARIANT_FALSE);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      //Validate our parameters
      ASSERT(m_sXMLFile.GetLength());

      ATL::CComVariant varXMLFile(m_sXMLFile);
      VARIANT_BOOL bSuccess;
      hr = m_XMLDOM->load(varXMLFile, &bSuccess);
      if (bFailIfNotPresent)
      {
        if (FAILED(hr))
          ThrowCOMAppSettingsException(hr);
        if (bSuccess != VARIANT_TRUE)
          ThrowCOMAppSettingsException(E_FAIL);
      }

      m_bDirty = FALSE; //Reset the dirty flag
    }
  }
 
  void RemoveIndentationNodes(ATL::CComPtr<IXMLDOMNode>& node)
  {
    //Iterate across all the children
    ATL::CComPtr<IXMLDOMNode> child;
    HRESULT hr = node->get_firstChild(&child);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);
    while (child)
    {
      //Recursively call ourselves
      RemoveIndentationNodes(child);
      
      //Get the node type
      DOMNodeType nodeType;
      hr = child->get_nodeType(&nodeType);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);

      //The value we use to determine if we should delete this node
      BOOL bDeleteThisNode = FALSE;
        
      //delete it if it is a text node and it looks like a indentation node we created using the "PrintPrint" method 
      if (nodeType == NODE_TEXT)
      {
        //Get the node value to see if it was one of our nodes which we use for indentation
        ATL::CComVariant varValue;
        hr = child->get_nodeValue(&varValue);
        if (SUCCEEDED(hr) && varValue.vt == VT_BSTR)
        {
          UINT nValueLen = SysStringLen(varValue.bstrVal);
          if (nValueLen >= 1)
          {
            bDeleteThisNode = TRUE;
            for (UINT i=0; (i<nValueLen) && bDeleteThisNode; i++)
              bDeleteThisNode = (varValue.bstrVal[i] == L'\r') || (varValue.bstrVal[i] == L'\n') || (varValue.bstrVal[i] == L' ');
          }
        }
      
        //Rememer the next sibling before we delete the current child
        if (bDeleteThisNode)
        {
          ATL::CComPtr<IXMLDOMNode> nextSibling;
          hr = child->get_nextSibling(&nextSibling);
          if (FAILED(hr))
            ThrowCOMAppSettingsException(hr);
        
          ATL::CComPtr<IXMLDOMNode> oldNode;
          hr = node->removeChild(child, &oldNode);
          if (FAILED(hr))
            ThrowCOMAppSettingsException(hr);
          
          //prepare for the next iteration
          child = nextSibling;  
        }
      }

      //Prepare for the next iteration
      if (!bDeleteThisNode)
      {
        ATL::CComPtr<IXMLDOMNode> tempChild;
        hr = child->get_nextSibling(&tempChild);
        if (FAILED(hr))
          ThrowCOMAppSettingsException(hr);
        child = tempChild;
      }
    }
  }
  
  void PrettyPrint(ATL::CComPtr<IXMLDOMNode>& node, const CString& sCurIndent, const CString& sIndent)
  {
    //Get the owner document
    ATL::CComPtr<IXMLDOMDocument> ownerDocument;
    HRESULT hr = node->get_ownerDocument(&ownerDocument);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);
  
    //We need to remember the last child in the loop
    ATL::CComPtr<IXMLDOMNode> lastChild;

    //Iterate across all the children
    ATL::CComPtr<IXMLDOMNode> child;
    hr = node->get_firstChild(&child);
    if (FAILED(hr))
      ThrowCOMAppSettingsException(hr);
    while (child)
    {
      //Get the child node type
      DOMNodeType nodeType;
      hr = child->get_nodeType(&nodeType);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);
        
      if (nodeType == NODE_ELEMENT)
      {
        //Recursively call ourselves
        PrettyPrint(child, sCurIndent + sIndent, sIndent);
      
        //Work out the the indent we are going to use
        CString sThisIndent(_T("\r\n"));
        sThisIndent += sCurIndent;
        sThisIndent += sIndent; 
        
        //Create the new text node
        ATL::CComPtr<IXMLDOMText> newNode;
        hr = ownerDocument->createTextNode(ATL::CComBSTR(sThisIndent), &newNode);
        if (FAILED(hr))
          ThrowCOMAppSettingsException(hr);
          
        //Add indentation before the current node  
        ATL::CComPtr<IXMLDOMNode> tempChild;
        hr = node->insertBefore(newNode, ATL::CComVariant(child), &tempChild);
        if (FAILED(hr))
          ThrowCOMAppSettingsException(hr);
          
        //remember the last child element node
        lastChild = child;
      }
      
      //Prepare for the next iteration
      ATL::CComPtr<IXMLDOMNode> tempChild;
      hr = child->get_nextSibling(&tempChild);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);
      child = tempChild;
    }
    
    //Add indentation after the last element node
    if (lastChild)    
    {
      CString sThisIndent(_T("\r\n"));
      sThisIndent += sCurIndent;
    
      //Create the new text node
      ATL::CComPtr<IXMLDOMText> newNode;
      hr = ownerDocument->createTextNode(CComBSTR(sThisIndent), &newNode);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);
        
      //Add the node
      ATL::CComPtr<IXMLDOMNode> tempChild;
      hr = node->appendChild(newNode, &tempChild);
      if (FAILED(hr))
        ThrowCOMAppSettingsException(hr);
    }
  }

//Member variables
  CString                       m_sXMLFile;     //The filename of the XML file to use
  ATL::CComPtr<IXMLDOMDocument> m_XMLDOM;       //the XML DOM for the file
  BOOL                          m_bDirty;       //Is a DOM save pending
  BOOL                          m_bWriteFlush;  //Should an immediate save be done any time a modification is made to the DOM
  BOOL                          m_bPrettyPrint; //Should the resultant XML file be "pretty printed" when saved
};

#endif //__APPSETTINGS_H__
