// FlatTab.h : header file
//
// Copyright © 1999 Ian Brumby
//
// This source code may be used in compiled form in any way you desire. 
// Source file(s) may be redistributed unmodified by any means PROVIDING
// they are not sold for profit without the authors expressed written consent,
// and providing that this notice and the authors name and all copyright
// notices remain intact.
//
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.4 	21 Jul 1999	- Initial release.
//			1.5		30 Aug 1999 - Several enhancements by Phil Baxter 
//					<Phil.Baxter@mrjinx.demon.co.uk>
// ==========================================================================  

// Excel uses FF_ROMAN and Font Height = ~(control's height - 4)
// Excel draws its tabs right to left
// Excel has 3DLIGHT highlighting on the left diagonal
// Excel has home/end buttons
// Excel's buttons change width as the control's height changes
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 7/06/02 22:21 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/Include/CJFlatTabCtrl.h $
 *
 * $History: CJFlatTabCtrl.h $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 7/06/02    Time: 22:21
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/Include
 * Eliminate warnings when compiled with VC.NET
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:40p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:15a
 * Updated in $/CodeJock/Include
 * Fixed minor bug with tab font creation.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/Include
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:25p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CCJFLATTABCTRL_H__
#define __CCJFLATTABCTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "CJMetaFileButton.h"

// control styles
#define FTS_BOTTOM              0x0002
#define FTS_HASARROWS           0x0004
#define FTS_HASHOMEEND          0x0008 // Must be used with FTS_HASARROWS

typedef CList <CString*, CString*> CTabList;

// CCJFlatTabCtrl is a CWnd derived class which is used to create a flat tab
// control simular to DevStudio.
class _CJX_EXT_CLASS CCJFlatTabCtrl : public CWnd
{

// Construction
public:
	CCJFlatTabCtrl();

// Attributes
public:

// Operations
public:

	virtual BOOL InsertItem(const int nItem, LPCTSTR lpszItem);
	virtual BOOL DeleteItem(int nItem);
	virtual BOOL DeleteAllItems();
	virtual BOOL GetItemRect(int nItem, LPRECT lpRect);
	virtual int HitTest(TCHITTESTINFO *pHitTestInfo) const;
	virtual int GetCurSel() const {return m_iCurSel;}
	virtual int SetCurSel(int nItem);
	virtual int GetItemCount() const { return static_cast<int>( m_TabList.GetCount() ); }
    virtual void SetTipText(int nItem, LPCTSTR lpszTabTip);
    virtual CString GetTipText(int nItem);
    virtual void Home();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJFlatTabCtrl)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJFlatTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJFlatTabCtrl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLeftArrow();
	afx_msg void OnRightArrow();
	afx_msg void OnHomeArrow();
	afx_msg void OnEndArrow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	void CreateTabFont(const int iTabHeight);
	int GetTabWidth(const int nItem) const;
	int GetTotalArrowWidth(void) const;
	int GetTotalTabWidth(void) const;
	void InvalidateTabs(void);
	void EnableButtons(void);
	int DrawTab(CDC &dc, const int x, const int y, const int iHeight, bool bSelected, bool bBottom, LPCTSTR lpszTab) const;
	
	bool m_bHasArrows;
	bool m_bHasHomeEnd;
	bool m_bTabsOnBottom;
	int m_iCurSel;
	int m_iClientWidth;
	int m_iClientHeight;
	int m_iTotalTabWidth;
	int m_iXOffset;
	CTabList m_TabList;
	CTabList m_TabTipList;
	CFont m_Font;
	CFont m_BoldFont;
	CCJMetaFileButton *m_pLeftButton;
	CCJMetaFileButton *m_pRightButton;
	CCJMetaFileButton *m_pHomeButton;
	CCJMetaFileButton *m_pEndButton;
	HENHMETAFILE m_hLeftArrow;
	HENHMETAFILE m_hLeftArrowDisabled;
	HENHMETAFILE m_hRightArrow;
	HENHMETAFILE m_hRightArrowDisabled;
	HENHMETAFILE m_hHomeArrow;
	HENHMETAFILE m_hHomeArrowDisabled;
	HENHMETAFILE m_hEndArrow;
	HENHMETAFILE m_hEndArrowDisabled;

    CToolTipCtrl        m_ToolTip;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CCJFLATTABCTRL_H__
