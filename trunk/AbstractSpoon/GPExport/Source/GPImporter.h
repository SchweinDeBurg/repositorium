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

// GPImporter.h: interface for the CGPImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GPIMPORTER_H__7B3A9934_69F0_46D5_88B4_C84D715772FC__INCLUDED_)
#define AFX_GPIMPORTER_H__7B3A9934_69F0_46D5_88B4_C84D715772FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

#include <afxtempl.h>

class CXmlItem;

class CGPImporter : public IImportTasklist
{
public:
	CGPImporter();
	virtual ~CGPImporter();

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

	bool Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile);

protected:
	CMap<int, int, CString, CString&> m_mapResources;
	CMap<int, int, HTASKITEM, HTASKITEM&> m_mapTasks;

protected:
	bool ImportTask(const CXmlItem* pXISrcTask, ITaskList7* pDestTaskFile, HTASKITEM htDestParent);
	void BuildResourceMap(const CXmlItem* pXISrcPrj);
//	void FixupDependencies(ITaskList7* pDestTaskFile, HTASKITEM htTask);
	void FixupResourceAllocations(const CXmlItem* pXISrcPrj, ITaskList7* pDestTaskFile);
};

#endif // !defined(AFX_GPIMPORTER_H__7B3A9934_69F0_46D5_88B4_C84D715772FC__INCLUDED_)
