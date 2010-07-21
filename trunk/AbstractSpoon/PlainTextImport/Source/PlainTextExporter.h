// PlainTextExporter.h: interface for the CPlainTextExporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAINTEXTEXPORTER_H__69016DB3_5424_49DF_A877_962E83BC6E6B__INCLUDED_)
#define AFX_PLAINTEXTEXPORTER_H__69016DB3_5424_49DF_A877_962E83BC6E6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h" // for HTASKITEM

class CPlainTextExporter : public IExportTasklist  
{
public:
	CPlainTextExporter();
	virtual ~CPlainTextExporter();

	void Release() { delete this; }
	const char* GetMenuText() { return "Outline"; }
	const char* GetFileFilter() { return "Text Files (*.txt)|*.txt||"; }
	const char* GetFileExtension() { return "txt"; }
	
	virtual bool Export(const ITaskList* pSrcTaskFile, const char* szDestFilePath, BOOL bSilent);

protected:
	CString INDENT;

protected:
	void ExportTask(const ITaskList* pSrcTaskFile, HTASKITEM hTask, 
					CStdioFile& fileOut, int nDepth);
};

#endif // !defined(AFX_PLAINTEXTEXPORTER_H__69016DB3_5424_49DF_A877_962E83BC6E6B__INCLUDED_)
