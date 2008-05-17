// CJToolBar.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 1:56p $
 * $Revision: 18 $
 * $Archive: /CodeJock/CJLibrary/CJToolBar.cpp $
 *
 * $History: CJToolBar.cpp $
 * 
 * *****************  Version 18  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 1:56p
 * Updated in $/CodeJock/CJLibrary
 * Removed Invalidate() calls from OnWindowPosChanging(...) to eliminate
 * screen flicker.
 * 
 * *****************  Version 17  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:51p
 * Updated in $/CodeJock/CJLibrary
 * Added CCJFontCombo to the list of controls that can be inserted into a
 * CCJToolBar.
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 9/13/99    Time: 5:45a
 * Updated in $/CodeJockey/CJLibrary
 * Fixed initialization bug (small).
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 8/30/99    Time: 12:06a
 * Updated in $/CodeJockey/CJLibrary
 * Added support for CCJComboBoxEx control.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:20p
 * Updated in $/CodeJockey/CJLibrary
 * Re-wrote toolbar class for vc5 compatibility. Added customization based 
 * on the "Customizable toolbar" article by Doug Keith - 
 * http://www.codeguru.com/toolbar/customizable_tb.shtml.
 * 
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 11:44p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 3/13/99    Time: 11:43p
 * Updated in $/CodeJockey/CJ60Lib
 * Changed class to handle both Office and DevStudio style to toolbars. To
 * use Office toolbars, use the CreateEx() method, otherwise your toolbars
 * will look like DevStudio.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 12/14/98   Time: 11:42p
 * Updated in $/CodeJockey/CJ60Lib
 * Changed class to derive from Paul DiLascia's CToolBar, this class
 * adds gripper and control insertion.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 11/02/98   Time: 11:41p
 * Updated in $/CodeJockey/CJ60Lib
 * Fixed bug with DrawNoGripper() method - (Christian Skovdal Andersen).
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 11:41p
 * Created in $/CodeJockey/CJ60Lib
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJToolBar.h"
#include "CJFlatComboBox.h"
#include "CJComboBoxEx.h"
#include "CJFontCombo.h"
#include "CJMenu.h"        // CCJMenu class declaration

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_CUSTOMIZE_BAR	0xFF

/*
DIBs use RGBQUAD format:
0xbb 0xgg 0xrr 0x00

  Reasonably efficient code to convert a COLORREF into an
  RGBQUAD is byte-order-dependent, so we need different
  code depending on the byte order we're targeting.
*/
#define _RGB_TO_RGBQUAD(r,g,b)   (RGB(b,g,r))
#define _CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))

struct CJX_COLORMAP
{
	// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
	DWORD rgbqFrom;
	int iSysColorTo;
};

const CJX_COLORMAP _sysColorMap[] =
{
	// mapping from color in DIB to system color
	{ _RGB_TO_RGBQUAD(0x00, 0x00, 0x00),  COLOR_BTNTEXT },       // black
	{ _RGB_TO_RGBQUAD(0x80, 0x80, 0x80),  COLOR_BTNSHADOW },     // dark gray
	{ _RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0),  COLOR_BTNFACE },       // bright gray
	{ _RGB_TO_RGBQUAD(0xFF, 0xFF, 0xFF),  COLOR_BTNHIGHLIGHT }   // white
};

HBITMAP AFXAPI
_LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc, BOOL bMono)
{
	HGLOBAL hglb;
	if ((hglb = LoadResource(hInst, hRsrc)) == NULL)
		return NULL;
	
	LPBITMAPINFOHEADER lpBitmap = (LPBITMAPINFOHEADER)LockResource(hglb);
	if (lpBitmap == NULL)
		return NULL;
	
	// make copy of BITMAPINFOHEADER so we can modify the color table
	const int nColorTableSize = 16;
	UINT nSize = lpBitmap->biSize + nColorTableSize * sizeof(RGBQUAD);
	LPBITMAPINFOHEADER lpBitmapInfo = (LPBITMAPINFOHEADER)::malloc(nSize);
	if (lpBitmapInfo == NULL)
		return NULL;
	memcpy(lpBitmapInfo, lpBitmap, nSize);
	
	// color table is in RGBQUAD DIB format
	DWORD* pColorTable =
		(DWORD*)(((LPBYTE)lpBitmapInfo) + (UINT)lpBitmapInfo->biSize);
	
	for (int iColor = 0; iColor < nColorTableSize; iColor++)
	{
		// look for matching RGBQUAD color in original
		for (int i = 0; i < _countof(_sysColorMap); i++)
		{
			if (pColorTable[iColor] == _sysColorMap[i].rgbqFrom)
			{
				if (bMono)
				{
					// all colors except text become white
					if (_sysColorMap[i].iSysColorTo != COLOR_BTNTEXT)
						pColorTable[iColor] = _RGB_TO_RGBQUAD(255, 255, 255);
				}
				else
					pColorTable[iColor] =
					_CLR_TO_RGBQUAD(::GetSysColor(_sysColorMap[i].iSysColorTo));
				break;
			}
		}
	}
	
	int nWidth = (int)lpBitmapInfo->biWidth;
	int nHeight = (int)lpBitmapInfo->biHeight;
	HDC hDCScreen = ::GetDC(NULL);
	HBITMAP hbm = ::CreateCompatibleBitmap(hDCScreen, nWidth, nHeight);
	
	if (hbm != NULL)
	{
		HDC hDCGlyphs = ::CreateCompatibleDC(hDCScreen);
		HBITMAP hbmOld = (HBITMAP)::SelectObject(hDCGlyphs, hbm);
		
		LPBYTE lpBits;
		lpBits = (LPBYTE)(lpBitmap + 1);
		lpBits += (1 << (lpBitmapInfo->biBitCount)) * sizeof(RGBQUAD);
		
		StretchDIBits(hDCGlyphs, 0, 0, nWidth, nHeight, 0, 0, nWidth, nHeight,
			lpBits, (LPBITMAPINFO)lpBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		SelectObject(hDCGlyphs, hbmOld);
		::DeleteDC(hDCGlyphs);
	}
	::ReleaseDC(NULL, hDCScreen);
	
	// free copy of bitmap info struct and resource itself
	::free(lpBitmapInfo);
	::FreeResource(hglb);
	
	return hbm;
}

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

struct AFX_DLLVERSIONINFO
{
	DWORD cbSize;
	DWORD dwMajorVersion;                   // Major version
	DWORD dwMinorVersion;                   // Minor version
	DWORD dwBuildNumber;                    // Build number
	DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
};

typedef HRESULT (CALLBACK* AFX_DLLGETVERSIONPROC)(AFX_DLLVERSIONINFO *);

int _ComCtlVersion = -1;

DWORD AFXAPI _GetComCtlVersion()
{
	// return cached version if already determined...
	if (_ComCtlVersion != -1)
		return _ComCtlVersion;
	
	// otherwise determine comctl32.dll version via DllGetVersion
	HINSTANCE hInst = ::GetModuleHandleA("COMCTL32.DLL");
	ASSERT(hInst != NULL);
	AFX_DLLGETVERSIONPROC pfn;
	pfn = (AFX_DLLGETVERSIONPROC)GetProcAddress(hInst, "DllGetVersion");
	DWORD dwVersion = VERSION_WIN4;
	if (pfn != NULL)
	{
		AFX_DLLVERSIONINFO dvi;
		memset(&dvi, 0, sizeof(dvi));
		dvi.cbSize = sizeof(dvi);
		HRESULT hr = (*pfn)(&dvi);
		if (SUCCEEDED(hr))
		{
			ASSERT(dvi.dwMajorVersion <= 0xFFFF);
			ASSERT(dvi.dwMinorVersion <= 0xFFFF);
			dwVersion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
		}
	}
	_ComCtlVersion = dwVersion;
	return dwVersion;
}

struct CJX_CONTROLPOS {
	int				nIndex;
	int				nID;
	CRect			rectOldPos;
} ;

int _dropDownWidth = -1;

int AFXAPI _GetDropDownWidth()
{
	// return cached version if already determined...
	if (_dropDownWidth != -1)
		return _dropDownWidth;

	// otherwise calculate it...
	HDC hDC = GetDC(NULL);
	ASSERT(hDC != NULL);
	HFONT hFont;
	if ((hFont = ::CreateFont(GetSystemMetrics(SM_CYMENUCHECK), 0, 0, 0,
		FW_NORMAL, 0, 0, 0, SYMBOL_CHARSET, 0, 0, 0, 0, _T("Marlett"))) != NULL)
	{
		HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	
		VERIFY(GetCharWidth(hDC, '6', '6', &_dropDownWidth));

		// fix potential resource leak - KStowell - 10-21-99
		if (hFont != NULL) {
			::SelectObject(hDC, hOldFont);
			::DeleteObject(hFont);
		}
	}

	::ReleaseDC(NULL, hDC);
	ASSERT(_dropDownWidth != -1);
	return _dropDownWidth;
}

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar

int CCJToolBar::m_nBarNumber = 0;

CCJToolBar::CCJToolBar()
{
	m_bShowDropArrow = false;
	m_pControls		 = NULL;
	m_pDropButtons	 = NULL;		// list of drop-down buttons
	m_nButtons		 = 0;
	m_pBarInfo		 = NULL;
	m_hKeyRoot		 = HKEY_CURRENT_USER;
	
	// get a pointer to the application.
	CWinApp* pApp = AfxGetApp();
	ASSERT( pApp );

	++m_nBarNumber;
	m_strValueName.Format( _T("CJToolBar %d"), m_nBarNumber );
	m_strSubKey.Format( _T("Software\\%s\\%s\\Settings"),
		pApp->m_pszRegistryKey, pApp->m_pszProfileName );

	// initialize state
	m_pStringMap = NULL;
	m_hRsrcImageWell = NULL;
	m_hInstImageWell = NULL;
	m_hbmImageWell = NULL;
	m_bDelayedButtonLayout = TRUE;

	// default image sizes
	m_sizeImage.cx = 16;
	m_sizeImage.cy = 15;

	// default button sizes
	m_sizeButton.cx = 23;
	m_sizeButton.cy = 22;

	// top and bottom borders are 1 larger than default for ease of grabbing
	m_cyTopBorder = 3;
	m_cyBottomBorder = 3;
	m_bExStyle = false;
}

CCJToolBar::~CCJToolBar()
{
	AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);
	delete m_pStringMap;
	
	m_nCount = 0;
	
	while (m_pDropButtons)
	{
		DROPDOWNBUTTON* pnext = m_pDropButtons->next;
		delete m_pDropButtons;
		m_pDropButtons = pnext;
	}
	
	if( m_pControls ) {
		for( POSITION pos = m_pControls->GetHeadPosition() ; pos ; ) {
			delete m_pControls->GetNext(pos);
		}
		delete m_pControls;
	}
}

