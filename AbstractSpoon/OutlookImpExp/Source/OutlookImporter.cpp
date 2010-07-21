// OutlookImporter.cpp: implementation of the COutlookImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OutlookImpExpApp.h"
#include "OutlookImporter.h"
#include "OutlookImportDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COutlookImporter::COutlookImporter()
{

}

COutlookImporter::~COutlookImporter()
{

}

bool COutlookImporter::Import(const char* /*szSrcFilePath*/, ITaskList* pDestTaskFile)
{
	COutlookImportDlg dlg;

	return (dlg.ImportTasks(pDestTaskFile) == TRUE);
}
