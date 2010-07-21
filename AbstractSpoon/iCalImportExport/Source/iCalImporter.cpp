// iCalImporter.cpp: implementation of the CiCalImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "iCalImportExportApp.h"
#include "iCalImporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CiCalImporter::CiCalImporter()
{

}

CiCalImporter::~CiCalImporter()
{

}

bool CiCalImporter::Import(const char* /*szSrcFilePath*/, ITaskList* /*pDestTaskFile*/)
{
//	HTASKITEM hTask = pDestTaskFile->NewTask("test");
//	pDestTaskFile->SetTaskComments(hTask, "comments");
//	pDestTaskFile->SetTaskPriority(hTask, 7);

	
	return true;
}
