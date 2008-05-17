// CJMaskEdit.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the article "A masked edit control (2)" by Jeremy Davis [jmd@jvf.co.uk]
// http://www.codeguru.com/editctrl/masked_edit2.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 1:53p $
 * $Revision: 11 $
 * $Archive: /CodeJock/CJLibrary/CJMaskEdit.cpp $
 *
 * $History: CJMaskEdit.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 1:53p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) and OnPaint(...) to handle flicker free
 * drawing.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
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
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/19/99    Time: 12:35a
 * Created in $/CodeJockey/CJLibrary
 * Initial creation and addition to library.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJMaskEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMaskEdit

CCJMaskEdit::CCJMaskEdit()
{
	m_bUseMask				= true;
	m_bMaskKeyInProgress	= false;
	m_strWindowText			= _T("");
	m_strMask				= _T("");
	m_strLiteral			= _T("");
	m_strValid				= _T("");
	m_strMaskLiteral		= _T("");
}

CCJMaskEdit::~CCJMaskEdit()
{
	// fix potential resource leak - KStowell - 10-22-99.
	m_Font.DeleteObject();
}

IMPLEMENT_DYNAMIC(CCJMaskEdit, CEdit)

BEGIN_MESSAGE_MAP(CCJMaskEdit, CEdit)
	//{{AFX_MSG_MAP(CCJMaskEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJMaskEdit message handlers

void CCJMaskEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( m_bUseMask )
	{
		switch( nChar )
		{
		case VK_DELETE:
		case VK_INSERT: return;
		}
	}
	
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCJMaskEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( !m_bMaskKeyInProgress ) {
		if( !CheckChar( nChar )) {
			return;
		}
	}
	
	if( m_bUseMask )
	{
		int nStartPos;
		int nEndPos;
		GetSel( nStartPos, nEndPos );

		if( isprint( nChar ))
		{
			SetSel( nStartPos, nEndPos+1 );
			
			Clear();

			m_strWindowText.SetAt( nEndPos, static_cast<TCHAR>(nChar));
		}
		else if( nChar == VK_BACK )
		{
			if(( nStartPos == nEndPos ) && 
			   ( nStartPos >= 1 ) && 
			   ( nStartPos <= m_strWindowText.GetLength( )))
			{
				UINT c = 0x20;
				
				// get the masked literal representation.
				if( m_strMaskLiteral.IsEmpty() == FALSE )
				{
					c = m_strMaskLiteral[ nStartPos - 1 ];
				}

				// backspace the cursor.
				SendMessage( WM_KEYDOWN, VK_LEFT, 0 );

				if( m_strMaskLiteral.IsEmpty() == FALSE )
				{
					// update the char backspacing over.
					SendChar( c );

					// backspace the cursor again.
					SendMessage( WM_KEYDOWN, VK_LEFT, 0 );
				}
			}
			else 
			{
				MessageBeep((UINT)-1);
			}

			return;
		}
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	if( !m_bMaskKeyInProgress && m_bUseMask && !m_strLiteral.IsEmpty( ))
	{
		int nStartPos;
		int nEndPos;
		GetSel( nStartPos, nEndPos );

		// make sure the string is not longer than the mask
		if( nEndPos < m_strLiteral.GetLength( ))
		{
			UINT c = m_strLiteral.GetAt( nEndPos );
			if (c != '_') {
				SetSel( nEndPos+1, nEndPos+1 );
			}
		}
	}
}

bool CCJMaskEdit::CheckChar(UINT nChar)
{
	// do not use mask
	if( !m_bUseMask ) {
		return true;
	}
	
	// control character, OK
	if( !isprint( nChar )) {
		return true;
	}
	
	// unselect all selections, if any
	int nStartPos, nEndPos;
	GetSel( nStartPos, nEndPos );
	SetSel( -1, 0 );
	
	// advance to the first character input position.
	for( nStartPos; nStartPos < m_strLiteral.GetLength(); ++nStartPos )
	{
		if( m_strLiteral.GetAt( nStartPos ) == '_' ) {
			SetSel( nStartPos, nStartPos );
			break;
		}
	}
	
	// check the key against the mask
	GetSel( nStartPos, nEndPos );
	
	// make sure the string is not longer than the mask
	if( nEndPos >= m_strMask.GetLength( )) {
		MessageBeep((UINT)-1);
		return false;
	}
	
	// check to see if a literal is in this position
	UINT c = '_';
	if( !m_strLiteral.IsEmpty( )) {
		c = m_strLiteral.GetAt( nEndPos );
	}
	
	if( c != '_' ) {
		SendChar( c );
		GetSel( nStartPos, nEndPos );
	}
	
	// check the valid string character
	if( m_strValid.Find( static_cast<TCHAR>(nChar)) != -1 ) {
		return true;
	}
	
	if( ProcessMask( nChar, nEndPos ))
		return true;

	MessageBeep((UINT)-1);
	return false;
}

