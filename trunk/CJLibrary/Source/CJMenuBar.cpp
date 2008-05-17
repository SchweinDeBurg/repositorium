// CJMenuBar.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the code written by Paul DiLascia for MSJ.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/24/99 9:12p $
 * $Revision: 11 $
 * $Archive: /CodeJock/CJLibrary/CJMenuBar.cpp $
 *
 * $History: CJMenuBar.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 9:12p
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:36p
 * Updated in $/CodeJockey/CJLibrary
 * Fixed bug with left arrow tracking - Tim Lewis <timlewis@home.com>
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:28p
 * Updated in $/CodeJockey/CJLibrary
 * Further cleanup to handle CCJMenu class.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/15/99    Time: 12:34a
 * Created in $/CodeJockey/CJLibrary
 * Initial creation. Extended the original class written by Paul DiLascia
 * for MSJ. Class uses CCJMenu and fixes some bugs with the original
 * class.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJMenuBar.h"
#include "CJMenu.h"        // CCJMenu class declaration

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMenuBar

CCJMenuBar::CCJMenuBar()
{
	m_iTrackingState		= TRACK_NONE;	// initial state: not tracking 
	m_iPopupTracking		= -1;
	m_iNewPopup				= -1;			// invalid
	m_bAutoRemoveFrameMenu	= TRUE;			// set frame's menu to NULL
}

CCJMenuBar::~CCJMenuBar()
{
	// fix potential resource leak - KStowell - 10-15-99
	m_Font.DeleteObject();
}

IMPLEMENT_DYNAMIC(CCJMenuBar, CCJToolBar)

BEGIN_MESSAGE_MAP(CCJMenuBar, CCJToolBar)
	//{{AFX_MSG_MAP(CCJMenuBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI_RANGE(0, 256, OnUpdateMenuButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCJMenuBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	return CreateEx(pParentWnd, NULL, dwStyle,
		CRect(m_cxLeftBorder, m_cyTopBorder, m_cxRightBorder, m_cyBottomBorder), nID);
}

BOOL CCJMenuBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, CRect rcBorders, UINT nID)
{
	if( !CCJToolBar::CreateEx( pParentWnd, dwCtrlStyle, dwStyle, rcBorders, nID))
		return FALSE;

	UpdateFont();
	m_frameHook.Install(this, *pParentWnd);

	SetWindowText(_T("Menu Bar"));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CCJMenuBar message handlers
CSize CCJMenuBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	if( m_bInReBar ) {
		return CCJToolBar::CalcFixedLayout(bStretch, bHorz);
	}

	else
	{
		CSize size = CCJToolBar::CalcFixedLayout(bStretch, bHorz);
		CRect rcClient;
		GetParentFrame()->GetClientRect(&rcClient);

		if( bHorz ) {
			size.cx = rcClient.Width();
		}
		else {
			size.cy = rcClient.Height();
		}

		return size;
	}
}

//////////////////
// Set menu bar font from current system menu font
//
void CCJMenuBar::UpdateFont()
{
	m_Font.DeleteObject();
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	VERIFY(m_Font.CreateFontIndirect(&ncm.lfMenuFont));
	SetFont(&m_Font);
}

//////////////////
// The reason for having this is so MFC won't automatically disable
// the menu buttons. Assumes < 256 top-level menu items. The ID of
// the ith menu button is i. IOW, the index and ID are the same.
//
void CCJMenuBar::OnUpdateMenuButton(CCmdUI* pCmdUI)
{
	ASSERT_VALID(this);
	if (IsValidButton(pCmdUI->m_nID))
		pCmdUI->Enable(TRUE);
}

//////////////////
// Recompute layout of menu bar
//
void CCJMenuBar::RecomputeMenuLayout()
{
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE |
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

//////////////////
// Make frame recalculate control bar sizes after menu change
//
void CCJMenuBar::RecomputeToolbarSize()
{
	// Force toolbar to recompute size
	CFrameWnd* pFrame = (CFrameWnd*)GetParentFrame();
	ASSERT_VALID(pFrame);
	ASSERT(pFrame->IsFrameWnd());
	pFrame->RecalcLayout();

	// floating frame
	pFrame = GetParentFrame();
	if (pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)))
		pFrame->RecalcLayout();	
}

