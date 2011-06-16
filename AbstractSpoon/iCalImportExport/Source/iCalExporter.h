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
// - merged with ToDoList version 6.2.2-6.2.4 sources
//*****************************************************************************

// iCalExporter.h: interface for the CiCalExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_)
#define AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

class CiCalExporter : public IExportTasklist
{
public:
	CiCalExporter();
	virtual ~CiCalExporter();

	// interface implementation
	void Release()
	{
		delete this;
	}

	// caller must copy only
	const char* GetMenuText()
	{
		return "iCalendar";
	}
	const char* GetFileFilter()
	{
		return "iCalendar Files (*.ics)|*.ics||";
	}
	const char* GetFileExtension()
	{
		return "ics";
	}

	bool Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);
	bool Export(const IMultiTaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);

protected:
	void ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, const CString& sParentUID, CStdioFile& fileOut);
	void __cdecl WriteString(CStdioFile& fileOut, LPCTSTR lpszFormat, ...);

	static CString FormatDateTime(LPCTSTR szType, const COleDateTime& date);
};

#endif // !defined(AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_)