BOOL CCJToolBar::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	return CreateEx(pParentWnd, NULL, dwStyle,
		CRect(m_cxLeftBorder, m_cyTopBorder, m_cxRightBorder, m_cyBottomBorder), nID);
}

BOOL CCJToolBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle, DWORD dwStyle, CRect rcBorders, UINT nID)
{
	ASSERT_VALID(pParentWnd);   // must have a parent
	ASSERT (!((dwStyle & CBRS_SIZE_FIXED) && (dwStyle & CBRS_SIZE_DYNAMIC)));
	
	// KStowell - bool for drawing office style gripper.
	if( dwCtrlStyle != NULL )
		m_bExStyle = true;

	SetBorders(rcBorders);

	// save the style
	m_dwStyle = (dwStyle & CBRS_ALL)|( dwStyle & CBRS_GRIPPER );
	if (nID == AFX_IDW_TOOLBAR)
		m_dwStyle |= CBRS_HIDE_INPLACE;

	dwStyle &= ~CBRS_ALL;
	dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE;
	dwStyle |= dwCtrlStyle;

	// initialize common controls

#ifdef _VC_VERSION_5

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_BAR_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));

#else

	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_BAR_REG));

#endif

	_GetComCtlVersion();
	ASSERT(_ComCtlVersion != -1);
	_GetDropDownWidth();
	ASSERT(_dropDownWidth != -1);

	// create the HWND
	CRect rect; rect.SetRectEmpty();
	if (!CWnd::Create(TOOLBARCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID))
		return FALSE;

	// sync up the sizes
	SetSizes(m_sizeButton, m_sizeImage);

	// Note: Parent must resize itself for control bar to be resized

	return TRUE;
}

BOOL CCJToolBar::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CCJToolBarBase::PreCreateWindow(cs))
		return FALSE;
	
	cs.style |= TBSTYLE_TRANSPARENT|TBSTYLE_FLAT;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// 1999 Kirk Stowell - Inserts a control into the toolbar at the given button id.
//
CWnd* CCJToolBar::InsertControl( CRuntimeClass* pClass, LPCTSTR lpszWindowName, CRect& rect, UINT nID, DWORD dwStyle )
{
	CWnd *pCtrl = NULL;
	
	if( pClass->IsDerivedFrom( RUNTIME_CLASS( CCJComboBoxEx )))	// CCJComboBoxEx control.
	{
		pCtrl = new CCJComboBoxEx;
	
		ASSERT_VALID( pCtrl );
		
		if(((CCJComboBoxEx*)pCtrl)->Create( WS_CHILD|WS_VISIBLE|dwStyle, rect, this, nID ) == FALSE )
		{
			delete pCtrl;
			return NULL;
		}
	}
	else if( pClass->IsDerivedFrom( RUNTIME_CLASS( CCJFlatComboBox )))	// CCJFlatComboBox control.
	{
		pCtrl = new CCJFlatComboBox;
	
		ASSERT_VALID( pCtrl );
		
		if(((CCJFlatComboBox*)pCtrl)->Create( WS_CHILD|WS_VISIBLE|dwStyle, rect, this, nID ) == FALSE )
		{
			delete pCtrl;
			return NULL;
		}
	}
	else if( pClass->IsDerivedFrom( RUNTIME_CLASS( CCJFontCombo )))	// CCJFontCombo control.
	{
		pCtrl = new CCJFontCombo;
	
		ASSERT_VALID( pCtrl );
		
		if(((CCJFontCombo*)pCtrl)->Create( WS_CHILD|WS_VISIBLE|dwStyle, rect, this, nID ) == FALSE )
		{
			delete pCtrl;
			return NULL;
		}
	}
	else if( pClass->IsDerivedFrom( RUNTIME_CLASS( CComboBox )))	// CComboBox control.
	{
		pCtrl = new CComboBox;

		ASSERT_VALID( pCtrl );

		if(((CComboBox*)pCtrl)->Create( WS_CHILD|WS_VISIBLE|dwStyle, rect, this, nID ) == FALSE )
		{
			delete pCtrl;
			return NULL;
		}
	}
	else if( pClass->IsDerivedFrom( RUNTIME_CLASS( CEdit )))		// CEdit control.
	{
		pCtrl = new CEdit;

		ASSERT_VALID( pCtrl );

		if(((CEdit*)pCtrl)->Create( WS_CHILD|WS_VISIBLE|dwStyle, rect, this, nID ) == FALSE )
		{
			delete pCtrl;
			return NULL;
		}
	}
	else if( pClass->IsDerivedFrom( RUNTIME_CLASS( CButton )))		// CButton control.
	{
		pCtrl = new CButton;

		ASSERT_VALID( pCtrl );

		if(((CButton*)pCtrl)->Create( lpszWindowName, WS_CHILD|WS_VISIBLE|dwStyle, rect, this, nID ) == FALSE )
		{
			delete pCtrl;
			return NULL;
		}
	}
	else if( pClass->IsDerivedFrom( RUNTIME_CLASS( CWnd )))			// CWnd object.
	{
		pCtrl = new CWnd;
		
		ASSERT_VALID( pCtrl );

#ifdef _UNICODE
		TCHAR szClassName[ 256 ];

		MultiByteToWideChar( CP_ACP,
							 MB_PRECOMPOSED,
							 pClass->m_lpszClassName,
							 -1,
							 szClassName,
							 255 );

		if(((CWnd*)pCtrl)->Create( szClassName, lpszWindowName, WS_CHILD|WS_VISIBLE|dwStyle, rect, this, nID ) == FALSE )
		{
			delete pCtrl;
			return NULL;
		}
#else
		if(((CWnd*)pCtrl)->Create( pClass->m_lpszClassName, lpszWindowName, WS_CHILD|WS_VISIBLE|dwStyle, rect, this, nID ) == FALSE )
		{
			delete pCtrl;
			return NULL;
		}
#endif
	}
	else															// An invalid object was passed in
	{
		ASSERT( FALSE );
		return NULL;
	}

	// if our object list has not been allocated, do it now...
	if( m_pControls == NULL )
	{
		m_pControls = new CObList();
		ASSERT( m_pControls );
	}

	// we have to remember this control, so we can delete it later
	m_pControls->AddTail( pCtrl );

	return InsertControl( pCtrl, rect, nID );
}

CWnd* CCJToolBar::InsertControl(CWnd* pCtrl, CRect & rect, UINT nID)
{
	ASSERT_VALID( pCtrl );

	// make sure the id is valid, and set the button 
	// style for a seperator.
	ASSERT( CommandToIndex( nID ) >= 0 );
	SetButtonInfo( CommandToIndex( nID ), nID, TBBS_SEPARATOR, rect.Width());
	
	// insert the control into the toolbar.
	GetItemRect( CommandToIndex(nID), &rect );
	pCtrl->SetWindowPos(0, rect.left, rect.top, 0, 0,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS );
	pCtrl->SetFont( GetFont( ));
	pCtrl->ShowWindow( SW_SHOW );

	return pCtrl;
}

