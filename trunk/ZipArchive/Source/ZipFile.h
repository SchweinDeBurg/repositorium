////////////////////////////////////////////////////////////////////////////////
// $RCSfile: ZipFile.h,v $
// $Revision: 1.3 $ $Name:  $
// $Date: 2005/02/14 08:09:54 $ $Author: Tadeusz Dracz $
////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000-2005 by Tadeusz Dracz (http://www.artpol-software.com/)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// For the licensing details see the file License.txt
////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ZIPFILE_H__80609DE0_2C6D_4C94_A90C_0BE34A50C769__INCLUDED_)
#define AFX_ZIPFILE_H__80609DE0_2C6D_4C94_A90C_0BE34A50C769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ZipAbstractFile.h"
#include "ZipExport.h"

#if defined(__INTEL_COMPILER)
// warning #69: integer conversion resulted in truncation
#pragma warning(disable: 69)
// warning #1125: function is hidden by another
#pragma warning(disable: 1125)
#endif	// __INTEL_COMPILER

class ZIP_API CZipFile :public CZipAbstractFile, public CFile
{
public:
	DECLARE_DYNAMIC(CZipFile)
	void Flush(){CFile::Flush();}
	ZIP_ULONGLONG GetPosition() const {return CFile::GetPosition() ;}
	CZipString GetFilePath() const
	{		
		try
		{
			// it throws an exception when working on an offline file
			return CFile::GetFilePath();
		}
		catch (CException* e)
		{
			e->Delete();
			return this->m_strFileName;
		}
	}
	void SetLength(ZIP_ULONGLONG nNewLen) {CFile::SetLength(nNewLen);}
	UINT Read(void *lpBuf, UINT nCount){return CFile::Read(lpBuf, nCount);}
	void Write(const void* lpBuf, UINT nCount){CFile::Write(lpBuf, nCount);}
	ZIP_ULONGLONG Seek(ZIP_LONGLONG lOff , int nFrom){return CFile::Seek(lOff, nFrom);}
	ZIP_ULONGLONG GetLength() const {return CFile::GetLength();}
	bool Open( LPCTSTR lpszFileName, UINT nOpenFlags, bool bThrowExc)
	{
		CFileException* e = new CFileException;
		bool bRet = CFile::Open(lpszFileName, nOpenFlags, e) != 0;
		if (!bRet && bThrowExc)
			throw e;
		e->Delete();
		return bRet;

	}
	CZipFile();
	bool IsClosed() const 
	{
		return m_hFile == CFile::hFileNull;
	}


	CZipFile( LPCTSTR lpszFileName, UINT nOpenFlags ):CFile(lpszFileName, nOpenFlags)
	{
	}
	void Close( )
	{
 		if (!IsClosed())
			CFile::Close();
	}
	operator HANDLE();
	virtual ~CZipFile();

};

#if defined(__INTEL_COMPILER)
#pragma warning(default: 69 1125)
#endif	// __INTEL_COMPILER

#endif // !defined(AFX_ZIPFILE_H__80609DE0_2C6D_4C94_A90C_0BE34A50C769__INCLUDED_)
