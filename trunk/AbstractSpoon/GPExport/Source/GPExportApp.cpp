// GPExportApp.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
#include "GPExportApp.h"
#include "GPExporter.h"
#include "GPImporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;  
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CGPExportApp

BEGIN_MESSAGE_MAP(CGPExportApp, CWinApp)
	//{{AFX_MSG_MAP(CGPExportApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGPExportApp construction

CGPExportApp::CGPExportApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CGPExportApp object

CGPExportApp theApp;

DLL_DECLSPEC IExportTasklist* CreateExportInterface()
{
	return new CGPExporter;
}

DLL_DECLSPEC IImportTasklist* CreateImportInterface()
{
	return new CGPImporter;
}

DLL_DECLSPEC int GetInterfaceVersion()
{
	return IIMPORTEXPORT_VERSION;
}