//////////////////
// 1997 Microsoft Systems Journal - Written by Paul DiLascia.
// call to add drop-down menus to toolbar buttons.
//
BOOL CCJToolBar::AddDropDownButton(UINT nIDButton, UINT nIDMenu, BOOL bArrow)
{
	ASSERT_VALID(this);
	
	DROPDOWNBUTTON* pb = FindDropDownButton(nIDButton);
	if (!pb) {
		pb = new DROPDOWNBUTTON;
		ASSERT(pb);
		pb->next = m_pDropButtons;
		m_pDropButtons = pb;
	}
	pb->idButton = nIDButton;
	pb->idMenu   = nIDMenu;
	
	int iButton = CommandToIndex(nIDButton);
	
	DWORD dwStyle = GetButtonStyle(iButton);
	dwStyle |= TBSTYLE_DROPDOWN;
	SetButtonStyle(iButton, dwStyle);
	
	if (bArrow)
		SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	
	return TRUE;
}

//////////////////
// 1997 Microsoft Systems Journal - Written by Paul DiLascia.
// Find buttons structure for given ID
//
DROPDOWNBUTTON* CCJToolBar::FindDropDownButton(UINT nID)
{
	for (DROPDOWNBUTTON* pb = m_pDropButtons; pb; pb = pb->next) {
		if (pb->idButton == nID)
			return pb;
	}
	return NULL;
}

//////////////////
// 1997 Microsoft Systems Journal - Written by Paul DiLascia.
// Message handler for TBN_DROPDOWN. Default is to display the 
// specified menu at the right place. You can override to generate dynamic menus
//
// Args:
//		- NMTOOLBAR struct from TBN_DROPDOWN
//		- command id of button
//		- point to display menu at
//
void CCJToolBar::OnToolBarBtnDropDown(NMHDR* pNMHDR, LRESULT* pRes)
{
	UNUSED_ALWAYS( pRes );
	
	const NMTOOLBAR& nmtb = *(NMTOOLBAR*)pNMHDR;
	
	// get location of button
	CRect rc;
	GetRect(nmtb.iItem, rc);
	ClientToScreen(&rc);
	
	// call virtual function to display dropdown menu
	OnDropDownButton(nmtb, nmtb.iItem, rc);
}

/////////////////
// 1997 Microsoft Systems Journal - Written by Paul DiLascia.
// Virtual fn you can override to hand drop-down button
// events with more friendly args
//
void CCJToolBar::OnDropDownButton(const NMTOOLBAR& nmtb, UINT nID, CRect rc)
{
	UNUSED_ALWAYS( nID );

	DROPDOWNBUTTON* pb = FindDropDownButton(nmtb.iItem);
	if (pb && pb->idMenu) {
		
		// load and display popup menu
		CCJMenu menu;
		VERIFY(menu.LoadMenu(pb->idMenu));
		CCJMenu* pPopup = (CCJMenu*)menu.GetSubMenu(0);
		ASSERT(pPopup);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
			rc.left, rc.bottom, GetParentFrame(), &rc);
	}
}

//////////////////
// 1997 Microsoft Systems Journal - Written by Paul DiLascia.
// Now toolbar has really moved: repaint area beneath old position
//
void CCJToolBar::OnWindowPosChanged(WINDOWPOS* lpwndpos) 
{
	CCJToolBarBase::OnWindowPosChanged(lpwndpos);
	if (!(lpwndpos->flags & SWP_NOMOVE)) {	 // if moved:
		InvalidateOldPos(m_rcOldPos);		 // invalidate area of old position
		
		// Now paint my non-client area at the new location.
		// Without this, you will still have a partial display bug (try it!)
		SendMessage(WM_NCPAINT);
	}
}

//////////////////
// 1997 Microsoft Systems Journal - Written by Paul DiLascia.
// Invalidate toolbar rectangle. Because flat toolbars are transparent,
// this requires invalidating parent and all siblings that intersect the
// rectangle.
//
void CCJToolBar::InvalidateOldPos(const CRect& rcInvalid)
{
	// make parent paint the area beneath rectangle
	CWnd* pParent = GetParent();		// parent (dock bar/frame) window
	ASSERT_VALID(pParent);				// check
	CRect rc( rcInvalid );       		// copy rectangle
	pParent->ScreenToClient(&rc);		// convert to parent client coords
	pParent->InvalidateRect(&rc);		// invalidate

	// now do same for each sibling too
	for (CWnd* pSib = pParent->GetWindow(GW_CHILD);
		  pSib;
		  pSib=pSib->GetNextWindow(GW_HWNDNEXT)) {

		CRect rc;										// window rect of sibling
		pSib->GetWindowRect(&rc);					// ...
		if (rc.IntersectRect(rc, rcInvalid)) {	// if intersects invalid rect
			pSib->ScreenToClient(&rc);				// convert to sibling coords
			pSib->InvalidateRect(&rc);				// invalidate
			pSib->SendMessage(WM_NCPAINT);		// nonclient area too!
		}
	}
}

//////////////////
// 1997 Microsoft Systems Journal - Written by Paul DiLascia.
// This is the all-important function that gets the true size of a button,
// instead of using m_sizeButton. And it's virtual, so you can override if
// my algorithm doesn't work, as will surely be the case in some circumstances.
//
CSize CCJToolBar::GetButtonSize(TBBUTTON* pData, int iButton)
{
	ASSERT(_ComCtlVersion > 0);

	// Get the actual size of the button, not what's in m_sizeButton.
	// Make sure to do SendMessage instead of calling MFC's GetItemRect,
	// which has all sorts of bad side-effects! (Go ahead, take a look at it.)
	// 
	CRect rc;
	SendMessage(TB_GETITEMRECT, iButton, (LPARAM)&rc);
	CSize sz = rc.Size();

	////////////////
	// Now must do special case for various versions of comctl32.dll,
	//
	DWORD dwStyle = pData[iButton].fsStyle;
	if ((pData[iButton].fsState & TBSTATE_WRAP)) {
		if (dwStyle & TBSTYLE_SEP) {
			// this is the last separator in the row (eg vertically docked)
			// fudge the height, and ignore the width. TB_GETITEMRECT will return
			// size = (8 x 22) even for a separator in vertical toolbar
			//
			if (_ComCtlVersion <= VERSION_IE3)
				sz.cy -= 3;		// empircally good fudge factor
			else if (_ComCtlVersion != VERSION_IE4)
				sz.cy = sz.cx;
			sz.cx = 0;			// separator takes no width if it's the last one

		} else if (dwStyle & TBSTYLE_DROPDOWN && ( m_bShowDropArrow == false )) {
			// ignore width of dropdown
			sz.cx = 0;
		}
	}
	return sz;
}

// This function saves the state (visible buttons, toolbar position, etc.)
// of the toolbar, using the registry key provided to the Create(...) function.
void CCJToolBar::SaveState()
{
	// if there is an associated registry subkey
	if (m_strSubKey.GetLength())
	{
		// save the toolbar state to the registry
		GetToolBarCtrl().SaveState( m_hKeyRoot, m_strSubKey, m_strValueName );
	}
}

// This function restores the state (visible buttons, toolbar position, etc.)
// of the toolbar, using the registry key provided to the Create(...) function.
void CCJToolBar::RestoreState()
{
	// if there is an associated registry subkey
	if (m_strSubKey.GetLength())
	{
		// restore the toolbar state from the registry
		GetToolBarCtrl().RestoreState( m_hKeyRoot, m_strSubKey, m_strValueName );
	}
}

// This function is called when the user begins dragging a toolbar
// button or when the customization dialog is being populated with
// toolbar information.  Basically, *pResult should be populated with
// your answer to the question, "is the user allowed to delete this
// button?".
void CCJToolBar::OnToolBarQueryDelete(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );

	// if we're not floating - user can delete anything
	*pResult = !IsFloating();
}

// This function is called when the user begins dragging a toolbar
// button or when the customization dialog is being populated with
// toolbar information.  Basically, *pResult should be populated with
// your answer to the question, "is the user allowed to insert a
// button to the left of this one?".
void CCJToolBar::OnToolBarQueryInsert(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );

	// if we're not floating - user can insert anywhere
	*pResult = !IsFloating();
}

// This function is called whenever the user makes a change to the
// layout of the toolbar.  Calling the mainframe's RecalcLayout forces
// the toolbar to repaint itself.
void CCJToolBar::OnToolBarChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );
	UNUSED_ALWAYS( pResult );

	// force the frame window to recalculate the size
	GetParentFrame()->RecalcLayout();
}

// This function is called when the user begins dragging a toolbar button.
void CCJToolBar::OnToolBarBeginDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );
	UNUSED_ALWAYS( pResult );
}

// This function is called when the user has completed a dragging operation.
void CCJToolBar::OnToolBarEndDrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );
	UNUSED_ALWAYS( pResult );
}

// This function is called when the user initially calls up the toolbar
// customization dialog box.
void CCJToolBar::OnToolBarBeginAdjust(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );
	UNUSED_ALWAYS( pResult );
}

// This function is called when the user clicks on the help button on the
// toolbar customization dialog box.
void CCJToolBar::OnToolBarCustomHelp(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );
	UNUSED_ALWAYS( pResult );
}

