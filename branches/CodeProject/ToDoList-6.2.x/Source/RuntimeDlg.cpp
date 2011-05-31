// Copyright (C) 2003-2005 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and licenese information
// - taken out from the original ToDoList package for better sharing
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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// RuntimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RuntimeDlg.h"
#include "DlgUnits.h"
#include "WinStyles.h"
#include "WinClasses.h"
#include "WClassDefines.h"
#include "RichEditHelper.h"
#include "DialogHelper.h"
#include "FileMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRuntimeDlg

class RTDLGTEMPLATE : private DLGTEMPLATE
{
public:
	RTDLGTEMPLATE(DWORD dwStyle, DWORD dwExStyle, const CRect& rect)
	{
		style = dwStyle;
		dwExtendedStyle = dwExStyle;
		x = (short)rect.left;
		y = (short)rect.top;
		cx = (short)rect.Width();
		cy = (short)rect.Height();

		CDlgUnits().FromPixels(x, y);
		CDlgUnits().FromPixels(cx, cy);

		cdit = 0; // always 0
		wMenu = 0; // always 0
		wClass = 0; // always 0
		nCaption = 0; // always 0
	}

	virtual ~RTDLGTEMPLATE()
	{
	}

	operator LPCDLGTEMPLATE()
	{
		return (LPCDLGTEMPLATE)this;
	}

private:
	WORD wMenu;
	WORD wClass;
	short nCaption;
};

/////////////////////////////////////////////////////////////////////////////

const CRect CRuntimeDlg::rectAuto = CRect(0, 0, 0, 0);
CMapStringToString CRuntimeDlg::s_mapClasses;

CRuntimeDlg::CRuntimeDlg(LPCTSTR szSettingsKey):
m_sSettingsKey(szSettingsKey),
m_hILarge(NULL),
m_hISmall(NULL),
m_rBordersDLU(0),
m_rBorders(0)
{
	if (!s_mapClasses.GetCount())
	{
		BuildClassMap();
	}

	SetBordersDLU(7);
}

CRuntimeDlg::~CRuntimeDlg()
{
	::DestroyIcon(m_hILarge);
	::DestroyIcon(m_hISmall);
}

BEGIN_MESSAGE_MAP(CRuntimeDlg, CDialog)
	//{{AFX_MSG_MAP(CRuntimeDlg)
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRuntimeDlg message handlers

IMPLEMENT_DYNAMIC(CRuntimeDlg, CDialog)

// private mfc functions we need to access
extern void AFXAPI AfxHookWindowCreate(CWnd* pWnd);
extern BOOL AFXAPI AfxUnhookWindowCreate();

void CRuntimeDlg::OnCancel()
{
	// filter out if we're a child
	if (GetStyle() & WS_CHILD)
	{
		return;
	}

	CDialog::OnCancel();
}

void CRuntimeDlg::OnOK()
{
	// filter out if we're a child
	if (GetStyle() & WS_CHILD)
	{
		return;
	}

	CDialog::OnOK();
}

int CRuntimeDlg::DoModal(LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	// fail immediately if no controls have been added
	if (!m_lstControls.GetCount())
	{
		return IDCANCEL;
	}

	// setup for modal loop and creation
	m_nModalResult = -1;
	m_nFlags |= WF_CONTINUEMODAL;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
	AfxUnhookWindowCreate();

	BOOL bEnableParent = FALSE;
	if (hWndParent != NULL && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
	}

	try
	{
		// create modeless dialog
		if (Create(szCaption, dwStyle, dwExStyle, rect, pParentWnd ? pParentWnd : CWnd::FromHandle(hWndParent), nID))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = MLF_SHOWONIDLE;
				if (GetStyle() & DS_NOIDLEMSG)
				{
					dwFlags |= MLF_NOIDLEMSG;
				}
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}

			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
			{
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
			}
		}
	}
	catch (...)
	{
		m_nModalResult = -1;
	}

	if (bEnableParent)
	{
		::EnableWindow(hWndParent, TRUE);
	}

	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
	{
		::SetActiveWindow(hWndParent);
	}

	// save pos
	SaveCurrentPos();

	// destroy modal window
	DestroyWindow();
	PostModal();

	return m_nModalResult;
}

