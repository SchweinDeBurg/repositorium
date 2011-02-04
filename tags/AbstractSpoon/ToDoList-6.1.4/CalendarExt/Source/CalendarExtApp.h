//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - adjusted #include's paths
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// CalendarExtApp.h: interface for the CCalendarExtApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALENDAREXTAPP_H__8FDB207D_40DB_405D_8D76_A9898B6DAE21__INCLUDED_)
#define AFX_CALENDAREXTAPP_H__8FDB207D_40DB_405D_8D76_A9898B6DAE21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IUIExtension.h"

class CCalendarExtApp : public IUIExtension
{
public:
	CCalendarExtApp();
	virtual ~CCalendarExtApp();

	virtual void Release(); // releases the interface

	virtual const char* GetMenuText();
	virtual HICON GetIcon();

	virtual IUIExtensionWindow* CreateExtensionWindow(HWND hParent, BOOL bShow = TRUE, LPSIZE pSize = NULL);

protected:
	HICON m_hIcon;
};

#endif // !defined(AFX_CALENDAREXTAPP_H__8FDB207D_40DB_405D_8D76_A9898B6DAE21__INCLUDED_)