// This function is called when the user dismisses the toolbar customization
// dialog box.
void CCJToolBar::OnToolBarEndAdjust(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );
	UNUSED_ALWAYS( pResult );

	// save the state of the toolbar for reinitialization
	SaveState();
}

// This function is called to populate the toolbar customization dialog box
// with information regarding all of the possible toolbar buttons.
void CCJToolBar::OnToolBarGetButtonInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pResult );

	TBNOTIFY* tbStruct;		// data needed by customize dialog box

	// init the pointer
	tbStruct = (TBNOTIFY *)pNMHDR;

	// if the index is valid
	if (0 <= tbStruct->iItem && tbStruct->iItem < m_nButtons)
	{
		// copy the stored button structure
		tbStruct->tbButton = m_pBarInfo[tbStruct->iItem].tbButton;

		// copy the text for the button label in the dialog
		_tcscpy( tbStruct->pszText, m_pBarInfo[tbStruct->iItem].btnText );

		// indicate valid data was sent
		*pResult = TRUE;
	}

	// else there is no button for this index
	else
	{
		*pResult = FALSE;
	}
}

// This function is called when the user clicks on the reset button on the
// toolbar customization dialog box.
void CCJToolBar::OnToolBarReset(NMHDR *pNMHDR, LRESULT *pResult)
{
	UNUSED_ALWAYS( pNMHDR );
	UNUSED_ALWAYS( pResult );

	// restore the toolbar to the way it was before entering customization
	RestoreState();
}

void CCJToolBar::OnDestroy() 
{
	// save the current state of the toolbar
	SaveState();
	
	CCJToolBarBase::OnDestroy();
}

void CCJToolBar::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( m_pBarInfo )
	{
		CPoint pt( point );
		ClientToScreen( &pt );
		
		// load and display popup menu
		CCJMenu popupMenu;
		VERIFY(popupMenu.CreatePopupMenu( ));
		popupMenu.InsertMenu(0, MF_BYPOSITION, IDC_CUSTOMIZE_BAR, _T("&Customize..."));
		int nResult = popupMenu.TrackPopupMenu(
			TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL|TPM_RETURNCMD,
			pt.x, pt.y, this );
		
		if( nResult == IDC_CUSTOMIZE_BAR )
		{
			// open the customization dialog.
			GetToolBarCtrl().Customize();
		}
	}

	else {
		CCJToolBarBase::OnRButtonDown(nFlags, point);
	}
}

/////////////////////////////////////////////////////////////////////////////
// The remainder of this class is mostly copied straight from MFC ( vc6 ), 
// except for the modifications which are commented...
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar

BOOL CCJToolBar::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	// If the toolbar is inside a ReBar, we need to make the parent frame
	// the owner so it will get notifications.
	CWnd* pParent = GetParent();
	ASSERT(pParent);
	TCHAR classname[64];
	GetClassName(pParent->m_hWnd, classname, _countof(classname));
	if (_tcscmp(classname, REBARCLASSNAME)==0)
	{
		CFrameWnd* pFrame = GetParentFrame();
		ASSERT_VALID(pFrame);
		SetOwner(pFrame);
		m_bInReBar = true;
		m_bExStyle = false;
	}
	
	if (!CCJToolBarBase::OnNcCreate(lpCreateStruct))
		return FALSE;
	
	// if the owner was set before the toolbar was created, set it now
	if (m_hWndOwner != NULL)
		DefWindowProc(TB_SETPARENT, (WPARAM)m_hWndOwner, 0);

	DefWindowProc(TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	return TRUE;
}

void CCJToolBar::SetOwner(CWnd* pOwnerWnd)
{
	ASSERT_VALID(this);
	if (m_hWnd != NULL)
	{
		ASSERT(::IsWindow(m_hWnd));
		DefWindowProc(TB_SETPARENT, (WPARAM)pOwnerWnd->GetSafeHwnd(), 0);
	}
	CCJToolBarBase::SetOwner(pOwnerWnd);
}

void CCJToolBar::SetSizes(SIZE sizeButton, SIZE sizeImage)
{
	ASSERT_VALID(this);

	// sizes must be non-zero and positive
	ASSERT(sizeButton.cx > 0 && sizeButton.cy > 0);
	ASSERT(sizeImage.cx > 0 && sizeImage.cy > 0);

	// button must be big enough to hold image
	//   + 7 pixels on x
	//   + 6 pixels on y
	ASSERT(sizeButton.cx >= sizeImage.cx + 7);
	ASSERT(sizeButton.cy >= sizeImage.cy + 6);

	if (::IsWindow(m_hWnd))
	{
		// set the sizes via TB_SETBITMAPSIZE and TB_SETBUTTONSIZE
		VERIFY(SendMessage(TB_SETBITMAPSIZE, 0, MAKELONG(sizeImage.cx, sizeImage.cy)));
		VERIFY(SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(sizeButton.cx, sizeButton.cy)));

		Invalidate();   // just to be nice if called when toolbar is visible
	}
	else
	{
		// just set our internal values for later
		m_sizeButton = sizeButton;
		m_sizeImage = sizeImage;
	}
}

void CCJToolBar::SetHeight(int cyHeight)
{
	ASSERT_VALID(this);

	int nHeight = cyHeight;
	if (m_dwStyle & CBRS_BORDER_TOP)
		cyHeight -= afxData.cyBorder2;
	if (m_dwStyle & CBRS_BORDER_BOTTOM)
		cyHeight -= afxData.cyBorder2;
	m_cyBottomBorder = (cyHeight - m_sizeButton.cy) / 2;
	// if there is an extra pixel, m_cyTopBorder will get it
	m_cyTopBorder = cyHeight - m_sizeButton.cy - m_cyBottomBorder;
	if (m_cyTopBorder < 0)
	{
		TRACE1("Warning: CCJToolBar::SetHeight(%d) is smaller than button.\n",
			nHeight);
		m_cyBottomBorder += m_cyTopBorder;
		m_cyTopBorder = 0;  // will clip at bottom
	}

	// recalculate the non-client region
	SetWindowPos(NULL, 0, 0, 0, 0,
		SWP_DRAWFRAME|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOZORDER);
	Invalidate();   // just to be nice if called when toolbar is visible
}

struct CToolBarData
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	//WORD aItems[wItemCount]

	WORD* items()
		{ return (WORD*)(this+1); }
};

BOOL CCJToolBar::LoadToolBar(LPCTSTR lpszResourceName, TOOLBARINFO* pBarInfo)
{
	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);

	// determine location of the bitmap in resource fork
	HINSTANCE hInst = AfxFindResourceHandle(lpszResourceName, RT_TOOLBAR);
	HRSRC hRsrc = ::FindResource(hInst, lpszResourceName, RT_TOOLBAR);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hGlobal = LoadResource(hInst, hRsrc);
	if (hGlobal == NULL)
		return FALSE;

	CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
	if (pData == NULL)
		return FALSE;
	ASSERT(pData->wVersion == 1);

	UINT* pItems = new UINT[pData->wItemCount];
	for (int i = 0; i < pData->wItemCount; i++)
		pItems[i] = pData->items()[i];
	BOOL bResult = SetButtons(pItems, pData->wItemCount);
	delete[] pItems;

	if (bResult)
	{
		// set new sizes of the buttons
		CSize sizeImage(pData->wWidth, pData->wHeight);
		CSize sizeButton(pData->wWidth + 7, pData->wHeight + 7);
		SetSizes(sizeButton, sizeImage);

		// load bitmap now that sizes are known by the toolbar control
		bResult = LoadBitmap(lpszResourceName);
	}

	UnlockResource(hGlobal);
	FreeResource(hGlobal);

	// is the toolbar is customizable?
	if(( pBarInfo ) && ( bResult )) {
		m_pBarInfo = pBarInfo;
		m_nButtons = GetToolBarCtrl().GetButtonCount();

		// modify the style to include adjustable
		ModifyStyle(0, CCS_ADJUSTABLE);
		RestoreState();
	}


	return bResult;
}

BOOL CCJToolBar::LoadBitmap(LPCTSTR lpszResourceName)
{
	ASSERT_VALID(this);
	ASSERT(lpszResourceName != NULL);

	// determine location of the bitmap in resource fork
	HINSTANCE hInstImageWell = AfxFindResourceHandle(lpszResourceName, RT_BITMAP);
	HRSRC hRsrcImageWell = ::FindResource(hInstImageWell, lpszResourceName, RT_BITMAP);
	if (hRsrcImageWell == NULL)
		return FALSE;

	// load the bitmap
	HBITMAP hbmImageWell;
	hbmImageWell = _LoadSysColorBitmap(hInstImageWell, hRsrcImageWell);

	// tell common control toolbar about the new bitmap
	if (!AddReplaceBitmap(hbmImageWell))
		return FALSE;

	// remember the resource handles so the bitmap can be recolored if necessary
	m_hInstImageWell = hInstImageWell;
	m_hRsrcImageWell = hRsrcImageWell;
	return TRUE;
}

