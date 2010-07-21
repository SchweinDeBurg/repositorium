// iCalExporter.h: interface for the CiCalExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_)
#define AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

class CiCalExporter : public IExportTasklist  
{
public:
	CiCalExporter();
	virtual ~CiCalExporter();

	// interface implementation
	void Release() { delete this; }

	// caller must copy only
	const char* GetMenuText() { return "iCalendar"; }
	const char* GetFileFilter() { return "iCalendar Files (*.ics)|*.ics||"; }
	const char* GetFileExtension() { return "ics"; }

	bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL bSilent);

protected:
	void ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, const CString& sParentUID, CStdioFile& fileOut);
	void __cdecl WriteString(CStdioFile& fileOut, LPCTSTR lpszFormat, ...);
};

#endif // !defined(AFX_ICALEXPORTER_H__5CDCA505_6C5C_4342_8F80_67368C7EE6A5__INCLUDED_)