BOOL CRuntimeDlg::OnInitDialog()
{
	// first init borders if set in DLU
	if (!m_rBordersDLU.IsRectNull())
	{
		SetBordersDLU(m_rBordersDLU.left, m_rBordersDLU.top, m_rBordersDLU.right, m_rBordersDLU.bottom);
	}

	// make sure we create the controls before calling the base class
	CreateControls();

	CDialog::OnInitDialog();

	// add icons only if we have no parent
	CWnd* pParent = GetParent();

	if (!pParent)
	{
		CString sAppPath = FileMisc::GetAppFileName();

		if (::ExtractIconEx(sAppPath, 0, &m_hILarge, &m_hISmall, 1))
		{
			SetIcon(m_hILarge ? m_hILarge : m_hISmall, TRUE);
			SetIcon(m_hISmall ? m_hISmall : m_hILarge, FALSE);
		}
	}

	// set font to parent's if poss
	HFONT hFont = pParent ? (HFONT)pParent->SendMessage(WM_GETFONT) : NULL;

	if (!hFont)
	{
		// use appropriate system font
		NONCLIENTMETRICS ncm;
		ZeroMemory(&ncm, sizeof(ncm));
		ncm.cbSize = sizeof(ncm);

		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, (PVOID)&ncm, FALSE);

		hFont = ::CreateFontIndirect(&ncm.lfMessageFont);

		if (!hFont)
		{
			hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		}
	}

	ASSERT(hFont);
	SetFont(hFont, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CRuntimeDlg::Create(LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle, const CRect& rect, CWnd* pParentWnd, UINT nID)
{
	// cache and remove visibility
	BOOL bVisible = (dwStyle & WS_VISIBLE);
	dwStyle &= ~WS_VISIBLE;

	// remove DS_SETFONT (not supported)
	dwStyle &= ~DS_SETFONT;

	if (dwStyle & WS_CHILD)
	{
		dwStyle |= DS_CONTROL;
		dwExStyle |= WS_EX_CONTROLPARENT;
	}

	// create modeless dialog
	AfxHookWindowCreate(this);

	if (CreateDlgIndirect(RTDLGTEMPLATE(dwStyle, dwExStyle, rect), pParentWnd, NULL) != NULL)
	{
		SetInitialPos(rect, dwStyle);

		// set window text
		SetWindowText(szCaption);

		// set control id
		SetDlgCtrlID(nID);

		PostCreate(); // for derived classes

		// reshow?
		if (bVisible)
		{
			ShowWindow(SW_SHOW);
		}

		return TRUE;
	}

	return FALSE;
}

void CRuntimeDlg::SetInitialPos(LPCRECT pRect, DWORD dwStyle)
{
	// size to fit?
	if (!pRect || rectAuto.EqualRect(pRect))
	{
		if (!m_sSettingsKey.IsEmpty())
		{
			CRect rect;
			rect.left = AfxGetApp()->GetProfileInt(m_sSettingsKey, _T("left"), rectAuto.left);
			rect.right = AfxGetApp()->GetProfileInt(m_sSettingsKey, _T("right"), rectAuto.right);
			rect.top = AfxGetApp()->GetProfileInt(m_sSettingsKey, _T("top"), rectAuto.top);
			rect.bottom = AfxGetApp()->GetProfileInt(m_sSettingsKey, _T("bottom"), rectAuto.bottom);

			if (rect == rectAuto) // first time
			{
				AutoFit();

				// center
				if (dwStyle & DS_CENTER)
				{
					CenterWindow();
				}
			}
			else
			{
				MoveWindow(rect);
			}
		}
		else
		{
			AutoFit();

			// center
			if (dwStyle & DS_CENTER)
			{
				CenterWindow();
			}
		}
	}
	else
	{
		MoveWindow(pRect);
	}
}

void CRuntimeDlg::SaveCurrentPos()
{
	if (!m_sSettingsKey.IsEmpty())
	{
		CRect rect;
		GetWindowRect(rect);

		AfxGetApp()->WriteProfileInt(m_sSettingsKey, _T("left"), rect.left);
		AfxGetApp()->WriteProfileInt(m_sSettingsKey, _T("right"), rect.right);
		AfxGetApp()->WriteProfileInt(m_sSettingsKey, _T("top"), rect.top);
		AfxGetApp()->WriteProfileInt(m_sSettingsKey, _T("bottom"), rect.bottom);
	}
}

void CRuntimeDlg::OnDestroy()
{
	// save pos
	SaveCurrentPos();

	CDialog::OnDestroy();
}

void CRuntimeDlg::AutoFit()
{
	// iterate all the child controls accumulating the largest bottom right coord
	CRect rClient(0, 0, 0, 0);

	CWnd* pCtrl = GetWindow(GW_CHILD);

	while (pCtrl)
	{
		CRect rCtrl;
		pCtrl->GetWindowRect(rCtrl);
		ScreenToClient(rCtrl);

		rClient.right = max(rClient.right, rCtrl.right);
		rClient.bottom = max(rClient.bottom, rCtrl.bottom);

		pCtrl = pCtrl->GetNextWindow();
	}

	// add border
	rClient.right += m_rBorders.right;
	rClient.bottom += m_rBorders.bottom;

	// calc new window rect
	CRect rWindow;
	GetWindowRect(rWindow);
	CalcWindowRect(rClient);

	rClient = CRect(rWindow.TopLeft(), rClient.Size());

	CPoint ptTopLeft = GetInitialPos();

	if (ptTopLeft.x != -1 || ptTopLeft.y != -1)
	{
		rClient.OffsetRect(ptTopLeft - rClient.TopLeft());
	}

	else // match centerpoints of old and new
	{
		rClient.OffsetRect(rWindow.CenterPoint() - rClient.CenterPoint());
	}

	MoveWindow(rClient);
}

BOOL CRuntimeDlg::AddControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
	const CRect& rect, UINT nID)
{
	return AddControl(szClass, szCaption, dwStyle, dwExStyle, rect.left, rect.top, rect.Width(), rect.Height(), nID);
}

