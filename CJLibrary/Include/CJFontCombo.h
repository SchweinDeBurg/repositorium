/////////////////////////////////////////////////////////////////////////////
// CJFontCombo.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the article 'Owner Drawn Font Selection ComboBox' by Girish Bharadwaj
// http://www.codeguru.com/misc/font_selection_combo.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:01p $
 * $Revision: 4 $
 * $Archive: /CodeJock/Include/CJFontCombo.h $
 *
 * $History: CJFontCombo.h $
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:01p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:41p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes. Added the
 * DECLARE_DYNAMIC and IMPLEMENT_DYNAMIC macros to CCJFontCombo for access
 * to run-time information
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/26/99    Time: 2:11p
 * Created in $/CodeJockey/Include
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJFONTCOMBO_H__
#define __CJFONTCOMBO_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

///////////////////////////////////////////////////////////////////////////
// CCJFontCombo window
class _CJX_EXT_CLASS CCJFontCombo : public CComboBox
{
	DECLARE_DYNAMIC(CCJFontCombo)

// Construction
public:
     CCJFontCombo();

// Attributes
protected:

	COLORREF	m_clrHilight;
	COLORREF	m_clrNormalText;
	COLORREF	m_clrHilightText;
	COLORREF	m_clrBkgnd;

// Operations
public:

	virtual int GetSelFont(LOGFONT& logFont);
	virtual void SetHilightColors(COLORREF clrHilight,COLORREF clrHilightText);
	virtual void SetNormalColors(COLORREF clrBkgnd,COLORREF clrNormalText);
	static BOOL CALLBACK EnumFontProc(LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetric, short nFontType, LPHANDLE lpData);

protected:

	virtual void InitializeFonts();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFontCombo)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
     virtual ~CCJFontCombo();

// Generated message map functions
protected:
	
	//{{AFX_MSG(CCJFontCombo)
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJFontCombo::SetHilightColors( COLORREF clrHilight, COLORREF clrHilightText )
	{ m_clrHilight = clrHilight; m_clrHilightText = clrHilightText; };

_CJXLIB_INLINE void CCJFontCombo::SetNormalColors( COLORREF clrBkgnd, COLORREF clrNormalText )
	{ m_clrNormalText = clrNormalText; m_clrBkgnd = clrBkgnd; };

///////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJFONTCOMBO_H__