BOOL CCJToolBar::SetBitmap(HBITMAP hbmImageWell)
{
	ASSERT_VALID(this);
	ASSERT(hbmImageWell != NULL);

	// the caller must manage changing system colors
	m_hInstImageWell = NULL;
	m_hRsrcImageWell = NULL;

	// tell common control toolbar about the new bitmap
	return AddReplaceBitmap(hbmImageWell);
}

BOOL CCJToolBar::AddReplaceBitmap(HBITMAP hbmImageWell)
{
	// need complete bitmap size to determine number of images
	BITMAP bitmap;
	VERIFY(::GetObject(hbmImageWell, sizeof(BITMAP), &bitmap));

	// add the bitmap to the common control toolbar
	BOOL bResult;
	if (m_hbmImageWell == NULL)
	{
		TBADDBITMAP addBitmap;
		addBitmap.hInst = NULL; // makes TBADDBITMAP::nID behave a HBITMAP
		addBitmap.nID = (UINT)hbmImageWell;
		bResult =  DefWindowProc(TB_ADDBITMAP,
			bitmap.bmWidth / m_sizeImage.cx, (LPARAM)&addBitmap) == 0;
	}
	else
	{
		TBREPLACEBITMAP replaceBitmap;
		replaceBitmap.hInstOld = NULL;
		replaceBitmap.nIDOld = (UINT)m_hbmImageWell;
		replaceBitmap.hInstNew = NULL;
		replaceBitmap.nIDNew = (UINT)hbmImageWell;
		replaceBitmap.nButtons = bitmap.bmWidth / m_sizeImage.cx;
		bResult = (BOOL)DefWindowProc(TB_REPLACEBITMAP, 0,
			(LPARAM)&replaceBitmap);
	}
	// remove old bitmap, if present
	if (bResult)
	{
		AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);
		m_hbmImageWell = hbmImageWell;
	}

	return bResult;
}

BOOL CCJToolBar::SetButtons(const UINT* lpIDArray, int nIDCount)
{
	ASSERT_VALID(this);
	ASSERT(nIDCount >= 1);  // must be at least one of them
	ASSERT(lpIDArray == NULL ||
		AfxIsValidAddress(lpIDArray, sizeof(UINT) * nIDCount, FALSE));

	// delete all existing buttons
	int nCount = (int)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
	while (nCount--)
		VERIFY(DefWindowProc(TB_DELETEBUTTON, 0, 0));

	TBBUTTON button; memset(&button, 0, sizeof(TBBUTTON));
	button.iString = -1;
	if (lpIDArray != NULL)
	{
		// add new buttons to the common control
		int iImage = 0;
		for (int i = 0; i < nIDCount; i++)
		{
			button.fsState = TBSTATE_ENABLED;
			if ((button.idCommand = *lpIDArray++) == 0)
			{
				// separator
				button.fsStyle = TBSTYLE_SEP;
				// width of separator includes 8 pixel overlap
				ASSERT(_ComCtlVersion != -1);
				if ((GetStyle() & TBSTYLE_FLAT) || _ComCtlVersion == VERSION_IE4)
					button.iBitmap = 6;
				else
					button.iBitmap = 8;
			}
			else
			{
				// a command button with image
				button.fsStyle = TBSTYLE_BUTTON;
				button.iBitmap = iImage++;
			}
			if (!DefWindowProc(TB_ADDBUTTONS, 1, (LPARAM)&button))
				return FALSE;
		}
	}
	else
	{
		// add 'blank' buttons
		button.fsState = TBSTATE_ENABLED;
		for (int i = 0; i < nIDCount; i++)
		{
			ASSERT(button.fsStyle == TBSTYLE_BUTTON);
			if (!DefWindowProc(TB_ADDBUTTONS, 1, (LPARAM)&button))
				return FALSE;
		}
	}
	m_nCount = (int)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
	m_bDelayedButtonLayout = TRUE;

	return TRUE;
}

#ifdef AFX_CORE3_SEG
#pragma code_seg(AFX_CORE3_SEG)
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar attribute access

void CCJToolBar::GetButton(int nIndex, TBBUTTON* pButton) const
{
	CCJToolBar* pBar = (CCJToolBar*)this;
	VERIFY(pBar->DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)pButton));
	// TBSTATE_ENABLED == TBBS_DISABLED so invert it
	pButton->fsState ^= TBSTATE_ENABLED;
}

void CCJToolBar::SetButton(int nIndex, TBBUTTON* pButton)
{
	// get original button state
	TBBUTTON button;
	VERIFY(DefWindowProc(TB_GETBUTTON, nIndex, (LPARAM)&button));

	// prepare for old/new button comparsion
	button.bReserved[0] = 0;
	button.bReserved[1] = 0;
	// TBSTATE_ENABLED == TBBS_DISABLED so invert it
	pButton->fsState ^= TBSTATE_ENABLED;
	pButton->bReserved[0] = 0;
	pButton->bReserved[1] = 0;

	// nothing to do if they are the same
	if (memcmp(pButton, &button, sizeof(TBBUTTON)) != 0)
	{
		// don't redraw everything while setting the button
		DWORD dwStyle = GetStyle();
		ModifyStyle(WS_VISIBLE, 0);
		VERIFY(DefWindowProc(TB_DELETEBUTTON, nIndex, 0));
		VERIFY(DefWindowProc(TB_INSERTBUTTON, nIndex, (LPARAM)pButton));
		ModifyStyle(0, dwStyle & WS_VISIBLE);

		// invalidate appropriate parts
		if (((pButton->fsStyle ^ button.fsStyle) & TBSTYLE_SEP) ||
			((pButton->fsStyle & TBSTYLE_SEP) && pButton->iBitmap != button.iBitmap))
		{
			// changing a separator
			Invalidate();
		}
		else
		{
			// invalidate just the button
			CRect rect;
			if (DefWindowProc(TB_GETITEMRECT, nIndex, (LPARAM)&rect))
				InvalidateRect(rect);
		}
	}
}

int CCJToolBar::CommandToIndex(UINT nIDFind) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	CCJToolBar* pBar = (CCJToolBar*)this;
	return (int)pBar->DefWindowProc(TB_COMMANDTOINDEX, nIDFind, 0);
}

UINT CCJToolBar::GetItemID(int nIndex) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	TBBUTTON button;
	GetButton(nIndex, &button);
	return button.idCommand;
}

void CCJToolBar::GetItemRect(int nIndex, LPRECT lpRect) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// handle any delayed layout
	if (m_bDelayedButtonLayout)
		((CCJToolBar*)this)->Layout();

	// now it is safe to get the item rectangle
	CCJToolBar* pBar = (CCJToolBar*)this;
	if (!pBar->DefWindowProc(TB_GETITEMRECT, nIndex, (LPARAM)lpRect))
		SetRectEmpty(lpRect);
}

void CCJToolBar::Layout()
{
	ASSERT(m_bDelayedButtonLayout);

	m_bDelayedButtonLayout = FALSE;

	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
	if ((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
		((CCJToolBar*)this)->CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH | LM_COMMIT);
	else if (bHorz)
		((CCJToolBar*)this)->CalcDynamicLayout(0, LM_HORZ | LM_HORZDOCK | LM_COMMIT);
	else
		((CCJToolBar*)this)->CalcDynamicLayout(0, LM_VERTDOCK | LM_COMMIT);
}

UINT CCJToolBar::GetButtonStyle(int nIndex) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	TBBUTTON button;
	GetButton(nIndex, &button);
	return MAKELONG(button.fsStyle, button.fsState);
}

void CCJToolBar::SetButtonStyle(int nIndex, UINT nStyle)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	TBBUTTON button;
	GetButton(nIndex, &button);
	if (button.fsStyle != (BYTE)LOWORD(nStyle) || button.fsState != (BYTE)HIWORD(nStyle))
	{
		button.fsStyle = (BYTE)LOWORD(nStyle);
		button.fsState = (BYTE)HIWORD(nStyle);
		SetButton(nIndex, &button);
		m_bDelayedButtonLayout = TRUE;
	}
}

#define CX_OVERLAP  0

CSize CCJToolBar::CalcSize(TBBUTTON* pData, int nCount)
{
	ASSERT(pData != NULL && nCount > 0);

	CPoint cur(0,0);
	CSize sizeResult(0,0);
	int cyTallestOnRow = 0;

	for (int i = 0; i < nCount; i++)
	{
		if (pData[i].fsState & TBSTATE_HIDDEN)
			continue;

		// **PD**
		// Load actual size of button into a local variable
		// called m_sizeButton. C++ will use this instead of
		// CToolBar::m_sizeButton.
		//
		CSize m_sizeButton = GetButtonSize(pData, i);

		// **PD**
		// I also changed the logic below to be more correct.
		cyTallestOnRow = max(cyTallestOnRow, m_sizeButton.cy);
		sizeResult.cx = max(cur.x + m_sizeButton.cx, sizeResult.cx);
		sizeResult.cy = max(cur.y + m_sizeButton.cy, sizeResult.cy);

		cur.x += m_sizeButton.cx - CX_OVERLAP;

		if (pData[i].fsState & TBSTATE_WRAP)
		{
			cur.x = 0;
			cur.y += cyTallestOnRow;
			cyTallestOnRow = 0;
			if (pData[i].fsStyle & TBSTYLE_SEP)
				cur.y += m_sizeButton.cy;
		}
	}
	return sizeResult;
}

