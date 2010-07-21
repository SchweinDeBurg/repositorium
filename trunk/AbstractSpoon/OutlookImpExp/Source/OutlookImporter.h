// OutlookImporter.h: interface for the COutlookImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTLOOKIMPORTER_H__C35841C7_32A6_4705_A061_24A85C3A0223__INCLUDED_)
#define AFX_OUTLOOKIMPORTER_H__C35841C7_32A6_4705_A061_24A85C3A0223__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"

class COutlookImporter : public IImportTasklist  
{
public:
	COutlookImporter();
	virtual ~COutlookImporter();

	void Release() { delete this; }
	const char* GetMenuText() { return "Microsoft Outlook"; }
	const char* GetFileFilter() { return NULL; }
	const char* GetFileExtension() { return NULL; }
	
	bool Import(const char* szSrcFilePath, ITaskList* pDestTaskFile);


};

#endif // !defined(AFX_OUTLOOKIMPORTER_H__C35841C7_32A6_4705_A061_24A85C3A0223__INCLUDED_)
