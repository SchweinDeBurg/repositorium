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
	void Release() { delete this; }

	// caller must copy only
	const char* GetMenuText() { return "GanttProject"; }
	const char* GetFileFilter() { return "GanttProject Files (*.gan)|*.gan||"; }
	const char* GetFileExtension() { return "gan"; }

	bool Import(const char* szSrcFilePath, ITaskList* pDestTaskFile);

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