int CCJToolBar::WrapToolBar(TBBUTTON* pData, int nCount, int nWidth)
{
	ASSERT(pData != NULL && nCount > 0);

	int nResult = 0;
	int x = 0;
	for (int i = 0; i < nCount; i++)
	{
		pData[i].fsState &= ~TBSTATE_WRAP;

		if (pData[i].fsState & TBSTATE_HIDDEN)
			continue;

		int dx, dxNext;

		// **PD**
		// Load actual size of button into a local variable
		// called m_sizeButton. C++ will use this instead of
		// CToolBar::m_sizeButton.
		//
		CSize m_sizeButton = GetButtonSize(pData, i);

		dx = m_sizeButton.cx;
		dxNext = dx - CX_OVERLAP;

		if (x + dx > nWidth)
		{
			BOOL bFound = FALSE;
			for (int j = i; j >= 0  &&  !(pData[j].fsState & TBSTATE_WRAP); j--)
			{
				// Find last separator that isn't hidden
				// a separator that has a command ID is not
				// a separator, but a custom control.
				if ((pData[j].fsStyle & TBSTYLE_SEP) &&
					(pData[j].idCommand == 0) &&
					!(pData[j].fsState & TBSTATE_HIDDEN))
				{
					bFound = TRUE; i = j; x = 0;
					pData[j].fsState |= TBSTATE_WRAP;
					nResult++;
					break;
				}
			}
			if (!bFound)
			{
				for (int j = i - 1; j >= 0 && !(pData[j].fsState & TBSTATE_WRAP); j--)
				{
					// Never wrap anything that is hidden,
					// or any custom controls
					if ((pData[j].fsState & TBSTATE_HIDDEN) ||
						((pData[j].fsStyle & TBSTYLE_SEP) &&
						(pData[j].idCommand != 0)))
						continue;

					bFound = TRUE; i = j; x = 0;
					pData[j].fsState |= TBSTATE_WRAP;
					nResult++;
					break;
				}
				if (!bFound)
					x += dxNext;
			}
		}
		else
			x += dxNext;
	}
	return nResult + 1;
}

void CCJToolBar::SizeToolBar(TBBUTTON* pData, int nCount, int nLength, BOOL bVert)
{
	ASSERT(pData != NULL && nCount > 0);

	if (!bVert)
	{
		int nMin, nMax, nTarget, nCurrent, nMid;

		// Wrap ToolBar as specified
		nMax = nLength;
		nTarget = WrapToolBar(pData, nCount, nMax);

		// Wrap ToolBar vertically
		nMin = 0;
		nCurrent = WrapToolBar(pData, nCount, nMin);

		if (nCurrent != nTarget)
		{
			while (nMin < nMax)
			{
				nMid = (nMin + nMax) / 2;
				nCurrent = WrapToolBar(pData, nCount, nMid);

				if (nCurrent == nTarget)
					nMax = nMid;
				else
				{
					if (nMin == nMid)
					{
						WrapToolBar(pData, nCount, nMax);
						break;
					}
					nMin = nMid;
				}
			}
		}
		CSize size = CalcSize(pData, nCount);
		WrapToolBar(pData, nCount, size.cx);
	}
	else
	{
		CSize sizeMax, sizeMin, sizeMid;

		// Wrap ToolBar vertically
		WrapToolBar(pData, nCount, 0);
		sizeMin = CalcSize(pData, nCount);

		// Wrap ToolBar horizontally
		WrapToolBar(pData, nCount, 32767);
		sizeMax = CalcSize(pData, nCount);

		while (sizeMin.cx < sizeMax.cx)
		{
			sizeMid.cx = (sizeMin.cx + sizeMax.cx) / 2;
			WrapToolBar(pData, nCount, sizeMid.cx);
			sizeMid = CalcSize(pData, nCount);

			if (nLength < sizeMid.cy)
			{
				if (sizeMin == sizeMid)
				{
					WrapToolBar(pData, nCount, sizeMax.cx);
					return;
				}
				sizeMin = sizeMid;
			}
			else if (nLength > sizeMid.cy)
				sizeMax = sizeMid;
			else
				return;
		}
	}
}

CSize CCJToolBar::CalcLayout(DWORD dwMode, int nLength)
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));
	if (dwMode & LM_HORZDOCK)
		ASSERT(dwMode & LM_HORZ);

	int nCount;
	TBBUTTON* pData = NULL;
	CSize sizeResult(0,0);

	//BLOCK: Load Buttons
	{
		nCount = DefWindowProc(TB_BUTTONCOUNT, 0, 0);
		if (nCount != 0)
		{
			int i;
			pData = new TBBUTTON[nCount];
			for (i = 0; i < nCount; i++)
				GetButton(i, &pData[i]);
		}
	}

	if (nCount > 0)
	{
		if (!(m_dwStyle & CBRS_SIZE_FIXED))
		{
			BOOL bDynamic = m_dwStyle & CBRS_SIZE_DYNAMIC;

			if (bDynamic && (dwMode & LM_MRUWIDTH))
				SizeToolBar(pData, nCount, m_nMRUWidth);
			else if (bDynamic && (dwMode & LM_HORZDOCK))
				SizeToolBar(pData, nCount, 32767);
			else if (bDynamic && (dwMode & LM_VERTDOCK))
				SizeToolBar(pData, nCount, 0);
			else if (bDynamic && (nLength != -1))
			{
				CRect rect; rect.SetRectEmpty();
				CalcInsideRect(rect, (dwMode & LM_HORZ));
				BOOL bVert = (dwMode & LM_LENGTHY);
				int nLen = nLength + (bVert ? rect.Height() : rect.Width());

				SizeToolBar(pData, nCount, nLen, bVert);
			}
			else if (bDynamic && (m_dwStyle & CBRS_FLOATING))
				SizeToolBar(pData, nCount, m_nMRUWidth);
			else
				SizeToolBar(pData, nCount, (dwMode & LM_HORZ) ? 32767 : 0);
		}

		sizeResult = CalcSize(pData, nCount);

		if (dwMode & LM_COMMIT)
		{
			CJX_CONTROLPOS* pControl = NULL;
			int nControlCount = 0;
			BOOL bIsDelayed = m_bDelayedButtonLayout;
			m_bDelayedButtonLayout = FALSE;

			for (int i = 0; i < nCount; i++)
				if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
					nControlCount++;

			if (nControlCount > 0)
			{
				pControl = new CJX_CONTROLPOS[nControlCount];
				nControlCount = 0;

				for(int i = 0; i < nCount; i++)
				{
					if ((pData[i].fsStyle & TBSTYLE_SEP) && (pData[i].idCommand != 0))
					{
						pControl[nControlCount].nIndex = i;
						pControl[nControlCount].nID = pData[i].idCommand;

						CRect rect;
						GetItemRect(i, &rect);
						ClientToScreen(&rect);
						pControl[nControlCount].rectOldPos = rect;

						nControlCount++;
					}
				}
			}

			if ((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
				m_nMRUWidth = sizeResult.cx;
			for (int i = 0; i < nCount; i++)
				SetButton(i, &pData[i]);

			if (nControlCount > 0)
			{
				for (int i = 0; i < nControlCount; i++)
				{
					CWnd* pWnd = GetDlgItem(pControl[i].nID);
					if (pWnd != NULL)
					{
						CRect rect;
						pWnd->GetWindowRect(&rect);
						CPoint pt = rect.TopLeft() - pControl[i].rectOldPos.TopLeft();
						GetItemRect(pControl[i].nIndex, &rect);
						pt = rect.TopLeft() + pt;
						pWnd->SetWindowPos(NULL, pt.x, pt.y, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
					}
				}
				delete[] pControl;
			}
			m_bDelayedButtonLayout = bIsDelayed;
		}
		delete[] pData;
	}

	//BLOCK: Adjust Margins
	{
		CRect rect; rect.SetRectEmpty();
		CalcInsideRect(rect, (dwMode & LM_HORZ));
		sizeResult.cy -= rect.Height();
		sizeResult.cx -= rect.Width();

		CSize size = CCJToolBarBase::CalcFixedLayout((dwMode & LM_STRETCH), (dwMode & LM_HORZ));
		sizeResult.cx = max(sizeResult.cx, size.cx);
		sizeResult.cy = max(sizeResult.cy, size.cy);
	}
	return sizeResult;
}

CSize CCJToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	DWORD dwMode = bStretch ? LM_STRETCH : 0;
	dwMode |= bHorz ? LM_HORZ : 0;

	return CalcLayout(dwMode);
}

CSize CCJToolBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	if ((nLength == -1) && !(dwMode & LM_MRUWIDTH) && !(dwMode & LM_COMMIT) &&
		((dwMode & LM_HORZDOCK) || (dwMode & LM_VERTDOCK)))
	{
		return CalcFixedLayout(dwMode & LM_STRETCH, dwMode & LM_HORZDOCK);
	}
	return CalcLayout(dwMode, nLength);
}

