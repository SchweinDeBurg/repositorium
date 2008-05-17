// CJMemFile.cpp: implementation of Maxallion's CCJMemFile class.
//
// Copyright © 1999 Maxallion
// mailto:XFile@maxallion.8m.com
// ICQ# 32304418
//
// CXFile - Extended Memory File - Beta - w/o a lot of error checking
// - Is used like a normal CFile or CStdioFile or CMemFile object
// - String Functions : ReadString, WriteString
// - Loads physical files into memory on creation and saves them back to disk on destruction
// - Can duplicate itself to other CFile derived objects
// - Has a Search function
// - can be read-accessed like an array
//
// OVERLOADED OPERATORS:
// = Imports from another file or sets file Position
// += Appends another file
// [] reads a byte like an array
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 24/05/02 13:52 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJMemFile.cpp $
 *
 * $History: CJMemFile.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 24/05/02   Time: 13:52
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 3:06a
 * Updated in $/CodeJock/CJLibrary
 * Added UNICODE modifications.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 3:01a
 * Created in $/CodeJock/CJLibrary
 * Initial creation.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJMemFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCJMemFile::CCJMemFile( UINT nGrowBytes)
{
	// Modified default constructor

	m_File.m_hFile = NULL;
	m_bOpen = false;
	m_uiOpenFlags = 0;
	CMemFile::CMemFile(nGrowBytes);
}

CCJMemFile::CCJMemFile( BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes )
{
	// Modified default constructor

	m_File.m_hFile = NULL;
	m_bOpen = false;
	m_uiOpenFlags = 0;
	CMemFile::CMemFile(lpBuffer,nBufferSize, nGrowBytes );
}

CCJMemFile::CCJMemFile(LPCTSTR lpszFilename, UINT uiOpenFlags)
{
	// CFile compatible constructor
	
	m_File.m_hFile = NULL;
	m_bOpen = false;
	m_uiOpenFlags = uiOpenFlags;
	Open(lpszFilename,uiOpenFlags);
}

CCJMemFile::~CCJMemFile()
{
	if (m_bOpen)
		Close();
	m_File.m_hFile = NULL;
	m_uiOpenFlags = 0;
	CMemFile::~CMemFile();
}

// Implementation

BOOL CCJMemFile::ReadString(CString& rString)
{
	// Readstring Beta Version
	
	rString.Empty();
	char ch;

	bool bFound = false;
	while (GetPosition() < GetLength())
	{
		Read(&ch,1);

		bFound = (ch == '\x0D');
		if (bFound || (ch == '\x0'))
			break;
		else
			rString = rString + ch;
	}
	if (bFound)
	{
		Read(&ch,1);
		if (ch != '\x0A')
			Seek(-1,current);
	}

	if (rString.GetLength() > 0)
		return true;
	return false;
}

void CCJMemFile::WriteString( LPCTSTR lpsz )
{
	// WriteString Beta Version
	
	if (_tcslen(lpsz) > 0)
	{
		for (UINT iCount = 0; iCount < _tcslen(lpsz); iCount++)
			if ((lpsz[iCount] != '\x0D') && (lpsz[iCount] != '\x00'))
				Write(&lpsz[iCount],1);
	}
	char ch = '\x0D';
	Write(&ch,1);
	ch = '\x0A';
	Write(&ch,1);

}

bool CCJMemFile::Duplicate(CFile *fDuplicate)
{
	// Standard Copy Routine
	
	BYTE *pbData = new BYTE[1024];
	
	#if _MSC_VER >= 1300	// VC 7.0 (.NET)
		ULONGLONG dwPos = GetPosition();
	#else					// VCV 5.0/6.0
		DWORD dwPos = GetPosition();
	#endif

	SeekToBegin();
	fDuplicate->SetLength(0);
	while (GetPosition() < GetLength())
	{
		DWORD dwRead = Read(pbData,1024);
		fDuplicate->Write(pbData,dwRead);
	}
	Seek(dwPos,fDuplicate->begin);
	delete pbData;
	return true;
}

bool CCJMemFile::Duplicate(CString strDup)
{
	// Duplicate With CString compatibility
	
	CFile file(strDup,CFile::modeCreate | CFile::modeWrite);
	bool bResult = Duplicate(&file);
	file.Close();
	return bResult;
}

