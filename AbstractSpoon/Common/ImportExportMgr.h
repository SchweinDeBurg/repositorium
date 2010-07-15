// Copyright (C) 2003-2005 AbstractSpoon Software.
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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

// ImportExportMgr.h: interface for the CImportExportMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMPORTEXPORTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
#define AFX_IMPORTEXPORTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ITaskList.h"
#include <afxtempl.h>

class IImportTasklist;
class IExportTasklist;

class CImportExportMgr
{
public:
	CImportExportMgr();
	virtual ~CImportExportMgr();

	virtual void Initialize();

	int GetNumImporters() const;
	int GetNumExporters() const;

	CString GetImporterMenuText(int nImporter) const;
	CString GetImporterFileExtension(int nImporter) const;
	BOOL ImporterHasFileExtension(int nImporter) const;
	CString GetImporterFileFilter(int nImporter) const;

	CString GetExporterMenuText(int nExporter) const;
	CString GetExporterFileExtension(int nExporter) const;
	BOOL ExporterHasFileExtension(int nExporter) const;
	CString GetExporterFileFilter(int nExporter) const;

	BOOL ImportTaskList(LPCTSTR szSrcFile, ITaskList* pDestTasks, int nByImporter) const;
	BOOL ExportTaskList(const ITaskList* pSrcTasks, LPCTSTR szDestFile, int nByExporter, BOOL bSilent) const;

	int FindImporter(LPCTSTR szFilePath);

protected:
	BOOL m_bInitialized;
	CArray<IImportTasklist*, IImportTasklist*> m_aImporters;
	CArray<IExportTasklist*, IExportTasklist*> m_aExporters;

protected:
	static CString& Clean(CString& sText);

};

#endif // !defined(AFX_IMPORTEXPORTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