void CCJToolBar::GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	TBBUTTON button;
	GetButton(nIndex, &button);
	nID = button.idCommand;
	nStyle = MAKELONG(button.fsStyle, button.fsState);
	iImage = button.iBitmap;
}

void CCJToolBar::SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage)
{
	ASSERT_VALID(this);

	TBBUTTON button;
	GetButton(nIndex, &button);
	TBBUTTON save;
	memcpy(&save, &button, sizeof(save));
	button.idCommand = nID;
	button.iBitmap = iImage;
	button.fsStyle = (BYTE)LOWORD(nStyle);
	button.fsState = (BYTE)HIWORD(nStyle);
	if (memcmp(&save, &button, sizeof(save)) != 0)
	{
		SetButton(nIndex, &button);
		m_bDelayedButtonLayout = TRUE;
	}
}

int CCJToolBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// check child windows first by calling CCJToolBarBase
	int nHit = CCJToolBarBase::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	// now hit test against CCJToolBar buttons
	CCJToolBar* pBar = (CCJToolBar*)this;
	int nButtons = (int)pBar->DefWindowProc(TB_BUTTONCOUNT, 0, 0);
	for (int i = 0; i < nButtons; i++)
	{
		CRect rect;
		TBBUTTON button;
		if (pBar->DefWindowProc(TB_GETITEMRECT, i, (LPARAM)&rect))
		{
			++rect.bottom;
			++rect.right;
			if (rect.PtInRect(point) &&
				pBar->DefWindowProc(TB_GETBUTTON, i, (LPARAM)&button) &&
				!(button.fsStyle & TBSTYLE_SEP))
			{
				int nHit = GetItemID(i);
				if (pTI != NULL && pTI->cbSize >= sizeof(AFX_OLDTOOLINFO))
				{
					pTI->hwnd = m_hWnd;
					pTI->rect = rect;
					pTI->uId = nHit;
					pTI->lpszText = LPSTR_TEXTCALLBACK;
				}
				// found matching rect, return the ID of the button
				return nHit != 0 ? nHit : -1;
			}
		}
	}
	return -1;
}

BOOL CCJToolBar::SetButtonText(int nIndex, LPCTSTR lpszText)
{
	// attempt to lookup string index in map
	int nString = -1;
	void* p;
	if (m_pStringMap != NULL && m_pStringMap->Lookup(lpszText, p))
		nString = (int)p;

	// add new string if not already in map
	if (nString == -1)
	{
		// initialize map if necessary
		if (m_pStringMap == NULL)
			m_pStringMap = new CMapStringToPtr;

		// add new string to toolbar list
		CString strTemp(lpszText, lstrlen(lpszText)+1);
		nString = (int)DefWindowProc(TB_ADDSTRING, 0, (LPARAM)(LPCTSTR)strTemp);
		if (nString == -1)
			return FALSE;

		// cache string away in string map
		m_pStringMap->SetAt(lpszText, (void*)nString);
		ASSERT(m_pStringMap->Lookup(lpszText, p));
	}

	// change the toolbar button description
	TBBUTTON button;
	GetButton(nIndex, &button);
	button.iString = nString;
	SetButton(nIndex, &button);

	return TRUE;
}

CString CCJToolBar::GetButtonText(int nIndex) const
{
	CString strResult;
	GetButtonText(nIndex, strResult);
	return strResult;
}

void CCJToolBar::GetButtonText(int nIndex, CString& rString) const
{
	if (m_pStringMap != NULL)
	{
		// get button information (need button.iString)
		TBBUTTON button;
		GetButton(nIndex, &button);

		// look in map for matching iString
		POSITION pos = m_pStringMap->GetStartPosition();
		CString str; void* p;
		while (pos)
		{
			m_pStringMap->GetNextAssoc(pos, str, p);
			if ((int)p == button.iString)
			{
				rString = str;
				return;
			}
		}
	}
	rString.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar message handlers

BEGIN_MESSAGE_MAP(CCJToolBar, CCJToolBarBase)
	//{{AFX_MSG_MAP(CCJToolBar)
	ON_WM_NCHITTEST()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SYSCOLORCHANGE()
	ON_MESSAGE(TB_SETBUTTONSIZE, OnSetButtonSize)
	ON_MESSAGE(TB_SETBITMAPSIZE, OnSetBitmapSize)
	ON_WM_NCCREATE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(WM_SETTINGCHANGE, OnPreserveZeroBorderHelper)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnPreserveZeroBorderHelper)
	ON_NOTIFY_REFLECT(TBN_DROPDOWN,		 OnToolBarBtnDropDown)
	ON_NOTIFY_REFLECT(TBN_BEGINADJUST,	 OnToolBarBeginAdjust)
	ON_NOTIFY_REFLECT(TBN_BEGINDRAG,	 OnToolBarBeginDrag)
	ON_NOTIFY_REFLECT(TBN_CUSTHELP,		 OnToolBarCustomHelp)
	ON_NOTIFY_REFLECT(TBN_ENDADJUST,	 OnToolBarEndAdjust)
	ON_NOTIFY_REFLECT(TBN_ENDDRAG,		 OnToolBarEndDrag)
	ON_NOTIFY_REFLECT(TBN_GETBUTTONINFO, OnToolBarGetButtonInfo)
	ON_NOTIFY_REFLECT(TBN_QUERYDELETE,	 OnToolBarQueryDelete)
	ON_NOTIFY_REFLECT(TBN_QUERYINSERT,	 OnToolBarQueryInsert)
	ON_NOTIFY_REFLECT(TBN_RESET,		 OnToolBarReset)
	ON_NOTIFY_REFLECT(TBN_TOOLBARCHANGE, OnToolBarChange)
END_MESSAGE_MAP()

////////////////
// Override to avoid MFC in case I'm inside a ReBar
//
BOOL CCJToolBar::OnEraseBkgnd(CDC*)
{
	return (BOOL)Default();
}

#if (_MFC_VER < 0x0900)
UINT CCJToolBar::OnNcHitTest(CPoint)
#else
LRESULT CCJToolBar::OnNcHitTest(CPoint)
#endif   // _MFC_VER
{
	return HTCLIENT;
}

//////////////////
// Calcluate size of client area. Adds room for grippers
//
void CCJToolBar::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS* lpncsp)
{
	// calculate border space (will add to top/bottom, subtract from right/bottom)
	CRect rect; rect.SetRectEmpty();
	BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
	CCJToolBarBase::CalcInsideRect(rect, bHorz);
	ASSERT(_ComCtlVersion != -1);
	ASSERT(_ComCtlVersion >= VERSION_IE4 || rect.top >= 2);

	// adjust non-client area for border space
	lpncsp->rgrc[0].left += rect.left;
	lpncsp->rgrc[0].top += rect.top;
	// previous versions of COMCTL32.DLL had a built-in 2 pixel border
	if (_ComCtlVersion < VERSION_IE4)
		lpncsp->rgrc[0].top -= 2;
	lpncsp->rgrc[0].right += rect.right;
	lpncsp->rgrc[0].bottom += rect.bottom;
}

//////////////////
// **PD**
// Part 2 of correction for MFC is to recalculate everything when the bar
// goes from docked to undocked because the AdjustSize calculation happens
// when the bar is in the old state, and thus wrong. After the bar is
// docked/undocked, I'll recalculate with the new style and commit the change.
//
void CCJToolBar::OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle)
{
	// a dynamically resizeable toolbar can not have the CBRS_FLOAT_MULTI
	ASSERT(!((dwNewStyle & CBRS_SIZE_DYNAMIC) &&
		(m_dwDockStyle & CBRS_FLOAT_MULTI)));
	
	// a toolbar can not be both dynamic and fixed in size
	ASSERT (!((dwNewStyle & CBRS_SIZE_FIXED) &&
		(dwNewStyle & CBRS_SIZE_DYNAMIC)));
	
	// CBRS_SIZE_DYNAMIC can not be disabled once it has been enabled
	ASSERT (((dwOldStyle & CBRS_SIZE_DYNAMIC) == 0) ||
		((dwNewStyle & CBRS_SIZE_DYNAMIC) != 0));
	
	if (m_hWnd != NULL &&
		((dwOldStyle & CBRS_BORDER_ANY) != (dwNewStyle & CBRS_BORDER_ANY)))
	{
		// recalc non-client area when border styles change
		SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
	}
	m_bDelayedButtonLayout = TRUE;

	if (dwOldStyle != dwNewStyle) {
		DWORD dwMode = 0;
		if ((dwNewStyle & CBRS_SIZE_DYNAMIC) && (dwNewStyle & CBRS_FLOATING))
			dwMode = LM_HORZ | LM_MRUWIDTH;
		else if (dwNewStyle & CBRS_ORIENT_HORZ)
			dwMode = LM_HORZ | LM_HORZDOCK;
		else
			dwMode =  LM_VERTDOCK;

		CalcDynamicLayout(-1, dwMode | LM_COMMIT);
	}
}

