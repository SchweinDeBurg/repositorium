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
	void Release() { delete this; }

	// caller must copy only
	const char* GetMenuText() { return "GanttProject"; }
	const char* GetFileFilter() { return "GanttProject Files (*.gan)|*.gan||"; }
	const char* GetFileExtension() { return "gan"; }

	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL bSilent);

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
