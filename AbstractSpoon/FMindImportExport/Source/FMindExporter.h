// FMindExporter.h: interface for the CFMindExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FMINDEXPORTER_H__49174123_010C_4E73_B1BF_5440AE766D46__INCLUDED_)
#define AFX_FMINDEXPORTER_H__49174123_010C_4E73_B1BF_5440AE766D46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITasklist.h"

class CXmlItem;

class CFMindExporter : public IExportTasklist  
{
public:
	CFMindExporter();
	virtual ~CFMindExporter();
	// interface implementation
	void Release() { delete this; }
	
	// caller must copy only
	const char* GetMenuText() { return "FreeMind"; }
	const char* GetFileFilter() { return "FreeMind Files (*.mm)|*.mm||"; }
	const char* GetFileExtension() { return "mm"; }
	
	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL bSilent);
	
protected:
	void ExportTask(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestParent , int LEVEL);
	CString FormatDate(time_t tDate);

	void AddTaskAllocTo(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestItem);
	void AddTaskCategories(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestItem);
	void AddTaskDependencies(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestItem);
};

#endif // !defined(AFX_FMINDEXPORTER_H__49174123_010C_4E73_B1BF_5440AE766D46__INCLUDED_)