void CCJToolBar::OnNcPaint()
{
	EraseNonClient();
}

void CCJToolBar::OnWindowPosChanging(LPWINDOWPOS lpwndpos)
{
	// not necessary to invalidate the borders
	DWORD dwStyle = m_dwStyle;
	m_dwStyle &= ~(CBRS_BORDER_ANY);
	CCJToolBarBase::OnWindowPosChanging(lpwndpos);
	m_dwStyle = dwStyle;
}

void CCJToolBar::OnPaint()
{
	if (m_bDelayedButtonLayout)
		Layout();

	Default();
}

LRESULT CCJToolBar::OnSetButtonSize(WPARAM, LPARAM lParam)
{
	return OnSetSizeHelper(m_sizeButton, lParam);
}

LRESULT CCJToolBar::OnSetBitmapSize(WPARAM, LPARAM lParam)
{
	return OnSetSizeHelper(m_sizeImage, lParam);
}

LRESULT CCJToolBar::OnSetSizeHelper(CSize& size, LPARAM lParam)
{
	//WINBUG: The IE4 version of COMCTL32.DLL supports a zero border, but
	//  only if TBSTYLE_TRANSPARENT is on during the the TB_SETBITMAPSIZE
	//  and/or TB_SETBUTTONSIZE messages.  In order to enable this feature
	//  all the time (so we get consistent border behavior, dependent only
	//  on the version of COMCTL32.DLL) we turn on TBSTYLE_TRANSPARENT
	//  whenever these messages go through.  It would be nice that in a
	//  future version, the system toolbar would just allow you to set
	//  the top and left borders to anything you please.

	BOOL bModify = FALSE;
	ASSERT(_ComCtlVersion != -1);
	DWORD dwStyle = 0;
	if (_ComCtlVersion >= VERSION_IE4)
	{
		dwStyle = GetStyle();
		bModify = ModifyStyle(0, TBSTYLE_TRANSPARENT|TBSTYLE_FLAT);
	}

	LRESULT lResult = Default();
	if (lResult)
		size = lParam;

	if (bModify)
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	return lResult;
}

LRESULT CCJToolBar::OnPreserveZeroBorderHelper(WPARAM, LPARAM lParam)
{
	UNUSED_ALWAYS( lParam );

	BOOL bModify = FALSE;
	ASSERT(_ComCtlVersion != -1);
	DWORD dwStyle = 0;
	if (_ComCtlVersion >= VERSION_IE4)
	{
		dwStyle = GetStyle();
		bModify = ModifyStyle(0, TBSTYLE_TRANSPARENT|TBSTYLE_FLAT);
	}

	LRESULT lResult = Default();

	if (bModify)
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);

	return lResult;
}

void CCJToolBar::OnSysColorChange()
{
	// re-color bitmap for toolbar
	if (m_hInstImageWell != NULL && m_hbmImageWell != NULL)
	{
		HBITMAP hbmNew;
		hbmNew = _LoadSysColorBitmap(m_hInstImageWell, m_hRsrcImageWell);
		if (hbmNew != NULL)
			AddReplaceBitmap(hbmNew);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar idle update through CCJToolCmdUI class

class CCJToolCmdUI : public CCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void Enable(BOOL bOn);
	virtual void SetCheck(int nCheck);
	virtual void SetText(LPCTSTR lpszText);
};

void CCJToolCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
	CCJToolBar* pToolBar = (CCJToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CCJToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) & ~TBBS_DISABLED;
	if (!bOn)
	{
		nNewStyle |= TBBS_DISABLED;
		// WINBUG: If a button is currently pressed and then is disabled
		// COMCTL32.DLL does not unpress the button, even after the mouse
		// button goes up!  We work around this bug by forcing TBBS_PRESSED
		// off when a button is disabled.
		nNewStyle &= ~TBBS_PRESSED;
	}
	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle);
}

void CCJToolCmdUI::SetCheck(int nCheck)
{
	ASSERT(nCheck >= 0 && nCheck <= 2); // 0=>off, 1=>on, 2=>indeterminate
	CCJToolBar* pToolBar = (CCJToolBar*)m_pOther;
	ASSERT(pToolBar != NULL);
	ASSERT_KINDOF(CCJToolBar, pToolBar);
	ASSERT(m_nIndex < m_nIndexMax);

	UINT nNewStyle = pToolBar->GetButtonStyle(m_nIndex) &
				~(TBBS_CHECKED | TBBS_INDETERMINATE);
	if (nCheck == 1)
		nNewStyle |= TBBS_CHECKED;
	else if (nCheck == 2)
		nNewStyle |= TBBS_INDETERMINATE;
	ASSERT(!(nNewStyle & TBBS_SEPARATOR));
	pToolBar->SetButtonStyle(m_nIndex, nNewStyle | TBBS_CHECKBOX);
}

void CCJToolCmdUI::SetText(LPCTSTR)
{
	// ignore it
}

void CCJToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
	CCJToolCmdUI state;
	state.m_pOther = this;

	state.m_nIndexMax = (UINT)DefWindowProc(TB_BUTTONCOUNT, 0, 0);
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; state.m_nIndex++)
	{
		// get buttons state
		TBBUTTON button;
		GetButton(state.m_nIndex, &button);
		state.m_nID = button.idCommand;

		// ignore separators
		if (!(button.fsStyle & TBSTYLE_SEP))
		{
			// allow reflections
			if (CWnd::OnCmdMsg(0,
				MAKELONG((int)CN_UPDATE_COMMAND_UI, WM_COMMAND+WM_REFLECT_BASE),
				&state, NULL))
				continue;

			// allow the toolbar itself to have update handlers
			if (CWnd::OnCmdMsg(state.m_nID, CN_UPDATE_COMMAND_UI, &state, NULL))
				continue;

			// allow the owner to process the update
			state.DoUpdate(pTarget, bDisableIfNoHndler);
		}
	}

	// update the dialog controls added to the toolbar
	UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

/////////////////////////////////////////////////////////////////////////////
// CCJToolBar diagnostics

#ifdef _DEBUG
void CCJToolBar::AssertValid() const
{
	// Note: CCJToolBarBase::AssertValid is not called because it checks for
	//  m_nCount and m_pData to be in sync, which they are not in CCJToolBar.

#if (_MFC_VER < 0x0600)
	ASSERT(m_hbmImageWell == NULL ||
		( ::GetObjectType(m_hbmImageWell) == OBJ_BITMAP));
#elif (_MFC_VER < 0x0900)
	ASSERT(m_hbmImageWell == NULL ||
		( afxData.bWin95 || ::GetObjectType(m_hbmImageWell) == OBJ_BITMAP));
#else
	ASSERT(m_hbmImageWell == NULL ||
		( (::GetVersion() & 0x80000000) >= 4 || ::GetObjectType(m_hbmImageWell) == OBJ_BITMAP));
#endif   // _MFC_VER

	if (m_hInstImageWell != NULL && m_hbmImageWell != NULL)
		ASSERT(m_hRsrcImageWell != NULL);
}

void CCJToolBar::Dump(CDumpContext& dc) const
{
	CCJToolBarBase::Dump(dc);

	dc << "m_hbmImageWell = " << (UINT)m_hbmImageWell;
	dc << "\nm_hInstImageWell = " << (UINT)m_hInstImageWell;
	dc << "\nm_hRsrcImageWell = " << (UINT)m_hRsrcImageWell;
	dc << "\nm_sizeButton = " << m_sizeButton;
	dc << "\nm_sizeImage = " << m_sizeImage;

	if (dc.GetDepth() > 0)
	{
		CCJToolBar* pBar = (CCJToolBar*)this;
		int nCount = pBar->DefWindowProc(TB_BUTTONCOUNT, 0, 0);
		for (int i = 0; i < nCount; i++)
		{
			TBBUTTON button;
			GetButton(i, &button);
			dc << "\ntoolbar button[" << i << "] = {";
			dc << "\n\tnID = " << button.idCommand;
			dc << "\n\tnStyle = " << MAKELONG(button.fsStyle, button.fsState);
			if (button.fsStyle & TBSTYLE_SEP)
				dc << "\n\tiImage (separator width) = " << button.iBitmap;
			else
				dc <<"\n\tiImage (bitmap image index) = " << button.iBitmap;
			dc << "\n}";
		}
	}

	dc << "\n";
}
#endif

#ifdef AFX_INIT_SEG
#pragma code_seg(AFX_INIT_SEG)
#endif

IMPLEMENT_DYNAMIC(CCJToolBar, CCJToolBarBase)

/////////////////////////////////////////////////////////////////////////////