BOOL CRuntimeDlg::AddControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
	int x, int y, int cx, int cy, UINT nID)
{
	if (GetSafeHwnd())
	{
		return (NULL != CreateControl(szClass, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, FALSE));
	}
	else
	{
		RTCONTROL rtc(NULL, szClass, szCaption, dwStyle, dwExStyle, CRect(x, y, x + cx, y + cy), nID, FALSE);
		m_lstControls.AddTail(rtc);
	}

	return TRUE;
}

BOOL CRuntimeDlg::AddControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
	const CRect& rect, UINT nID)
{
	return AddControl(pWnd, szCaption, dwStyle, dwExStyle, rect.left, rect.top, rect.Width(), rect.Height(), nID);
}

BOOL CRuntimeDlg::AddControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
	int x, int y, int cx, int cy, UINT nID)
{
	if (GetSafeHwnd())
	{
		return CreateControl(pWnd, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, FALSE);
	}
	else
	{
		RTCONTROL rtc(pWnd, NULL, szCaption, dwStyle, dwExStyle, CRect(x, y, x + cx, y + cy), nID, FALSE);
		m_lstControls.AddTail(rtc);
	}

	return TRUE;
}

BOOL CRuntimeDlg::AddRCControl(LPCTSTR szRCType, LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle,
	DWORD dwExStyle, int x, int y, int cx, int cy, UINT nID, UINT nIconID)
{
	CString sClass(szClass);

	// get the win32 class name
	if (sClass.IsEmpty() || _tcsicmp(szRCType, _T("CONTROL")) != 0)
	{
		if (!CRCCtrlParser::GetClassName(szRCType, sClass) || sClass.IsEmpty())
		{
			return FALSE;
		}
	}

	if (!dwStyle)
	{
		dwStyle = CRCCtrlParser::GetDefaultStyles(sClass);
	}

	if (!(dwStyle & WS_NOTVISIBLE))
	{
		dwStyle |= WS_VISIBLE;
	}
	else
	{
		dwStyle &= ~WS_NOTVISIBLE;
	}

	if (CRCCtrlParser::CtrlWantsClientEdge(sClass))
	{
		dwExStyle |= WS_EX_CLIENTEDGE;
	}

	if (GetSafeHwnd())
	{
		return (NULL != CreateControl(sClass, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, TRUE, nIconID));
	}
	else
	{
		RTCONTROL rtc(NULL, sClass, szCaption, dwStyle, dwExStyle, CRect(x, y, x + cx, y + cy), nID, TRUE, nIconID);
		m_lstControls.AddTail(rtc);
	}

	return TRUE;
}

