// CJStatusBar.h : header file
//
// Copyright © 1999 Pierre MELLINAND
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/26/99 11:08p $
 * $Revision: 1 $
 * $Archive: /CodeJock/Include/CJStatusBar.h $
 *
 * $History: CJStatusBar.h $
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 11:08p
 * Created in $/CodeJock/Include
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJSTATUSBAR_H__
#define __CJSTATUSBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CCJStatusBarPaneControlInfo class

class _CJX_EXT_CLASS CCJStatusBarPaneControlInfo : public CObject
{
	DECLARE_DYNCREATE(CCJStatusBarPaneControlInfo)

// Construction
public:
	CCJStatusBarPaneControlInfo();
	virtual ~CCJStatusBarPaneControlInfo();

// Attributes
public:
	CWnd*	m_pWnd;
	int		m_nPaneID;
	BOOL	m_bAutodelete;
};

/////////////////////////////////////////////////////////////////////////////
// CCJStatusBarPane class

class _CJX_EXT_CLASS CCJStatusBarPane : public CObject
{
// Construction
public:
	CCJStatusBarPane();
	CCJStatusBarPane (CCJStatusBarPane& rPane);
	virtual ~CCJStatusBarPane();

// Attributes
public:
	UINT	m_nID;
	int		m_cxText;
	UINT	m_nStyle;
	UINT	m_nFlags;
	CString m_strText;

// Operations
public:
	CCJStatusBarPane& operator=(CCJStatusBarPane& rPane);
};

/////////////////////////////////////////////////////////////////////////////
// CCJStatusBar class

class _CJX_EXT_CLASS CCJStatusBar : public CStatusBar
{
	DECLARE_DYNCREATE(CCJStatusBar)

// Construction
public:
	CCJStatusBar();
	virtual ~CCJStatusBar();

// Attributes
private:
	CArray<CCJStatusBarPaneControlInfo*, CCJStatusBarPaneControlInfo*>	m_aPans;

// Operations
public:
	virtual void PositionControls();
	virtual CCJStatusBarPaneControlInfo* GetPanControl(int nPaneID);
	virtual BOOL AddControl(CWnd * pWnd, int nPaneID, BOOL bAutodeleteControl = TRUE );
	virtual void SetPaneWidth(int index, int cxWidth);
	virtual BOOL AddIndicator( int position, UINT paneID);
	virtual BOOL GetStatusPane(int nIndex, CCJStatusBarPane & xfxpane);
	virtual void RemovePane(int nPaneID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJStatusBar)
	//}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
protected:
	//{{AFX_MSG(CCJStatusBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJSTATUSBAR_H__

