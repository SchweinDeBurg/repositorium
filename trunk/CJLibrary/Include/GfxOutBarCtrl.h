// Copyright (c) Iuri Apollonio 1998
// Use & modify as you want & need, and leave those 3 lines.
// http://www.codeguru.com
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/26/99 10:50p $
 * $Revision: 6 $
 * $Archive: /CodeJock/Include/GfxOutBarCtrl.h $
 *
 * $History: GfxOutBarCtrl.h $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:50p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __GFXOUTBARCTRL_H__
#define __GFXOUTBARCTRL_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GfxOutBarCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGfxOutBarCtrl window

#include <afxcoll.h>
#include "CJMenu.h"        // CCJMenu class declaration

#define WM_OUTBAR_NOTIFY		WM_USER + 1
#define NM_OB_ITEMCLICK			1
#define NM_OB_ONLABELENDEDIT	2
#define NM_OB_ONGROUPENDEDIT	3
#define NM_OB_DRAGITEM			4
#define NM_FOLDERCHANGE			5
#define NM_OB_ITEMHOVER			6

struct OUTBAR_INFO 
{
	int		index;
	int		iDragFrom;
	int		iDragTo;

	LPCTSTR	cText;
};

class _CJX_EXT_CLASS CGfxOutBarCtrl : public CWnd
{
// Construction
	DECLARE_DYNCREATE(CGfxOutBarCtrl)
public:
	CGfxOutBarCtrl();

	enum {  fSmallIcon = 1 << 0, fLargeIcon = 1 << 1, fEditGroups = 1 << 2, fEditItems = 1 << 3,
			fRemoveGroups = 1 << 4, fRemoveItems = 1 << 5, fAddGroups = 1 << 6, 
			fDragItems = 1 << 7, fAnimation = 1 << 8, fSelHighlight = 1 << 9 };

	enum { ircIcon = 1, ircLabel = 2, ircAll = 3 };

// Attributes
public:
	COLORREF	crBackGroundColor, crBackGroundColor1;
	COLORREF	crTextColor;
	COLORREF	cr3dFace, crLightBorder, crHilightBorder, crShadowBorder, crDkShadowBorder;
	int			iFolderHeight;

	int			xSmallIconLabelOffset, yLargeIconLabelOffset;
	int			ySmallIconSpacing, yLargeIconSpacing;

	int			xLeftMargin, yTopMargin;
	bool		bUpArrow, bDownArrow, bUpPressed, bDownPressed;
	CRect		rcUpArrow, rcDownArrow;
	bool		bLooping;

	int			iHitInternal1, iHitInternal2;

	long		lAnimationTickCount;

	int			iLastSel, iSelAnimTiming;
	int			iSelAnimCount;


	DWORD		dwFlags;

	CPtrArray	arFolder;

	int			iLastFolderHighlighted;
	int			iLastSelectedFolder;
	int			iFirstItem;

	int			iLastItemHighlighted;
	bool		bPressedHighlight;

	int			iLastDragItemDraw, iLastDragItemDrawType;

	class CBarItem
	{
	public:
		CBarItem( LPCTSTR name, const int image, DWORD exData);
		virtual  ~CBarItem();
		int				iImageIndex;
		LPTSTR			cItem;
		DWORD			dwData;
	};

	class CBarFolder
	{
	public:
		CBarFolder( LPCTSTR name, DWORD exData);
		virtual  ~CBarFolder();
		int GetItemCount();
		int InsertItem( int index, LPCTSTR text, const int image, const DWORD exData);
		LPTSTR			cName;
		DWORD			dwData;
		CImageList *	pLargeImageList;
		CImageList *	pSmallImageList;
		CPtrArray		arItems;
		CWnd *			pChild;
	};

	int iSelFolder;

	CImageList *	pLargeImageList;
	CImageList *	pSmallImageList;

	HCURSOR			hHandCursor;
	HCURSOR			hDragCursor;
	HCURSOR			hNoDragCursor;

	CPen *			pBlackPen;
	
