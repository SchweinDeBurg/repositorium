// iCalImporter.h: interface for the CiCalImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICALIMPORTER_H__36D6AF21_80EA_4361_85E9_B9BCDB38F913__INCLUDED_)
#define AFX_ICALIMPORTER_H__36D6AF21_80EA_4361_85E9_B9BCDB38F913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

class CiCalImporter : public IImportTasklist  
{
public:
	CiCalImporter();
	virtual ~CiCalImporter();

	// interface implementation
	void Release() { delete this; }

	// caller must copy only
	const char* GetMenuText() { return "iCalendar"; }
	const char* GetFileFilter() { return "iCalendar Files (*.ics)|*.ics||"; }
	const char* GetFileExtension() { return "ics"; }

	bool Import(const char* szSrcFilePath, ITaskList* pDestTaskFile);
};

#endif // !defined(AFX_ICALIMPORTER_H__36D6AF21_80EA_4361_85E9_B9BCDB38F913__INCLUDED_)
