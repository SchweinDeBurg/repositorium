// OutlookExporter.cpp: implementation of the COutlookExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "OutlookImpExpApp.h"
#include "OutlookExporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COutlookExporter::COutlookExporter()
{

}

COutlookExporter::~COutlookExporter()
{

}

bool COutlookExporter::Export(const ITaskList* /*pSrcTaskFile*/, const char* /*szDestFilePath*/)
{
	return false;
}