//////////////////
// Set tracking state: none, button, or popup
//
void CCJMenuBar::SetTrackingState(TRACKINGSTATE iState, int iButton)
{
	ASSERT_VALID(this);
	if (iState != m_iTrackingState) {
		if (iState == TRACK_NONE)
			iButton = -1;

		SetHotItem(iButton);					 // could be none (-1)

		if (iState==TRACK_POPUP) {
			// set related state stuff
			m_bEscapeWasPressed = FALSE;	 // assume Esc key not pressed
			m_bProcessRightArrow =			 // assume left/right arrow..
				m_bProcessLeftArrow = TRUE; // ..will move to prev/next popup
			m_iPopupTracking = iButton;	 // which popup I'm tracking
		}
		m_iTrackingState = iState;
	}
}

//////////////////
// Toggle state from home state to button-tracking and back
//
void CCJMenuBar::ToggleTrackButtonMode()
{
	ASSERT_VALID(this);
	if (m_iTrackingState == TRACK_NONE || m_iTrackingState == TRACK_BUTTON) {
		SetTrackingState(m_iTrackingState == TRACK_NONE ?
			TRACK_BUTTON : TRACK_NONE, 0);
	}
}

//////////////////
// Get button index before/after a given button
//
int CCJMenuBar::GetNextOrPrevButton(int iButton, BOOL bPrev)
{
	ASSERT_VALID(this);
	if (bPrev) {
		iButton--;
		if (iButton <0)
			iButton = GetButtonCount() - 1;
	} else {
		iButton++;
		if (iButton >= GetButtonCount())
			iButton = 0;
	}
	return iButton;
}

/////////////////
// This is to correct a bug in the system toolbar control: TB_HITTEST only
// looks at the buttons, not the size of the window. So it returns a button
// hit even if that button is totally outside the size of the window!
//
int CCJMenuBar::HitTest(CPoint p) const
{
	int iHit = CCJToolBar::HitTest(p);
	if (iHit>0) {
		CRect rc;
		GetClientRect(&rc);
		if (!rc.PtInRect(p)) // if point is outside window
			iHit = -1;			// can't be a hit!
	}
	return iHit;
}

//////////////////
// Load menu from resource
//

BOOL CCJMenuBar::LoadMenu(UINT nID)
{
	CFrameWnd* pFrame = GetParentFrame();
	ASSERT_VALID(pFrame);

	pFrame->GetMenu()->DestroyMenu();
	pFrame->SetMenu(NULL);

	CMenu menu;
	menu.LoadMenu(nID);
	m_nMenuID = nID;
		
	// delete existing buttons
	int nCount = GetButtonCount();
	while (nCount--) {
		VERIFY(DeleteButton(0));
	}
	
	DWORD dwStyle = GetStyle();
	BOOL bModifyStyle = ModifyStyle(0, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);

	SetImageList(NULL);
	UINT nMenuItems = menu.GetMenuItemCount();

	for( UINT i = 0; i < nMenuItems; i++ )
	{
		CString strName;
		if( menu.GetMenuString(i, strName, MF_BYPOSITION ))
		{
			TBBUTTON tbb;
			memset( &tbb, 0, sizeof( TBBUTTON ));
			tbb.idCommand = menu.GetMenuItemID(i);
			
			// Because the toolbar is too brain-damaged to know if it already has
			// a string, and is also too brain-dead to even let you delete strings,
			// I have to determine if each string has been added already. Otherwise
			// in a MDI app, as the menus are repeatedly switched between doc and
			// no-doc menus, I will keep adding strings until somebody runs out of
			// memory. Sheesh!
			// 
			int iString = -1;
			for (int j=0; j<m_arStrings.GetSize(); j++) {
				if (m_arStrings[j] == strName) {
					iString = j; // found it
					break;
				}
			}
			if (iString <0) {
				// string not found: add it
				iString = AddStrings(strName);
				m_arStrings.SetAtGrow(iString, strName);
			}
			
			tbb.iString = iString;
			tbb.fsState = TBSTATE_ENABLED;
			tbb.fsStyle = TBSTYLE_AUTOSIZE;
			tbb.iBitmap = -1;
			tbb.idCommand = i;
			VERIFY(AddButtons(1, &tbb));
		}
	}
	
	if( bModifyStyle ) {
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}
	
	if( ::IsMenu( menu )) {
		AutoSize();								 // size buttons
		RecomputeToolbarSize();				 // and menubar itself
	}

	menu.DestroyMenu();
	return TRUE;
}

