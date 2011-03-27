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

// GPExporter.h: interface for the CGPExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_)
#define AFX_GPEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

#include <afxtempl.h>

class CXmlItem;

class CGPExporter : public IExportTasklist
{
public:
	CGPExporter();
	virtual ~CGPExporter();

	// interface implementation
	void Release()
	{
		delete this;
	}

	// caller must copy only
	const char* GetMenuText()
	{
		return "GanttProject";
	}
	const char* GetFileFilter()
	{
		return "GanttProject Files (*.gan)|*.gan||";
	}
	const char* GetFileExtension()
	{
		return "gan";
	}

	bool Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent);

protected:
	CMap<CString, LPCTSTR, int, int&> m_mapResources;

protected:
	bool ExportTask(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestParent, CXmlItem* pXIAllocations);
	void BuildResourceMap(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pDestPrj);
	void ExportResources(const ITaskList7* pSrcTaskFile, CXmlItem* pDestPrj);
	void SetupDisplay(CXmlItem* pDestPrj);
	void SetupCalendar(CXmlItem* pDestPrj);
};

#endif // !defined(AFX_GPEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_)
