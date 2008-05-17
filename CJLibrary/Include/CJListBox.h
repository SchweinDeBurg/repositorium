// CJListBox.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 9:36p $
 * $Revision: 1 $
 * $Archive: /CodeJock/Include/CJListBox.h $
 *
 * $History: CJListBox.h $
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 9:36p
 * Created in $/CodeJock/Include
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJLISTBOX_H__
#define __CJLISTBOX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CCJListBox is a CListBox derived class which creates a
// flicker free listbox control.
class _CJX_EXT_CLASS CCJListBox : public CListBox
{
	DECLARE_DYNAMIC(CCJListBox)

// Construction
public:
	CCJListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJListBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJListBox();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJListBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJLISTBOX_H__

