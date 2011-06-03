// Copyright (C) 2003-2011 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// PlainTextExporter.h: interface for the CPlainTextExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAINTEXTEXPORTER_H__69016DB3_5424_49DF_A877_962E83BC6E6B__INCLUDED_)
#define AFX_PLAINTEXTEXPORTER_H__69016DB3_5424_49DF_A877_962E83BC6E6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h" // for HTASKITEM

class CPlainTextExporter : public IExportTasklist
{
public:
	CPlainTextExporter();
	virtual ~CPlainTextExporter();

	void Release()
	{
		delete this;
	}
	const char* GetMenuText()
	{
		return "Outline";
	}
	const char* GetFileFilter()
	{
		return "Text Files (*.txt)|*.txt||";
	}
	const char* GetFileExtension()
	{
		return "txt";
	}

	virtual bool Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);
	virtual bool Export(const IMultiTaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);

protected:
	CString INDENT;
	BOOL WANTPROJECT;

protected:
	bool InitConsts(BOOL bSilent);
	void ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, CStdioFile& fileOut, int nDepth);
};

#endif // !defined(AFX_PLAINTEXTEXPORTER_H__69016DB3_5424_49DF_A877_962E83BC6E6B__INCLUDED_)
