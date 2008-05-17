// Copyright (c) Iuri Apollonio 1998
// Use & modify as you want & need, and leave those 3 lines.
// http://www.codeguru.com
//
// GfxSplitterWnd.h: interface for the CGfxSplitterWnd class.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/26/99 10:50p $
 * $Revision: 4 $
 * $Archive: /CodeJock/Include/GfxSplitterWnd.h $
 *
 * $History: GfxSplitterWnd.h $
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:50p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __GFXSPLITTERWND_H__
#define __GFXSPLITTERWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class _CJX_EXT_CLASS CGfxSplitterWnd : public CSplitterWnd  
{
public:
	int m_upBorder;
	bool bWhiteLine;
	CGfxSplitterWnd();
	virtual ~CGfxSplitterWnd();
	virtual void OnDrawSplitter(CDC* pDC, ESplitType nType, const CRect& rectArg);
	virtual void OnInvertTracker(const CRect & rect);

	virtual void RecalcLayout();
protected:
	virtual void GetInsideRect(CRect& rect) const;
	// Generated message map functions
	//{{AFX_MSG(CGfxSplitterWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __GFXSPLITTERWND_H__
