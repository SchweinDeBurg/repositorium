// FMindImporter.h: interface for the CFMindImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FMINDIMPORTER_H__48AE3CCE_E042_432D_B5DB_D7E310CF99CE__INCLUDED_)
#define AFX_FMINDIMPORTER_H__48AE3CCE_E042_432D_B5DB_D7E310CF99CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"
#include "../../Common/XmlFile.h"

class CFMindImporter : public IImportTasklist  
{
public:
	CFMindImporter();
	virtual ~CFMindImporter();
	// interface implementation
	void Release() { delete this; }
	
	// caller must copy only
	const char* GetMenuText() { return "FreeMind"; }
	const char* GetFileFilter() { return "FreeMind Files (*.mm)|*.mm||"; }
	const char* GetFileExtension() { return "mm"; }
	
	bool Import(const char* szSrcFilePath, ITaskList* pDestTaskFile);

protected:
	bool ImportTask(const CXmlItem* pFMTask, ITaskList7* pDestTaskFile, HTASKITEM hParent) const;
	long GetColor(const CXmlItem* pFMTask, LPCTSTR szColorField) const;
	long GetDate(const CXmlItem* pFMTask, LPCTSTR szColorField) const;
	CString GetAttribValueS(const CXmlItem* pFMTask , const char* szAttribName) const;
	int GetAttribValueI(const CXmlItem* pFMTask , const char* szAttribName) const;
	bool GetAttribValueB(const CXmlItem* pFMTask , const char* szAttribName) const;
	double GetAttribValueD(const CXmlItem* pFMTask , const char* szAttribName) const;
	CString GetTaskRichContent(const CXmlItem* pFMTask , const char* szRichType) const;

};

#endif // !defined(AFX_FMINDIMPORTER_H__48AE3CCE_E042_432D_B5DB_D7E310CF99CE__INCLUDED_)
