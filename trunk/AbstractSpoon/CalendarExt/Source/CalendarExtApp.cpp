//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - adjusted #include's paths
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// CalendarExtApp.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
#include "CalendarExtResource.h"
#include "CalendarExtApp.h"
#include "CalendarFrameWnd.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE CalendarExtDLL = { NULL, NULL };

extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("CalendarExt.dll initializing...\n");

		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(CalendarExtDLL, hInstance))
		{
			return 0;
		}

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(CalendarExtDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("CalendarExt.dll terminating...\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(CalendarExtDLL);
	}
	return 1;   // ok
}

DLL_DECLSPEC IUIExtension* CreateUIExtensionInterface()
{
	return new CCalendarExtApp;
}

DLL_DECLSPEC int GetInterfaceVersion()
{
	return IUIEXTENSION_VERSION;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCalendarExtApp::CCalendarExtApp():
m_hIcon(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_CALENDAR);
}

CCalendarExtApp::~CCalendarExtApp()
{
}

void CCalendarExtApp::Release()
{
	delete this;
}

const char* CCalendarExtApp::GetMenuText()
{
#ifdef _DEBUG
	return "Calendar (DEBUG)";
#else
	return "Calendar";
#endif
}

HICON CCalendarExtApp::GetIcon()
{
	return m_hIcon;
}

IUIExtensionWindow* CCalendarExtApp::CreateExtensionWindow(HWND hParent, BOOL bShow, LPSIZE pSize)
{
	IUIExtensionWindow* pWindow = new CCalendarFrameWnd;

	pSize->cx = 500;
	pSize->cy = 400;

	if (pWindow && !pWindow->Create(hParent, bShow, pSize))
	{
		delete pWindow;
		pWindow = NULL;
	}

	return pWindow;
}