//////////////////
// Handle mouse click: if clicked on button, press it
// and go into main menu loop.
//
void CCJMenuBar::OnLButtonDown(UINT nFlags, CPoint pt)
{
	ASSERT_VALID(this);
	int iButton = HitTest(pt);
	if (iButton >= 0 && iButton<GetButtonCount()) // if mouse is over a button:
		TrackPopup(iButton);								 //   track it
	else														 // otherwise:
		CCJToolBar::OnLButtonDown(nFlags, pt);	 //   pass it on...
}

//////////////////
// Handle mouse movement
//
void CCJMenuBar::OnMouseMove(UINT nFlags, CPoint pt)
{
	ASSERT_VALID(this);

	if (m_iTrackingState==TRACK_BUTTON) {

		// In button-tracking state, ignore mouse-over to non-button area.
		// Normally, the toolbar would de-select the hot item in this case.
		// 
		// Only change the hot item if the mouse has actually moved.
		// This is necessary to avoid a bug where the user moves to a different
		// button from the one the mouse is over, and presses arrow-down to get
		// the menu, then Esc to cancel it. Without this code, the button will
		// jump to wherever the mouse is--not right.

		int iHot = HitTest(pt);
		if (IsValidButton(iHot) && pt != m_ptMouse)
			SetHotItem(iHot);
		return;			 // don't let toolbar get it
	}
	m_ptMouse = pt; // remember point
	CCJToolBar::OnMouseMove(nFlags, pt);
}

//////////////////
// Window was resized: need to recompute layout
//
void CCJMenuBar::OnSize(UINT nType, int cx, int cy)
{
	CCJToolBar::OnSize(nType, cx, cy);
	RecomputeMenuLayout();
}

//////////////////
// Bar style changed: eg, moved from left to right dock or floating
//
void CCJMenuBar::OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle)
{
	CCJToolBar::OnBarStyleChange(dwOldStyle, dwNewStyle);
	RecomputeMenuLayout();
}

/////////////////
// When user selects a new menu item, note whether it has a submenu
// and/or parent menu, so I know whether right/left arrow should
// move to the next popup.
//
void CCJMenuBar::OnMenuSelect(HMENU hmenu, UINT iItem)
{
	if (m_iTrackingState > 0) {
		// process right-arrow if item is NOT a submenu
		m_bProcessRightArrow = (::GetSubMenu(hmenu, iItem) == NULL);
		// process left-arrow if curent menu is one I'm tracking
		m_bProcessLeftArrow = TRUE;
	}
}

// globals--yuk! But no other way using windows hooks.
//
static CCJMenuBar*	g_pMenuBar = NULL;
static HHOOK		g_hMsgHook = NULL;

////////////////
// Menu filter hook just passes to virtual CCJMenuBar function
//
LRESULT CALLBACK
CCJMenuBar::MenuInputFilter(int code, WPARAM wp, LPARAM lp)
{
	return (code==MSGF_MENU && g_pMenuBar &&
		g_pMenuBar->OnMenuInput(*((MSG*)lp))) ? TRUE
		: CallNextHookEx(g_hMsgHook, code, wp, lp);
}

