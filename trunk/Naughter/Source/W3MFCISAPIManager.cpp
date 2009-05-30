/*
Module : W3MFCISAPIManager.cpp
Purpose: Implementation for the Mime Manager classes
Created: PJN / 23-02-2003
History: PJN / 19-11-2007 1. Updated copyright details.
                          2. CIniISAPIManager class has been renamed to CW3MFCIniISAPIManager

Copyright (c) 2003 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com) 

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
#include "W3MFCISAPIManager.h"


//////////////// Macros ///////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation ///////////////////////////////////////////////

CW3MFCIniISAPIManager::CW3MFCIniISAPIManager()
{
}

void CW3MFCIniISAPIManager::FullArraysFromMultiSZString(LPTSTR pszString)
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
  m_sExtensionMap.InitHashTable(7929); //7929 is prime

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
      m_sExtensionMap.SetAt(sKey, sValue);
    }

    //Move on to the next string
    pszCurrentString += (_tcslen(pszCurrentString) + 1);
  }
}

int CW3MFCIniISAPIManager::Initialize(const CString& sIniPath, const CString& sSection)
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

  return static_cast<int>(m_sExtensionMap.GetCount());
}

CString CW3MFCIniISAPIManager::GetISAPIExtension(const CW3MFCRequest& request)
{
  //What will be the return value
  CString sFile;

  //Get the extension of the URL of the request
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
    if (!m_sExtensionMap.Lookup(sTemp, sFile))
    {
      //if we could not find it in the map then
      //return the local file itself (if its
      //extension is a DLL)
      if (sExtension.CompareNoCase(_T(".DLL")) == 0)
        sFile = request.m_sLocalFile;
    }
  }

  return sFile;
}
