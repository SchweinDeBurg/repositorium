// CJMetaFileButton.h : header file
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
//			1.20	21 Apr 1999	- Initial release.
// ==========================================================================  
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:04p $
 * $Revision: 6 $
 * $Archive: /CodeJock/Include/CJMetaFileButton.h $
 *
 * $History: CJMetaFileButton.h $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:04p
 * Updated in $/CodeJock/Include
 * Overrode OnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:45p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJMETAFILEBUTTON_H__
#define __CJMETAFILEBUTTON_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CCJMetaFileButton is a CButton derived class which is used by
// the CCJFlatTabCtrl class.
class _CJX_EXT_CLASS CCJMetaFileButton : public CButton
{
// Construction
public:
	CCJMetaFileButton();

// Attributes
public:

// Operations
public:
	virtual void SetMetaFiles(HENHMETAFILE hMetaFile,
		HENHMETAFILE hMetaFileSel = NULL,
		HENHMETAFILE hMetaFileFocus = NULL,
		HENHMETAFILE hMetaFileDisabled = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJMetaFileButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJMetaFileButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJMetaFileButton)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	HENHMETAFILE m_hEnhMetaFile;
	HENHMETAFILE m_hEnhMetaFileSel;
	HENHMETAFILE m_hEnhMetaFileFocus;
	HENHMETAFILE m_hEnhMetaFileDisabled;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __CJMETAFILEBUTTON_H__