//////////////////
// Handle menu input event: Look for left/right to change popup menu,
// mouse movement over over a different menu button for "hot" popup effect.
// Returns TRUE if message handled (to eat it).
//
BOOL CCJMenuBar::OnMenuInput(MSG& m)
{
	ASSERT_VALID(this);
	ASSERT(m_iTrackingState == TRACK_POPUP); // sanity check
	int msg = m.message;

	if (msg==WM_KEYDOWN) {
		// handle left/right-arow.
		TCHAR vkey = static_cast<TCHAR>(m.wParam);

		if ((vkey == VK_LEFT  && m_bProcessLeftArrow) ||
			(vkey == VK_RIGHT && m_bProcessRightArrow)) {

			CancelMenuAndTrackNewOne(
				GetNextOrPrevButton(m_iPopupTracking, vkey==VK_LEFT));
			return TRUE; // eat it

		} else if (vkey == VK_ESCAPE) {
			m_bEscapeWasPressed = TRUE;	 // (menu will abort itself)
		}

	} else if (msg==WM_MOUSEMOVE || msg==WM_LBUTTONDOWN) {
		// handle mouse move or click
		CPoint pt = m.lParam;
		ScreenToClient(&pt);

		if (msg == WM_MOUSEMOVE) {
			if (pt != m_ptMouse) {
				int iButton = HitTest(pt);
				if (IsValidButton(iButton) && iButton != m_iPopupTracking) {
					// user moved mouse over a different button: track its popup
					CancelMenuAndTrackNewOne(iButton);
				}
				m_ptMouse = pt;
			}

		} else if (msg == WM_LBUTTONDOWN) {
			if (HitTest(pt) == m_iPopupTracking) {
				// user clicked on same button I am tracking: cancel menu
				CancelMenuAndTrackNewOne(-1);
				return TRUE; // eat it
			}
		}
	}
	return FALSE; // not handled
}

//////////////////
// Cancel the current popup menu by posting WM_CANCELMODE, and track a new
// menu. iNewPopup is which new popup to track (-1 to quit).
//
void CCJMenuBar::CancelMenuAndTrackNewOne(int iNewPopup)
{
	ASSERT_VALID(this);
	if (iNewPopup != m_iPopupTracking) {
		GetParentFrame()->PostMessage(WM_CANCELMODE); // quit menu loop
		m_iNewPopup = iNewPopup;					 // go to this popup (-1 = quit)
	}
}

