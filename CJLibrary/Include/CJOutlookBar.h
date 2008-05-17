// CJOutlookBar.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 2:19p $
 * $Revision: 11 $
 * $Archive: /CodeJock/Include/CJOutlookBar.h $
 *
 * $History: CJOutlookBar.h $
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:19p
 * Updated in $/CodeJock/Include
 * Added COLORREF members to class to eliminate continuous calls to
 * GetSysColor(...). Overrode OnSysColorChange() to re-initialize COLORREF
 * variables. 
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:45p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 3:16a
 * Updated in $/CodeJockey/Include
 * Added ResetContent() thanks to Hiren Patel [hpatel@spss.com] for
 * pointing this out.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 10:43p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 3/03/99    Time: 10:43p
 * Updated in $/CodeJockey/Include
 * Changed images list's to pointer vars passed into class.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 2/24/99    Time: 10:42p
 * Updated in $/CodeJockey/Include
 * Christopher Brown (Allknowing@earthlink.net) some modifications to
 * correct icon resource leak, and eliminate screen flicker.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 10:42p
 * Created in $/CodeJockey/Include
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJOUTLOOKBAR_H__
#define __CJOUTLOOKBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CContentItems is a utility class used as a Container for 
// CJOutlookBar menu items.
class _CJX_EXT_CLASS CContentItems
{
public:

	// non-default constructor used to initialize
	// member data
	//
	CContentItems(UINT nID, CString str);

protected:

	UINT	m_nImageID;	// index of menu item
	CString m_csText;	// text of menu item

public:

	// overloaded operator which copies a CContentItem object
	// 
	void operator = (CContentItems& pItems);

	// returns the index of a menu item
	//
	virtual UINT GetImageID();

	// returns the text of a menu item
	//
	virtual CString GetText();
};

// CJOutlookBar is a CListBox derived class which simulates a control simular
// to the Outlook bar as seen in Outlook, can only be used with the
// LBS_OWNERDRAWVARIABLE style bit set. very generic 
class _CJX_EXT_CLASS CCJOutlookBar : public CListBox
{
	DECLARE_DYNAMIC(CCJOutlookBar)

public:

	// Default constructor
	//
	CCJOutlookBar();

	// Virtual destructor
	//
	virtual ~CCJOutlookBar();

protected:

	CWnd*			m_pOwner;			// points to the parent window for this control see SetOwner(...)
    CPoint          m_point;			// holds the cursor position
	CImageList*		m_pImageListLarge;	// points to the small image list for this control see SetImageLists(...)
	CImageList*		m_pImageListSmall;	// points to the large image list for this control see SetImageLists(...)
	CContentItems*  m_pContents;		// points to a CContentItem array see SetItems(...)
    int             m_nNumItems;		// number of menu items in this control see SetItems(...)
	int             m_nIndex;			// currently selected menu index
    bool            m_bHilight;			// true when menu item is selected
    bool            m_bLBDown;			// true when left mouse button is pressed
	CFont			m_Font;				// font used by the control.
	COLORREF		m_clrHilite;		// set to ::GetSysColor(COLOR_BTNHIGHLIGHT)
	COLORREF		m_clrShadow;		// set to ::GetSysColor(COLOR_BTNSHADOW)
	COLORREF		m_clrDkShad;		// set to ::GetSysColor(COLOR_3DDKSHADOW)
	COLORREF		m_clrNormal;		// set to ::GetSysColor(COLOR_BTNFACE)
	COLORREF		m_clrTextNm;		// set to ::GetSysColor(COLOR_WINDOW)

public:

	// sets the large and small image lists for the control. currently
	// only large image list is implemented
	//
	virtual void SetImageLists(CImageList* pImageListSmall=NULL, CImageList* pImageListLarge=NULL);

	// sets the CContentItem array and m_nNumItems member variables
	//
	virtual void SetItems(CContentItems* pItems, int nNumItems);

	// sets the window that will receive messages from this control
	//
	virtual void SetOwner(CWnd* pWnd);

	// called by the control to initialize the controls font.
	//
	virtual void UpdateFont();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJOutlookBar)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CCJOutlookBar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions for CContentItems
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE CContentItems::CContentItems( UINT nID, CString str )
	{ m_nImageID = nID; m_csText = str; }

_CJXLIB_INLINE CString CContentItems::GetText()
	{ return m_csText; }

_CJXLIB_INLINE UINT CContentItems::GetImageID()
	{ return m_nImageID; }

_CJXLIB_INLINE void CContentItems::operator = ( CContentItems& pItems )
	{ m_nImageID = pItems.m_nImageID; m_csText = pItems.m_csText; }

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions for CCJOutlookBar
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJOutlookBar::SetImageLists(CImageList* pImageListSmall, CImageList* pImageListLarge)
	{ m_pImageListLarge = pImageListLarge; m_pImageListSmall = pImageListSmall; }

_CJXLIB_INLINE void CCJOutlookBar::SetItems(CContentItems * pItems, int nNumItems)
	{ m_pContents = pItems; m_nNumItems = nNumItems; ResetContent(); for (int i = 0; i < nNumItems; ++i) AddString(m_pContents[i].GetText()); }

_CJXLIB_INLINE void CCJOutlookBar::SetOwner(CWnd* pWnd)
	{ m_pOwner = pWnd; }

#define OBN_SELENDOK	(WM_USER + 1)	// for message handler
#define OB_CYBUTTON		70				// average button height

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJOUTLOOKBAR_H__