bool CCJMemFile::Open(CString strFilename, UINT uiOpenFlags,CFileException *pError)
{
	// CFile compatible Open

	if (m_File.m_hFile != NULL)
		return false;
	if (m_File.Open(strFilename,uiOpenFlags,pError))
	{
		m_uiOpenFlags = uiOpenFlags;
		Load();
		m_bOpen = true;
		return true;
	}
	else
		return false;
}

bool CCJMemFile::Save()
{
	if (m_File.m_hFile == NULL) 
		return false;
	CString str;
	
	// Check if the physical file has write access
	if (!(m_uiOpenFlags & modeWrite) && !(m_uiOpenFlags & modeReadWrite))
	{
		return false;
	}

	#if _MSC_VER >= 1300	// VC 7.0 (.NET)
		ULONGLONG dwPos = GetPosition();
	#else					// VCV 5.0/6.0
		DWORD dwPos = GetPosition();
	#endif

	DWORD dwLength = 1024;
	BYTE *pData = new BYTE[dwLength];
	if (pData == NULL)
		return false;
	SeekToBegin();
	m_File.SetLength(0);
	while (GetPosition() < GetLength())
	{
		DWORD dwRead = Read(pData,dwLength);
		m_File.Write(pData,dwRead);
	}
	Seek(dwPos,begin);
	delete pData;
	m_File.Flush();
	return true;
}

bool CCJMemFile::Load()
{
	if (m_File.m_hFile == NULL)
		return false;

	DWORD dwLength = 1024;
	BYTE *pData = new BYTE[dwLength];
	if (pData == NULL)
		return false;
	SeekToBegin();
	SetLength(0);
	m_File.SeekToBegin();
	while (m_File.GetPosition() < m_File.GetLength())
	{
		DWORD dwRead = m_File.Read(pData,dwLength);
		Write(pData,dwRead);
	}

	delete pData;
	return true;
}


void CCJMemFile::Close()
{
	if ((m_File.m_hFile != NULL) && m_bOpen)
	{	
		m_bOpen = false;
		Save();
		m_File.Close();
		m_File.m_hFile = NULL;
	}
}

void CCJMemFile::Flush()
{
	CMemFile::Flush();
	Save();
}

bool CCJMemFile::Discard()
{
	return Load();
}

bool CCJMemFile::Import(CFile *fImp)
{
	if ((fImp == NULL) || (fImp->m_hFile == NULL))
		return false;

	DWORD dwLength = 1024;
	BYTE *pData = new BYTE[dwLength];
	if (pData == NULL)
		return false;
	SeekToBegin();
	SetLength(0);
	fImp->SeekToBegin();
	while (fImp->GetPosition() < fImp->GetLength())
	{
		DWORD dwRead = fImp->Read(pData,dwLength);
		Write(pData,dwRead);
	}

	delete pData;
	return true;
}

bool CCJMemFile::Append(CFile *fApp)
{
	if ((fApp == NULL) || (fApp->m_hFile == NULL))
		return false;

	DWORD dwLength = 1024;
	BYTE *pData = new BYTE[dwLength];
	if (pData == NULL)
		return false;
	SeekToEnd();
	fApp->SeekToBegin();
	while (fApp->GetPosition() < fApp->GetLength())
	{
		DWORD dwRead = fApp->Read(pData,dwLength);
		Write(pData,dwRead);
	}

	delete pData;
	return true;
}