BOOL CRuntimeDlg::AddRCControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle,
	DWORD dwExStyle, int x, int y, int cx, int cy, UINT nID, UINT nIconID)
{
	CString sClass = GetClassName(pWnd);

	if (!dwStyle)
	{
		dwStyle = CRCCtrlParser::GetDefaultStyles(sClass);
	}

	if (!(dwStyle & WS_NOTVISIBLE))
	{
		dwStyle |= WS_VISIBLE;
	}
	else
	{
		dwStyle &= ~WS_NOTVISIBLE;
	}

	if (CRCCtrlParser::CtrlWantsClientEdge(sClass))
	{
		dwExStyle |= WS_EX_CLIENTEDGE;
	}

	if (GetSafeHwnd())
	{
		return (NULL != CreateControl(pWnd, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, TRUE, nIconID));
	}
	else
	{
		RTCONTROL rtc(pWnd, sClass, szCaption, dwStyle, dwExStyle, CRect(x, y, x + cx, y + cy), nID, TRUE, nIconID);
		m_lstControls.AddTail(rtc);
	}

	return TRUE;
}

HWND CRuntimeDlg::CreateControl(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
	int x, int y, int cx, int cy, UINT nID, BOOL bDLU, UINT nIconID)
{
	if (bDLU)
	{
		CDlgUnits dlu(*this);

		dlu.ToPixels(x, y);
		dlu.ToPixels(cx, cy);
	}

	// override caption
	CString sCaption = OverrideItemText(nID);

	if (sCaption.IsEmpty())
	{
		sCaption = szCaption;
	}

	// validate the topleft position against the borders
	x = max(x, m_rBorders.left);
	y = max(y, m_rBorders.top);

	// make reasonably sure control is child
	dwStyle |= WS_CHILD;
	dwStyle &= ~WS_POPUP;

	// we don't need to be notified by our own children
	dwExStyle |= WS_EX_NOPARENTNOTIFY;

	// if its a richedit then make sure richedit dll is initialized
	HWND hwnd = NULL;

	if (CWinClasses::IsRichEditControl(szClass))
	{
		hwnd = CreateRichEdit(szClass, sCaption, dwStyle, dwExStyle, x, y, cx, cy, *this);
	}
	else
	{
		hwnd = ::CreateWindowEx(dwExStyle, szClass, sCaption, dwStyle, x, y, cx, cy, *this, NULL, NULL, NULL);
	}

	if (hwnd)
	{
		SetWindowLong(hwnd, GWL_ID, nID);

		HFONT hFont = (HFONT)SendMessage(WM_GETFONT); // gets the dialog's font

		if (!hFont)
		{
			hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		}

		::SendMessage(hwnd, WM_SETFONT, (WPARAM)hFont, 0);

		if (nIconID && _tcsicmp(szClass, _T("static")) == 0)
		{
			HICON hIcon = AfxGetApp()->LoadIcon(nIconID);

			if (hIcon)
			{
				::SendMessage(hwnd, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			}
		}
		else if (!sCaption.IsEmpty())
		{
			::SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)sCaption);
		}
	}
	else
	{
		TRACE(_T("CreateWindowEx(%s) failed. GetLastError returned %08X\n"), szClass, GetLastError());
	}

	return hwnd;
}

HWND CRuntimeDlg::CreateRichEdit(LPCTSTR szClass, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
	int x, int y, int cx, int cy, HWND hwndParent)
{
	CRichEditHelper::InitRichEdit();

	ASSERT(CWinClasses::IsRichEditControl(szClass));

	HWND hwnd = ::CreateWindowEx(dwExStyle, szClass, szCaption, dwStyle, x, y, cx, cy, hwndParent, NULL, NULL, NULL);

	// if it failed and its not richedit10 then try richedit10
	if (!hwnd && !CWinClasses::IsClass(szClass, WC_RICHEDIT))
	{
		hwnd = ::CreateWindowEx(dwExStyle, WC_RICHEDIT, szCaption, dwStyle, x, y, cx, cy, hwndParent, NULL, NULL, NULL);
	}

	return hwnd;
}

BOOL CRuntimeDlg::CreateControl(CWnd* pWnd, LPCTSTR szCaption, DWORD dwStyle, DWORD dwExStyle,
	int x, int y, int cx, int cy, UINT nID, BOOL bDLU, UINT nIconID)
{
	ASSERT(!pWnd->GetSafeHwnd());

	if (pWnd->GetSafeHwnd())
	{
		return FALSE;
	}

	CString sClass = GetControlClassName(pWnd);

	if (sClass.IsEmpty())
	{
		return FALSE;
	}

	HWND hwnd = CreateControl(sClass, szCaption, dwStyle, dwExStyle, x, y, cx, cy, nID, bDLU, nIconID);

	if (hwnd)
	{
		return pWnd->SubclassWindow(hwnd);
	}

	// else
	return FALSE;
}


