// CJCaptionPopupWnd.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:44p $
 * $Revision: 5 $
 * $Archive: /CodeJock/Include/CJCaptionPopupWnd.h $
 *
 * $History: CJCaptionPopupWnd.h $
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:44p
 * Updated in $/CodeJock/Include
 * Modified resource include for static builds.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 9:46p
 * Created in $/CodeJockey/Include
 * Initial creation. Added popup class as a helper class for the
 * CCJCaption class. This class is used to display a popup window which
 * contains a child CWnd derived object.
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJCAPTIONPOPUPWND_H__
#define __CJCAPTIONPOPUPWND_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCJCaptionPopupWnd window

#include "CJCaption.h"
#include "CJFlatButton.h"

// caption popup window messages
#define CM_ONPUSHPINBUTTON   WM_USER + 1001        // push pin clicked.
#define CM_ONPUSHPINCANCEL   WM_USER + 1002        // push pin clicked.

// CCJCaptionPopupWnd is a CWnd derived class which is used by
// the CCJCaption class to display a popup child window simular to Outlook
class _CJX_EXT_CLASS CCJCaptionPopupWnd : public CWnd
{
// Construction
public:
	CCJCaptionPopupWnd();

// Attributes
public:
	CWnd*			m_pParentWnd;
	CWnd*			m_pChildWnd;
	CWnd*			m_pChildParent;
protected:
	CCJCaption		m_Caption;
	CCJFlatButton	m_CaptionButton;
	HICON			m_hIconOn;		// handle to normal button icon.
	HICON			m_hIconOff;		// handle to pressed button icon.

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJCaptionPopupWnd)
	public:
	virtual BOOL Create(const CRect& rect, CWnd* pParentWnd, CWnd* pChildWnd);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJCaptionPopupWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJCaptionPopupWnd)
	afx_msg void OnDestroy();
	afx_msg void OnCaptButton();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__CJCAPTIONPOPUPWND_H__)
