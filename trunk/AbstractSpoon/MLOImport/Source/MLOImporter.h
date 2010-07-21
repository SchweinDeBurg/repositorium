// MLOImporter.h: interface for the CMLOImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MLOIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_)
#define AFX_MLOIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

class CXmlItem;

class CMLOImporter : public IImportTasklist  
{
public:
	CMLOImporter();
	virtual ~CMLOImporter();

   void Release() { delete this; }
   const char* GetMenuText() { return "My Life Organized (MLO)"; }
	const char* GetFileFilter() { return "MLO Task Files (*.xml)|*.xml||"; }
	const char* GetFileExtension() { return "xml"; }

	bool Import(const char* szSrcFilePath, ITaskList* pDestTaskFile);

protected:
	bool ImportTask(const CXmlItem* pXIMLOTask, ITaskList5* pDestTaskFile, HTASKITEM hParent) const;
	time_t GetDate(const CXmlItem* pXIMLOTask, LPCTSTR szDateField) const;

};

#endif // !defined(AFX_MLOIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_)