BOOL CRuntimeDlg::CreateControl(const RTCONTROL& rtc)
{
	if (rtc.m_pWnd)
		return CreateControl(rtc.m_pWnd, rtc.m_sCaption, rtc.m_dwStyle, rtc.m_dwExStyle,
			rtc.m_rect.left, rtc.m_rect.top, rtc.m_rect.Width(), rtc.m_rect.Height(),
			rtc.m_nID, rtc.m_bDLU, rtc.m_nIconID);

	else if (!rtc.m_sClass.IsEmpty())
		return (NULL != CreateControl(rtc.m_sClass, rtc.m_sCaption, rtc.m_dwStyle, rtc.m_dwExStyle,
			rtc.m_rect.left, rtc.m_rect.top, rtc.m_rect.Width(), rtc.m_rect.Height(),
			rtc.m_nID, rtc.m_bDLU, rtc.m_nIconID));

	else
	{
		return FALSE;
	}
}

void CRuntimeDlg::CreateControls()
{
	ASSERT(GetSafeHwnd());

	if (!GetSafeHwnd())
	{
		return;
	}

	POSITION pos = m_lstControls.GetHeadPosition();

	while (pos)
	{
		CreateControl(m_lstControls.GetNext(pos));
	}
}

void CRuntimeDlg::BuildClassMap()
{
	if (s_mapClasses.GetCount())
	{
		return;   // already done
	}

	s_mapClasses[_T("CStatic")]         = WC_STATIC;
	s_mapClasses[_T("CButton")]         = WC_BUTTON;
	s_mapClasses[_T("CListBox")]        = WC_LISTBOX;
	s_mapClasses[_T("CComboBox")]       = WC_COMBOBOX;
	s_mapClasses[_T("CEdit")]           = WC_EDIT;
	s_mapClasses[_T("CScrollBar")]      = WC_SCROLLBAR;
	s_mapClasses[_T("CListCtrl")]       = WC_LISTVIEW;
	s_mapClasses[_T("CTreeCtrl")]       = WC_TREEVIEW;
	s_mapClasses[_T("CSpinButtonCtrl")] = WC_SPIN;
	s_mapClasses[_T("CHeaderCtrl")]     = WC_HEADER;
	s_mapClasses[_T("CSliderCtrl")]     = WC_SLIDER;
	s_mapClasses[_T("CProgressCtrl")]   = WC_PROGRESS;
	s_mapClasses[_T("CHotKeyCtrl")]     = WC_HOTKEY;
	s_mapClasses[_T("CToolTipCtrl")]    = WC_TOOLTIPS;
	s_mapClasses[_T("CTabCtrl")]        = WC_TABCONTROL;
	s_mapClasses[_T("CAnimateCtrl")]    = WC_ANIMATE;
	s_mapClasses[_T("CToolBarCtrl")]    = WC_TOOLBAR;
	s_mapClasses[_T("CStatusBarCtrl")]  = WC_STATUSBAR;
	s_mapClasses[_T("CRichEditCtrl")]   = WC_RICHEDIT20;
	s_mapClasses[_T("CIPAddressCtrl")]  = WC_IPADDRESS;
	s_mapClasses[_T("CDateTimeCtrl")]   = WC_DATETIMEPICK;
	s_mapClasses[_T("CMonthCalCtrl")]   = WC_MONTHCAL;
	s_mapClasses[_T("CReBar")]          = WC_REBAR;
	//	s_mapClasses[_T("")] = "";
}

void CRuntimeDlg::SetBorders(int nLeft, int nTop, int nRight, int nBottom)
{
	m_rBordersDLU.SetRectEmpty();
	m_rBorders.SetRect(max(0, nLeft), max(0, nTop), max(0, nRight), max(0, nBottom));
}

void CRuntimeDlg::SetBordersDLU(int nLeft, int nTop, int nRight, int nBottom)
{
	if (GetSafeHwnd())
	{
		CDlgUnits dlu(*this);

		dlu.ToPixels(nLeft, nTop);
		dlu.ToPixels(nRight, nBottom);

		SetBorders(nLeft, nTop, nRight, nBottom);
	}
	else
	{
		m_rBorders.SetRectEmpty();
		m_rBordersDLU.SetRect(max(0, nLeft), max(0, nTop), max(0, nRight), max(0, nBottom));
	}
}