void CCJMaskEdit::SendChar(UINT nChar)
{
	m_bMaskKeyInProgress = true;
#ifdef WIN32
    AfxCallWndProc(this, m_hWnd, WM_CHAR, nChar, 1);
#else
    SendMessage(WM_CHAR, nChar, 1);
#endif
	m_bMaskKeyInProgress = false;
}

bool CCJMaskEdit::ProcessMask(UINT nChar, int nEndPos)
{
	// check the key against the mask
	switch( m_strMask.GetAt( nEndPos ))
	{
    case '0':		// digit only //completely changed this
		{
			if( isdigit( nChar )) {
				return true;
			}
			break;
		}
    case '9':		// digit or space
		{
			if( isdigit( nChar )) {
				return true;
			}
			if( nChar == VK_SPACE ) {
				return true;
			}
			break;
		}
    case '#':		// digit or space or '+' or '-'
		{
			if( isdigit( nChar )) {
				return true;
			}
			if( nChar == VK_SPACE || nChar == VK_ADD || nChar == VK_SUBTRACT ) {
				return true;
			}
			break;
		}
    case 'L':		// alpha only
		{
			if( isalpha( nChar )) {
				return true;
			}
			break;
		}
    case '?':		// alpha or space
		{
			if( isalpha( nChar )) {
				return true;
			}
			if( nChar == VK_SPACE ) {
				return true;
			}
			break;
		}
    case 'A':		// alpha numeric only
		{
			if( isalnum( nChar )) {
				return true;
			}
			break;
		}
    case 'a':		// alpha numeric or space
		{
			if( isalnum( nChar )) {
				return true;
			}
			if( nChar == VK_SPACE ) {
				return true;
			}
			break;
		}
    case '&':		// all print character only
		{
			if( isprint( nChar )) {
				return true;
			}
			break;
		}
    case 'H':		// hex digit
		{
			if( isxdigit( nChar )) {
				return true;
			}
			break;
		}
	case 'X':		// hex digit or space
		{
			if( isxdigit( nChar )) {
				return true;
			}
			if( nChar == VK_SPACE ) {
				return true;
			}
			break;
		}
	}

	return false;
}

void CCJMaskEdit::SetEditMask(LPCTSTR lpszMask, LPCTSTR lpszLiteral, LPCTSTR lpszWindowText)
{
	ASSERT(lpszMask);
	ASSERT(lpszLiteral);
	ASSERT(lpszWindowText);

	// initialize the mask for the control.
	m_strMask        = lpszMask;
	m_strLiteral     = lpszLiteral;
	m_strWindowText  = lpszWindowText;
	m_strMaskLiteral = m_strWindowText;

	// set the window text for the control.
	SetWindowText( lpszWindowText );

	// initialize the font used by the control.
	UpdateFont();
}

/////////////////////////////////////////////////////////////////////////////
// DDX for date mask control

