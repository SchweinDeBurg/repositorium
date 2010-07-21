// MLOExporter.h: interface for the CMLOExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MLOEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_)
#define AFX_MLOEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

#include <afxtempl.h>

class CXmlItem;

class CMLOExporter : public IExportTasklist  
{
public:
	CMLOExporter();
	virtual ~CMLOExporter();

	// interface implementation
	void Release() { delete this; }

	// caller must copy only
   const char* GetMenuText() { return "My Life Organized (MLO)"; }
	const char* GetFileFilter() { return "MLO Task Files (*.xml)|*.xml||"; }
	const char* GetFileExtension() { return "xml"; }

	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL bSilent);

protected:

protected:
	bool ExportTask(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CXmlItem* pXIDestParent);

   void BuildPlacesMap(const ITaskList7* pSrcTaskFile, HTASKITEM hTask, CMapStringToString& mapPlaces);
	void ExportPlaces(const ITaskList7* pSrcTaskFile, CXmlItem* pDestPrj);
};

#endif // !defined(AFX_MLOEXPORTER_H__F588E6B1_3646_4994_99A2_4223FDDA1A31__INCLUDED_)