	CCJMenu			m_popupMenu;	// popup menu fix...07-29-1999 - KStowell
	CString			m_strRemoveItem;
	CString			m_strRenameItem;
	
// Operations
public:
	enum { htNothing = -1, htFolder, htItem, htUpScroll, htDownScroll};

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGfxOutBarCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void DrawAnimItem(const int xoffset, const int yoffset, const int index);
	virtual void SetAnimSelHighlight(const int iTime);
	virtual DWORD GetFolderData(int iFolder = -1);
	virtual CWnd * GetFolderChild(int iFolder = -1);
	virtual int AddFolderBar(LPCTSTR pFolder, CWnd * pSon, const DWORD exData = 0);
	virtual CString GetItemText(const int index);
	virtual void SetAnimationTickCount(const long value) { lAnimationTickCount = value; };
	virtual long GetAnimationTickCount() { return lAnimationTickCount; };

	virtual void AnimateFolderScroll(const int iFrom, const int iTo);
	virtual int GetDragItemRect(const int index, CRect &rect);
	virtual void DrawDragArrow(CDC * pDC, const int iFrom, const int iTo);
	virtual void SetItemImage(const int index, const int iImage);
	virtual void SetItemData(const int index, const DWORD dwData);
	virtual int  GetItemImage(const int index) const;
	virtual DWORD GetItemData(const int index) const;
	virtual bool IsValidItem(const int index) const;
	virtual void RemoveItem(const int index);
	virtual void SetItemText(const int index, LPCTSTR text);
	virtual void StartItemEdit(const int index);
	virtual void SetFolderText(const int index, LPCTSTR text);
	virtual void StartGroupEdit(const int index);
	virtual void GetLabelRect(const int iFolder, const int iIndex, CRect &rect);
	virtual void GetIconRect(const int iFolder, const int iIndex, CRect &rect);
	virtual void HighlightItem(const int index, const bool bPressed = false);
	virtual void GetVisibleRange(const int iFolder, int &first, int &last);
	virtual void DrawItem(CDC * pDC, const int iFolder, CRect rc, const int index, const bool bOnlyImage = false);
	virtual CImageList * GetFolderImageList(const int index, const bool bSmall) const;
	virtual CSize GetItemSize(const int iFolder, const int index, const int type);
	virtual void PaintItems(CDC * pDC, const int iFolder, CRect rc);
	virtual CImageList * GetImageList(CImageList * pImageList, int nImageList);
	virtual CImageList * SetFolderImageList(const int folder, CImageList * pImageList, int nImageList);
	virtual CImageList * SetImageList(CImageList * pImageList, int nImageList);
	virtual int GetCountPerPage() const;
	virtual void RemoveFolder(const int index);
	virtual int GetSelFolder() const;
	virtual int GetFolderCount() const;
	virtual void SetSelFolder(const int index);
	virtual int GetItemCount() const;
	virtual int InsertItem(const int folder, const int index, LPCTSTR text, const int image = -1, const DWORD exData = 0);
	virtual void HighlightFolder(const int index);
	virtual int HitTestEx(const CPoint &point, int &index);
	virtual void GetInsideRect(CRect &rect) const;
	virtual int AddFolder(LPCTSTR cFolderName, const DWORD exData);
	virtual void GetItemRect(const int iFolder, const int iIndex, CRect &rect);
	virtual bool GetFolderRect(const int iIndex, CRect &rect) const;
	virtual void ModifyFlag(const DWORD &dwRemove, const DWORD &dwAdd, const UINT redraw = 0);
	virtual DWORD GetFlag() const;
	virtual void SetSmallIconView(const bool bSet);
	virtual bool IsSmallIconView() const;
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd * pParentWnd, UINT nID, const DWORD dwFlag = fDragItems|fEditGroups|fEditItems|fRemoveGroups|fRemoveItems|fAddGroups|fAnimation|fSelHighlight);
	virtual ~CGfxOutBarCtrl();

	// Generated message map functions
protected:
	void DrawFolder(CDC * pDC, const int iIdx, CRect rect, const bool bSelected);
	//{{AFX_MSG(CGfxOutBarCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGfxLargeicon();
	afx_msg void OnUpdateGfxLargeicon(CCmdUI* pCmdUI);
	afx_msg void OnGfxSmallicon();
	afx_msg void OnUpdateGfxSmallicon(CCmdUI* pCmdUI);
	afx_msg void OnGfxRemoveitem();
	afx_msg void OnUpdateGfxRemoveitem(CCmdUI* pCmdUI);
	afx_msg void OnGfxRenameitem();
	afx_msg void OnUpdateGfxRenameitem(CCmdUI* pCmdUI);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	afx_msg long OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __GFXOUTBARCTRL_H__