#if _MSC_VER >= 1300	// VC 7.0 (.NET)

	ULONGLONG CCJMemFile::Insert(CString strSrc, LONGLONG dwSourcePos, LONGLONG dwDestPos, LONGLONG dwBytes)
	{
		CFile file(strSrc, CFile::modeRead);

		ULONGLONG dwResult = Insert(&file, dwSourcePos, dwDestPos, dwBytes);
		file.Close();

		return dwResult;
	}

	ULONGLONG CCJMemFile::Insert(CFile *fSrc, LONGLONG dwSourcePos, LONGLONG dwDestPos, LONGLONG dwBytes)
	{
		if (dwBytes == 0)
			return 0;

		CCJMemFile fPreIns;
		CCJMemFile fIns;
		CCJMemFile fPostIns;
		CCJMemFile fComplete;

		Extract(&fPreIns,0,dwDestPos); // Extract Data BEFORE insertion point
		Extract(&fPostIns,dwDestPos,GetLength() - dwDestPos); // Extract Data AFTER intertion point

		// Extract the "to be inserted" Data from the (fSrc) Source file
		{
			LONGLONG dwLength = 1024;
			LONGLONG dwCounter = 0;

			ASSERT(dwLength <= UINT_MAX);
			BYTE *pData = new BYTE[static_cast<UINT>(dwLength)];
			
			if (pData == NULL)
				return 0;

			fSrc->Seek(dwSourcePos, CFile::begin);
		
			while ((( (LONGLONG)fSrc->GetPosition() - dwSourcePos) < dwBytes) && (fSrc->GetPosition() < fSrc->GetLength()) )
			{
				UINT dwRead = fSrc->Read(pData, static_cast<UINT>(dwLength));

				if ( (dwCounter + (LONGLONG)dwRead) > (dwSourcePos + dwBytes) )
				{
					dwRead = static_cast<UINT>(dwBytes - dwCounter);
				}
				fIns.Write(pData, dwRead);
				dwCounter += dwRead;
			}
			delete pData;
		} // Finished Extraction


		fComplete += &fPreIns; // Append BEFORE Data
		fComplete += &fIns; // Append the "to be inserted" Data
		fComplete += &fPostIns;  // Append the AFTER Data

		*this = &fComplete; // Copy completed File

		return fIns.GetLength();
	}


	ULONGLONG CCJMemFile::Extract(CFile *fDest, LONGLONG dwStartPos, LONGLONG dwBytes)
	{
		if (dwBytes == 0)
			return 0;

		ULONGLONG dwPos = GetPosition();
		LONGLONG dwLength = 1024;
		LONGLONG dwCounter = 0;

		ASSERT(dwLength <= UINT_MAX);
		BYTE *pData = new BYTE[static_cast<UINT>(dwLength)];

		if (pData == NULL)
			return 0;
		Seek(dwStartPos,begin);
		
		while ((( (LONGLONG)GetPosition() - dwStartPos) < dwBytes) && (GetPosition() < GetLength()) )
		{
			LONGLONG dwRead = Read(pData, static_cast<UINT>(dwLength));
			if (dwCounter + dwRead > dwStartPos + dwBytes)
				dwRead = dwBytes - dwCounter;
			fDest->Write(pData, static_cast<UINT>(dwRead));
			dwCounter += dwRead;
		}
		Seek(dwPos,begin);
		delete pData;
		fDest->Flush();
		return dwCounter;

	}

	ULONGLONG CCJMemFile::Extract(CString strDest, LONGLONG dwStartPos, LONGLONG dwBytes)
	{
		CFile file(strDest,CFile::modeCreate | CFile::modeWrite);
		ULONGLONG dwResult = Extract(&file,dwStartPos,dwBytes);
		file.Close();
		return dwResult;
	}

