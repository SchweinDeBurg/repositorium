// PlainTextImporter.h: interface for the CPlainTextImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAINTEXTIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_)
#define AFX_PLAINTEXTIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"

class CPlainTextImporter : public IImportTasklist  
{
public:
	CPlainTextImporter();
	virtual ~CPlainTextImporter();
	
	void Release() { delete this; }
	const char* GetMenuText() { return "Outline"; }
	const char* GetFileFilter() { return "Text Files (*.txt)|*.txt||"; }
	const char* GetFileExtension() { return "txt"; }
	
	bool Import(const char* szSrcFilePath, ITaskList* pDestTaskFile);

protected:
	CString INDENT;
	int ROOTDEPTH;

protected:
	int GetDepth(const CString& sLine); // returns the indent
	BOOL GetTitleComments(const CString& sLine, CString& sTitle, CString& sComments);
};

#endif // !defined(AFX_PLAINTEXTIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_)
