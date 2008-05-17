// CJLogoPane.h : header file
//
// Copyright © 1999 Pierre MELLINAND
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/26/99 11:08p $
 * $Revision: 1 $
 * $Archive: /CodeJock/Include/CJLogoPane.h $
 *
 * $History: CJLogoPane.h $
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 11:08p
 * Created in $/CodeJock/Include
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJLOGOPANE_H__
#define __CJLOGOPANE_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJLogoPane class

class _CJX_EXT_CLASS CCJLogoPane : public CWnd
{
	DECLARE_DYNCREATE(CCJLogoPane)

// Construction
public:
	CCJLogoPane();

// Attributes
public:
	CString m_strLogoText;
	CFont 	m_Font;

// Operations
public:
	virtual void	SetLogoText(LPCTSTR lpszLogoText);
	virtual CString GetLogoText();
	virtual void	SetLogoFont(LPCTSTR lpszFontName, int nHeight=24, int nWeight=FW_BOLD, BOOL bItalic=TRUE, BOOL bUnderline=FALSE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJLogoPane)
	public:
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, CWnd* pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJLogoPane();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJLogoPane)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJLogoPane::SetLogoText(LPCTSTR lpszLogoText)
	{ m_strLogoText = lpszLogoText; }

_CJXLIB_INLINE CString CCJLogoPane::GetLogoText()
	{ return m_strLogoText; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJLOGOPANE_H__