//////////////////
// Track the popup submenu associated with the i'th button in the menu bar.
// This fn actually goes into a loop, tracking different menus until the user
// selects a command or exits the menu.
//
void CCJMenuBar::TrackPopup(int iButton)
{
	ASSERT_VALID(this);
	
	m_popupMenu.LoadMenu(m_nMenuID);
	m_popupMenu.LoadToolbar(m_nMenuID);
	ASSERT(::IsMenu(m_popupMenu));

	while (iButton >= 0) {					 // while user selects another menu
		
		m_iNewPopup = -1;						 // assume quit after this
		PressButton(iButton, TRUE);		 // press the button
		UpdateWindow();						 // and force repaint now
		
		// post a simulated arrow-down into the message stream
		// so TrackPopupMenu will read it and move to the first item
		GetParentFrame()->PostMessage(WM_KEYDOWN, VK_DOWN, 1);
		GetParentFrame()->PostMessage(WM_KEYUP, VK_DOWN, 1);
		
		SetTrackingState(TRACK_POPUP, iButton); // enter tracking state
		
		// Need to install a hook to trap menu input in order to make
		// left/right-arrow keys and "hot" mouse tracking work.
		//
		ASSERT(g_pMenuBar == NULL);
		g_pMenuBar = this;
		ASSERT(g_hMsgHook == NULL);
		g_hMsgHook = SetWindowsHookEx(WH_MSGFILTER,
			MenuInputFilter, NULL, ::GetCurrentThreadId());
		
		// get submenu and display it beneath button
		TPMPARAMS tpm;
		CRect rcButton;
		GetRect(iButton, rcButton);
		ClientToScreen(&rcButton);
		CPoint pt = ComputeMenuTrackPoint(rcButton, tpm);

		CCJMenu* pPopupMenu = (CCJMenu*)m_popupMenu.GetSubMenu(iButton);
		pPopupMenu->TrackPopupMenu( TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
			pt.x, pt.y, GetParentFrame());
		
		// uninstall hook.
		::UnhookWindowsHookEx(g_hMsgHook);
		g_hMsgHook = NULL;
		g_pMenuBar = NULL;
		
		PressButton(iButton, FALSE);	 // un-press button
		UpdateWindow();					 // and force repaint now
		
		// If the user exited the menu loop by pressing Escape,
		// return to track-button state; otherwise normal non-tracking state.
		SetTrackingState(m_bEscapeWasPressed ?
			TRACK_BUTTON : TRACK_NONE, iButton);
		
		// If the user moved mouse to a new top-level popup (eg from File to
		// Edit button), I will have posted a WM_CANCELMODE to quit
		// the first popup, and set m_iNewPopup to the new menu to show.
		// Otherwise, m_iNewPopup will be -1 as set above.
		// So just set iButton to the next popup menu and keep looping...
		iButton = m_iNewPopup;
	}
	
	// fix potential resource leak - KStowell - 10-15-99
	m_popupMenu.DestroyMenu();
}

//////////////////
// Given button rectangle, compute point and "exclude rect" for
// TrackPopupMenu, based on current docking style, so that the menu will
// appear always inside the window.
//
CPoint CCJMenuBar::ComputeMenuTrackPoint(const CRect& rcButn, TPMPARAMS& tpm)
{
	tpm.cbSize = sizeof(tpm);
	DWORD dwStyle = m_dwStyle;
	CPoint pt;
	CRect& rcExclude = (CRect&)tpm.rcExclude;
	rcExclude = rcButn;
	::GetWindowRect(::GetDesktopWindow(), &rcExclude);

	switch (dwStyle & CBRS_ALIGN_ANY) {
	case CBRS_ALIGN_BOTTOM:
		pt = CPoint(rcButn.left, rcButn.top);
		rcExclude.top = rcButn.top;
		break;

	case CBRS_ALIGN_LEFT:
		pt = CPoint(rcButn.right, rcButn.top);
		rcExclude.right = rcButn.right;
		break;

	case CBRS_ALIGN_RIGHT:
		pt = CPoint(rcButn.left, rcButn.top);
		rcExclude.left = rcButn.left;
		break;

	default: //	case CBRS_ALIGN_TOP:
		pt = CPoint(rcButn.left, rcButn.bottom);
		break;
	}
	return pt;
}

