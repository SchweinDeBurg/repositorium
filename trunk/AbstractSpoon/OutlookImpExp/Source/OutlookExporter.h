// OutlookExporter.h: interface for the COutlookExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTLOOKEXPORTER_H__CA0FFD07_69C8_4A9F_A453_41674A63BF4C__INCLUDED_)
#define AFX_OUTLOOKEXPORTER_H__CA0FFD07_69C8_4A9F_A453_41674A63BF4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"

class COutlookExporter : public IExportTasklist    
{
public:
	COutlookExporter();
	virtual ~COutlookExporter();

	void Release() { delete this; }
	const char* GetMenuText() { return "Microsoft Outlook"; }
	const char* GetFileFilter() { return NULL; }
	const char* GetFileExtension() { return NULL; }

	virtual bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath);

};

#endif // !defined(AFX_OUTLOOKEXPORTER_H__CA0FFD07_69C8_4A9F_A453_41674A63BF4C__INCLUDED_)