CRect CRuntimeDlg::OffsetCtrl(UINT nCtrlID, int x, int y)
{
	return CDialogHelper::OffsetCtrl(this, nCtrlID, x, y);
}

CRect CRuntimeDlg::MoveCtrl(UINT nCtrlID, int x, int y)
{
	return CDialogHelper::MoveCtrl(this, nCtrlID, x, y);
}

CRect CRuntimeDlg::ResizeCtrl(UINT nCtrlID, int cx, int cy)
{
	return CDialogHelper::ResizeCtrl(this, nCtrlID, cx, cy);
}

int CRuntimeDlg::AddRCControls(const CString& sRCControls)
{
	CRCCtrlParser rccp(sRCControls);

	return rccp.GetRCControls(m_lstControls);
}

BOOL CRuntimeDlg::PreTranslateMessage(MSG* pMsg)
{
	BOOL bChild = (GetStyle() & WS_CHILD);
	BOOL bTab = (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB);

	// also filter out what look like accelerator keys
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam != VK_CONTROL && (GetKeyState(VK_CONTROL) & 0x8000))
		{
			return FALSE;
		}
	}

	return (bChild && !bTab) ? CWnd::PreTranslateMessage(pMsg) : CDialog::PreTranslateMessage(pMsg);
}


void CRuntimeDlg::ExcludeControls(CDC* pDC, UINT nCtrlIDFrom, UINT nCtrlIDTo)
{
	CDialogHelper::ExcludeControls(this, pDC, nCtrlIDFrom, nCtrlIDTo);
}

void CRuntimeDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialog::OnSetFocus(pOldWnd);

	CWnd* pChild = GetWindow(GW_CHILD);

	if (pChild)
	{
		pChild->SetFocus();
	}
}

void CRuntimeDlg::SetFont(CFont* pFont, BOOL bRedraw)
{
	SetFont(pFont ? (HFONT)pFont->GetSafeHandle() : NULL, bRedraw);
}

void CRuntimeDlg::SetFont(HFONT hFont, BOOL bRedraw)
{
	CDialogHelper::SetFont(this, hFont, bRedraw);
}

int CRuntimeDlg::CalcLinesRequired(const CString& sText, int nWidthDLU)
{
	// iterate the text by segment (\n) and add up the number of lines
	// each segment takes up
	int nStart = 0;
	int nCR = sText.Find('\n', nStart);
	int nLines = 0;

	while (nCR != -1)
	{
		int nSegLenDLU = (nCR - nStart) * 4;

		if (nSegLenDLU)
		{
			nLines += (nSegLenDLU / nWidthDLU) + ((nSegLenDLU % nWidthDLU) ? 1 : 0);
		}
		else
		{
			nLines++;   // empty line
		}

		nStart = nCR + 1;
		nCR = sText.Find('\n', nStart);
	}

	// handle last segment
	int nSegLenDLU = (sText.GetLength() - nStart) * 4;
	nLines += (nSegLenDLU / nWidthDLU) + ((nSegLenDLU % nWidthDLU) ? 1 : 0);

	return nLines;
}

void CRuntimeDlg::ShowControls(UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bShow)
{
	CDialogHelper::ShowControls(this, nCtrlIDFrom, nCtrlIDTo, bShow);
}

CString CRuntimeDlg::GetControlClassName(CWnd* pWnd)
{
	ASSERT(pWnd);

	if (!pWnd)
	{
		return _T("");
	}

	// if there is no permanent mapping to this CWnd just return GetClassName()
	// but only if it has a window handle attached else it must be a real CWnd
	if (pWnd->GetSafeHwnd() && CWnd::FromHandlePermanent(*pWnd) != pWnd)
	{
		return GetClassName(pWnd);
	}

	CRuntimeClass* pRTClass = pWnd->GetRuntimeClass();

	// work our way up the derivation chain till we find a match
	while (pRTClass)
	{
		CString sWinClass, sRTClass = ATL::CA2T(pRTClass->m_lpszClassName);

		if (s_mapClasses.Lookup(sRTClass, sWinClass))
		{
			return sWinClass;
		}

		// try ancestor
#ifdef _AFXDLL
		pRTClass = (*pRTClass->m_pfnGetBaseClass)();
#else
		pRTClass = pRTClass->m_pBaseClass;
#endif
	}

	// means we did not find anything
	ASSERT(0);
	return _T("");
}