//////////////////
// This function translates special menu keys and mouse actions.
// You must call it from your frame's PreTranslateMessage.
//
BOOL CCJMenuBar::TranslateFrameMessage(MSG* pMsg)
{
	ASSERT_VALID(this);
	ASSERT(pMsg);
	UINT msg = pMsg->message;
	if (WM_LBUTTONDOWN <= msg && msg <= WM_MOUSELAST) {
		if (pMsg->hwnd != m_hWnd && m_iTrackingState > 0) {
			// user clicked outside menu bar: exit tracking mode
			SetTrackingState(TRACK_NONE);
		}

	} else if (msg==WM_SYSKEYDOWN || msg==WM_SYSKEYUP || msg==WM_KEYDOWN) {

		TCHAR vkey = static_cast<TCHAR>(pMsg->wParam);	// get virt key

		BOOL  bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;		// Alt key down
		
		if (vkey==VK_MENU ||
			(vkey==VK_F10 && !((GetKeyState(VK_SHIFT) & 0x80000000) ||
			                   (GetKeyState(VK_CONTROL) & 0x80000000) || bAlt))) {

			// key is VK_MENU or F10 with no alt/ctrl/shift: toggle menu mode
			if (msg==WM_SYSKEYUP) {
				ToggleTrackButtonMode();
			}
			return TRUE;

		} else if ((msg==WM_SYSKEYDOWN || msg==WM_KEYDOWN)) {
			if (m_iTrackingState == TRACK_BUTTON) {
				// I am tracking: handle left/right/up/down/space/Esc
				switch (vkey) {
				case VK_LEFT:
				case VK_RIGHT:
					// left or right-arrow: change hot button if tracking buttons
					SetHotItem(GetNextOrPrevButton(GetHotItem(), vkey==VK_LEFT));
					return TRUE;

				case VK_SPACE:  // (personally, I like SPACE to enter menu too)
				case VK_UP:
				case VK_DOWN:
					// up or down-arrow: move into current menu, if any
					TrackPopup(GetHotItem());
					return TRUE;

				case VK_ESCAPE:
					// escape key: exit tracking mode
					SetTrackingState(TRACK_NONE);
					return TRUE;
				}
			}

			// Handle alphanumeric key: invoke menu. Note that Alt-X
			// chars come through as WM_SYSKEYDOWN, plain X as WM_KEYDOWN.
			if ((bAlt || m_iTrackingState == TRACK_BUTTON) && isalnum(vkey)) {
				// Alt-X, or else X while in tracking mode
				UINT nID;
				if (MapAccelerator(vkey, nID)) {
					TrackPopup(nID);	 // found menu mnemonic: track it
					return TRUE;		 // handled
				} else if (m_iTrackingState==TRACK_BUTTON && !bAlt) {
					MessageBeep(0);
					return TRUE;
				}
			}

			// Default for any key not handled so far: return to no-menu state
			if (m_iTrackingState > 0) {
				SetTrackingState(TRACK_NONE);
			}
		}
	}
	return FALSE; // not handled, pass along
}

#ifdef _DEBUG
void CCJMenuBar::AssertValid() const
{
	CCJToolBar::AssertValid();
	ASSERT(TRACK_NONE<=m_iTrackingState && m_iTrackingState<=TRACK_POPUP);
	m_frameHook.AssertValid();
}

void CCJMenuBar::Dump(CDumpContext& dc) const
{
	CCJToolBar::Dump(dc);
}
#endif

//////////////////////////////////////////////////////////////////
// CCJMenuBarFrameHook is used to trap menu-related messages sent to the owning
// frame. The same class is also used to trap messages sent to the MDI client
// window in an MDI app. I should really use two classes for this,
// but it uses less code to chare the same class. Note however: there
// are two different INSTANCES of CCJMenuBarFrameHook in CCJMenuBar: one for
// the frame and one for the MDI client window.
//
CCJMenuBarFrameHook::CCJMenuBarFrameHook()
{
}

CCJMenuBarFrameHook::~CCJMenuBarFrameHook()
{
	HookWindow((HWND)NULL); // (unhook)
}

//////////////////
// Install hook to trap window messages sent to frame or MDI client.
// 
BOOL CCJMenuBarFrameHook::Install(CCJMenuBar* pMenuBar, HWND hWndToHook)
{
	ASSERT_VALID(pMenuBar);
	m_pMenuBar = pMenuBar;
	return HookWindow(hWndToHook);
}

//////////////////////////////////////////////////////////////////
// Trap frame/MDI client messages specific to menubar. 
//
LRESULT CCJMenuBarFrameHook::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	CCJMenuBar& mb = *m_pMenuBar;

	switch (msg) {
	// The following messages are trapped for the frame window
	case WM_SYSCOLORCHANGE:
		mb.UpdateFont();
		break;

	case WM_MENUSELECT:
		mb.OnMenuSelect((HMENU)lp, (UINT)LOWORD(wp));
		break;
	}
	return CSubclassWnd::WindowProc(msg, wp, lp);
}