#else					// VC 5.0/6.0

	DWORD CCJMemFile::Insert(CString strSrc, DWORD dwSourcePos, DWORD dwDestPos, DWORD dwBytes)
	{
		CFile file(strSrc,CFile::modeRead);
		DWORD dwResult = Insert(&file, dwSourcePos, dwDestPos, dwBytes);
		file.Close();
		return dwResult;
	}

	DWORD CCJMemFile::Insert(CFile *fSrc, DWORD dwSourcePos, DWORD dwDestPos, DWORD dwBytes)
	{
		if (dwBytes == 0)
			return 0;

		CCJMemFile fPreIns;
		CCJMemFile fIns;
		CCJMemFile fPostIns;
		CCJMemFile fComplete;

		Extract(&fPreIns,0,dwDestPos); // Extract Data BEFORE insertion point
		Extract(&fPostIns,dwDestPos,GetLength() - dwDestPos); // Extract Data AFTER intertion point

		// Extract the "to be inserted" Data from the (fSrc) Source file
		{
			DWORD dwLength = 1024;
			DWORD dwCounter = 0;
			BYTE *pData = new BYTE[dwLength];
			if (pData == NULL)
				return 0;
			fSrc->Seek(dwSourcePos,begin);
		
			while (((fSrc->GetPosition() - dwSourcePos) < dwBytes) && (fSrc->GetPosition() < fSrc->GetLength()) )
			{
				DWORD dwRead = fSrc->Read(pData,dwLength);
				if (dwCounter + dwRead > dwSourcePos + dwBytes)
					dwRead = dwBytes - dwCounter;
				fIns.Write(pData,dwRead);
				dwCounter += dwRead;
			}
			delete pData;
		} // Finished Extraction


		fComplete += &fPreIns; // Append BEFORE Data
		fComplete += &fIns; // Append the "to be inserted" Data
		fComplete += &fPostIns;  // Append the AFTER Data

		*this = &fComplete; // Copy completed File

		return fIns.GetLength();
	}


	DWORD CCJMemFile::Extract(CFile *fDest, DWORD dwStartPos, DWORD dwBytes)
	{
		if (dwBytes == 0)
			return 0;
		DWORD dwPos = GetPosition();
		DWORD dwLength = 1024;
		DWORD dwCounter = 0;
		BYTE *pData = new BYTE[dwLength];
		if (pData == NULL)
			return 0;
		Seek(dwStartPos,begin);
		
		while (((GetPosition() - dwStartPos) < dwBytes) && (GetPosition() < GetLength()) )
		{
			DWORD dwRead = Read(pData,dwLength);
			if (dwCounter + dwRead > dwStartPos + dwBytes)
				dwRead = dwBytes - dwCounter;
			fDest->Write(pData,dwRead);
			dwCounter += dwRead;
		}
		Seek(dwPos,begin);
		delete pData;
		fDest->Flush();
		return dwCounter;

	}

	DWORD CCJMemFile::Extract(CString strDest, DWORD dwStartPos, DWORD dwBytes)
	{
		CFile file(strDest,CFile::modeCreate | CFile::modeWrite);
		DWORD dwResult = Extract(&file,dwStartPos,dwBytes);
		file.Close();
		return dwResult;
	}

#endif

// Overloaded Operators

void CCJMemFile::operator =(CFile *fDup)
{
	Import(fDup);
}

void CCJMemFile::operator =(CString strDup)
{
	CFile file;
	if (file.Open(strDup,CFile::modeRead))
	{
		Import(&file);
		file.Close();
	}
}

void CCJMemFile::operator =(DWORD dwFilePos)
{
	Seek(dwFilePos,begin);
}


void CCJMemFile::operator +=(CFile *fApp)
{
	Append(fApp);
}

void CCJMemFile::operator +=(CString strApp)
{
	CFile file;
	if (file.Open(strApp,CFile::modeRead))
	{
		Append(&file);
		file.Close();
	}
}

// Indexing Operators

BYTE CCJMemFile::operator [](DWORD dwFilePos)
{
	if (dwFilePos > GetLength() -1)
		return 0;

	BYTE bTemp;
	Seek(dwFilePos,begin);
	Read(&bTemp,sizeof(BYTE));
	return bTemp;
}

#if (_MFC_VER < 0x0700)
LONG CCJMemFile::FindData(void* pData, DWORD dwDataLen, LONG lStartPos)
#else
LONGLONG CCJMemFile::FindData(void* pData, DWORD dwDataLen, LONG lStartPos)
#endif   // _MFC_VER
{
	BYTE* pBytes = (BYTE*)pData;
	
	
	Seek(lStartPos,begin);
	
	bool bFoundAll = false;
#if (_MFC_VER < 0x0700)
	LONG lPos = -1;
#else
	LONGLONG lPos = -1;
#endif   // _MFC_VER

	while ((!bFoundAll) && (GetPosition() < GetLength()))
	{
		bool bFoundFirst = false;
	
		while ((!bFoundFirst) && (GetPosition() < GetLength()))
		{
			lPos = GetPosition();
			BYTE read;
			Read(&read,1);
			if (read != pBytes[0])
				continue;
		
			bFoundFirst = true;
			break;
		}

		//Hier Die Restlichen Bytes (dwDataLen -1) vergleichen

		DWORD i = 1;
		bool bFound = true;

		while ((i < dwDataLen) && (GetPosition() < GetLength()) && bFoundFirst)
		{
			BYTE read;
			Read(&read,1);
			if (read == pBytes[i++])
				continue;

			bFound = false;
			Seek(lPos+1,begin);
			break;
		}

		bFoundAll = bFound && bFoundFirst;
	}

	lPos = bFoundAll ? lPos : -1;
	return lPos;
}
