/*
Module : W3MFCMimeManager.cpp
Purpose: Implementation for the Mime Manager classes
Created: PJN / 22-04-1999
History: PJN / 12-01-2007 1. Fixed a bug in CW3MFCRegistryMimeManager::GetMimeType where it did not correctly
                          handle an empty file extension value. Thanks to Alexey V. Leonov for reporting this
                          bug.
         PJN / 19-11-2007 1. CHttpMimeManager class has been renamed CW3MFCMimeManager
                          2. CRegistryHttpMimeManager class has been renamed CW3MFCRegistryMimeManager
                          3. CIniHttpMimeManager class has been renamed CW3MFCIniMimeManager

Copyright (c) 1999 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)  

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////// Includes /////////////////////////////////////////////////////

#include "stdafx.h"
#include "W3MFCMimeManager.h"


//////////////// Macros ///////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation ///////////////////////////////////////////////

CString CW3MFCRegistryMimeManager::GetMimeType(const CW3MFCRequest& request)
{
  //Get the extension of the local file the request maps to
  TCHAR pszExt[_MAX_EXT];
  _tsplitpath_s(request.m_sLocalFile, NULL, 0, NULL, 0, NULL, 0, pszExt, sizeof(pszExt)/sizeof(TCHAR));

  if (!_tcslen(pszExt))
  	return CString(_T("application/octet-stream"));

  //Validate our parameters
  CString sExtension(pszExt);
  ASSERT(sExtension.GetLength());
  ASSERT(sExtension.GetAt(0) == _T('.'));

  //Prevent the string arrays from being manipulated
  //by multiple threads at the one time
  CSingleLock sl(&m_CS, TRUE);

  //Make a local copy because we may be modifying it
  CString sTemp(sExtension);

  //Remove the leading "." if any
  if (sTemp.Find(_T('.')) == 0)
    sTemp = sTemp.Right(sTemp.GetLength() - 1);

  //Make it upper case
  sTemp.MakeUpper();

  //Lookup in the hash table first
  CString sMimeType;
  if (!m_sMimeMap.Lookup(sTemp, sMimeType))
  {
    //Open the specified key
    HKEY hItem;
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, sExtension, 0, KEY_READ, &hItem) == ERROR_SUCCESS)
    { 
      //Query the key for the content type
      TCHAR sPath[_MAX_PATH];
      DWORD dwSize = _MAX_PATH;
      DWORD dwType = REG_SZ;
      if (RegQueryValueEx(hItem, _T("Content Type"), NULL, &dwType, reinterpret_cast<LPBYTE>(sPath), &dwSize) == ERROR_SUCCESS)
      {
        sMimeType = sPath;

        //Add to the cache so that we do not have to do the registry lookup again
        m_sMimeMap.SetAt(sTemp, sMimeType);
      }
      else
        sMimeType = _T("application/octet-stream");

      //Don't forget to close our key
      RegCloseKey(hItem);
    }
  }

  return sMimeType;
}


void CW3MFCIniMimeManager::FullArraysFromMultiSZString(LPTSTR pszString)
{
  //First work out how many strings there are in the multi sz string
  int nStrings = 0;
  LPTSTR pszCurrentString = pszString;
  while (pszCurrentString[0] != _T('\0'))
  {
    ++nStrings;
    pszCurrentString += (_tcslen(pszCurrentString) + 1);
  }

  //Initialize the hash table
  m_sMimeMap.InitHashTable(7929); //7929 is prime number

  //Set each array element's value
  pszCurrentString = pszString;
  for (long i=1; i<=nStrings; i++)
  {
    CString sCurrentString(pszCurrentString);
    int nEquals = sCurrentString.Find(_T('='));
    if (nEquals != -1)
    {
      //Form the key and value to add to the hash table
      CString sKey = sCurrentString.Left(nEquals);
      sKey.MakeUpper();
      CString sValue = sCurrentString.Right(sCurrentString.GetLength() - nEquals - 1);

      //Add to the hash table
      m_sMimeMap.SetAt(sKey, sValue);
    }

    //Move on to the next string
    pszCurrentString += (_tcslen(pszCurrentString) + 1);
  }
}

int CW3MFCIniMimeManager::Initialize(const CString& sIniPath, const CString& sSection)
{
  DWORD dwSize = 1024;
  BOOL bSuccess = FALSE;
  while (!bSuccess)
  {
    //Allocate some heap memory for the SDK call
    LPTSTR pszKeyValues = new TCHAR[dwSize];

    //Call the SDK function
    DWORD dwRetreived = GetPrivateProfileSection(sSection, pszKeyValues, dwSize, sIniPath);
    if (dwRetreived == (dwSize - 2))
    {
      //Realloc the array by doubling its size ready for the next loop around
      delete [] pszKeyValues;
      dwSize *= 2;
    }
    else if (dwRetreived == 0)
    {
      //Tidy up the heap memory we have used
      delete [] pszKeyValues;

      bSuccess = TRUE;
    }
    else
    {
      //Flesh out our internal arrays from the multi sz string
      FullArraysFromMultiSZString(pszKeyValues);

      //Tidy up the heap memory we have used
      delete [] pszKeyValues;

      bSuccess = TRUE;
    }
  }

  return static_cast<int>(m_sMimeMap.GetCount());
}

CString CW3MFCIniMimeManager::GetMimeType(const CW3MFCRequest& request)
{
  //what will be the return value
  CString sMimeType(_T("application/octet-stream"));

  //Get the extension of the local file the request maps to
  TCHAR pszExt[_MAX_EXT];
  _tsplitpath_s(request.m_sLocalFile, NULL, 0, NULL, 0, NULL, 0, pszExt, sizeof(pszExt)/sizeof(TCHAR));
  if (_tcslen(pszExt))
  {
    //Validate our parameters
    CString sExtension(pszExt);
    ASSERT(sExtension.GetLength());
    ASSERT(sExtension.GetAt(0) == _T('.'));

    //Prevent the string arrays from being manipulated
    //by multiple threads at the one time
    CSingleLock sl(&m_CS, TRUE);

    //Make a local copy because we may be modifying it
    CString sTemp(sExtension);

    //Remove the leading "." if any
    if (sTemp.Find(_T('.')) == 0)
      sTemp = sTemp.Right(sTemp.GetLength() - 1);

    //Make it upper case
    sTemp.MakeUpper();

    //Lookup in the hash table
    m_sMimeMap.Lookup(sTemp, sMimeType);
  }

  return sMimeType;
}