void AFXAPI DDX_OleDateTime(CDataExchange* pDX, int nIDC, CCJDateEdit& rControl, COleDateTime& rDateTime)
{
	DDX_Control( pDX, nIDC, rControl );
	
	if( pDX->m_bSaveAndValidate ) {
		rDateTime = rControl.GetDateTime();
	}
	else {
		rControl.SetDateTime( rDateTime );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCJDateEdit class

IMPLEMENT_DYNAMIC(CCJDateEdit, CCJMaskEdit)

CCJDateEdit::CCJDateEdit()
{
	m_bUseMask		= true;
	m_strMask		= _T("00/00/0000");
	m_strLiteral	= _T("__/__/____");
}

COleDateTime CCJDateEdit::ReadOleDateTime(LPCTSTR lpszData)
{
	COleDateTime dt;
	dt.ParseDateTime(lpszData);
	return dt;
}

void CCJDateEdit::FormatOleDateTime(CString &strData, COleDateTime dt)
{
	strData = dt.Format( _T("%d/%m/%Y"));
}

void CCJDateEdit::SetDateTime(COleDateTime& dt)
{
	CString strText;
	FormatOleDateTime(strText, dt);
	m_strWindowText = m_strMaskLiteral = strText;
	SetWindowText(strText);
}

void CCJDateEdit::SetDateTime(CString strDate)
{
	m_strWindowText = m_strMaskLiteral = strDate;
	SetWindowText(strDate);
}

COleDateTime CCJDateEdit::GetDateTime()
{
	CString strText;
	GetWindowText(strText);
	return ReadOleDateTime(strText);
}

CString CCJDateEdit::GetWindowDateTime()
{
	CString strText;
	GetWindowText(strText);
	return strText;
}

bool CCJDateEdit::ProcessMask(UINT nChar, int nEndPos)
{
	// check the key against the mask
	switch( m_strMask.GetAt( nEndPos ))
	{
    case '0':		// digit only //completely changed this
		{
			bool bReturn = true;

			if( isdigit( nChar ))
			{
				if( nEndPos == 0 )
				{
					if( nChar > '3' ) {
						bReturn = false;
					}
				}
				else if( nEndPos == 1 )
				{
					if( m_strWindowText.GetAt(0) == '3' )
					{
						if( nChar > '1' ) {
							bReturn = false;
						}
					}
				}
				else if( nEndPos == 3 )
				{
					if( nChar > '1' ) {
						bReturn = false;
					}
				}
				else if( nEndPos == 4 )
				{
					if( m_strWindowText.GetAt(3) == '1' )
					{
						if( nChar > '2') {
							bReturn = false;
						}
					}
				}
				return bReturn;
			}
			break;
		}
	}

	MessageBeep((UINT)-1);
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CCJTimeEdit class

IMPLEMENT_DYNAMIC(CCJTimeEdit, CCJDateEdit)

CCJTimeEdit::CCJTimeEdit()
{
	m_bUseMask		= true;
	m_strMask		= _T("00:00");
	m_strLiteral	= _T("__:__");
	m_strHours		= _T("47");
	m_strMins		= _T("59");
}

void CCJTimeEdit::FormatOleDateTime(CString &strData, COleDateTime dt)
{
	if( dt.m_dt == 0 ) { 
		strData = "00:00"; 
	}
	else {
		strData = dt.Format( _T("%H:%M")); 
	}
}

bool CCJTimeEdit::ProcessMask(UINT nChar, int nEndPos)
{
	// check the key against the mask
	switch( m_strMask.GetAt( nEndPos ))
	{
    case '0':		// digit only //completely changed this
		{
			bool bReturn = true;

			if( isdigit( nChar ))
			{
				if( nEndPos == 0 )
				{
					if(nChar > (UINT)m_strHours[0]) {
						bReturn = false;
					}
				}
				else if( nEndPos == 1 )
				{
					if(m_strWindowText.GetAt(0) == m_strHours[0])
					{
						if( nChar > (UINT)m_strHours[1] ) {
							bReturn = false;
						}
					}
				}
				else if( nEndPos == 3 )
				{
					if( nChar > (UINT)m_strMins[0] ) {
						bReturn = false;
					}
				}
				else if( nEndPos == 4 )
				{
					if( m_strWindowText.GetAt(3) == m_strMins[0] )
					{
						if( nChar > (UINT)m_strMins[1] ) {
							bReturn = false;
						}
					}
				}
				return bReturn;
			}
			break;
		}
	}

	MessageBeep((UINT)-1);
	return false;
}

void CCJTimeEdit::SetHours(int nHours)
{
	m_strHours.Format( _T("%d"), nHours);
}

void CCJTimeEdit::SetMins(int nMins)
{
	m_strMins.Format( _T("%d"), nMins);
}

void CCJMaskEdit::UpdateFont()
{
	m_Font.DeleteObject();
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	VERIFY(m_Font.CreateFontIndirect(&ncm.lfMessageFont));
	SetFont(&m_Font);
}

BOOL CCJMaskEdit::OnEraseBkgnd(CDC* pDC) 
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}

void CCJMaskEdit::OnPaint() 
{
    // Use a "Offscreen" DC to fill rect and send to DefWindowProc...  
	CClientDC memDC(this);
	
    CRect rcClip;
    GetClientRect(&rcClip);
    memDC.FillSolidRect(rcClip, ::GetSysColor(COLOR_WINDOW));
    DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0 );
	
    // Default is called for cleanup.
    Default();
}
