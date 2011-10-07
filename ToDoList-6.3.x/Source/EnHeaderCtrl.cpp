//_ **********************************************************
//_
//_ Name: EnHeaderCtrl.cpp
//_ Purpose:
//_ Created: 15 September 1998
//_ Author: D.R.Godson
//_ Modified By:
//_
//_ Copyright (c) 1998 Brilliant Digital Entertainment Inc.
//_
//_ **********************************************************

// EnHeaderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "EnHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnHeaderCtrl

CEnHeaderCtrl::CEnHeaderCtrl()
{
	m_bAllowTracking = TRUE;
}

CEnHeaderCtrl::~CEnHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CEnHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CEnHeaderCtrl)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(HDN_BEGINTRACK, OnBeginTrackHeader)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnHeaderCtrl message handlers

void CEnHeaderCtrl::OnBeginTrackHeader(NMHDR* /*pNMHDR*/, LRESULT* /*pResult*/)
{
	// this no longer seems to work with vc++5 because the message never seems to get
	// here. instead we do some trickery in the OnLButtonDown handler
	// *pResult = !m_bAllowTracking;
}

BOOL CEnHeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bAllowTracking)
	{
		return CHeaderCtrl::OnSetCursor(pWnd, nHitTest, message);
	}
	else
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return FALSE;
	}
}

int CEnHeaderCtrl::AddColumn(int nCol, int nWidth, CString sTitle, int nFormat, UINT uIDBitmap)
{
	HD_ITEM hdi;
	CBitmap bm;

	ASSERT(nCol >= 0 && nWidth > 0);

	// mask
	hdi.mask = HDI_WIDTH | HDI_FORMAT;
	hdi.mask |= uIDBitmap ? HDI_BITMAP : 0;
	hdi.mask |= !sTitle.IsEmpty() ? HDI_TEXT : 0;

	// format
	hdi.fmt = nFormat;

	// bitmap
	if (uIDBitmap)
	{
		bm.LoadBitmap(uIDBitmap);
		hdi.hbm = (HBITMAP)bm.GetSafeHandle();
		bm.Detach();
	}

	// width
	hdi.cxy = nWidth;

	// text
	if (!sTitle.IsEmpty())
	{
		hdi.pszText = (LPTSTR)(LPCTSTR)sTitle;
		hdi.cchTextMax = sTitle.GetLength();
	}

	return InsertItem(nCol, &hdi);
}

BOOL CEnHeaderCtrl::SetColumnWidth(int nCol, int nWidth)
{
	HD_ITEM hdi;

	ASSERT(nCol >= 0 && nWidth > 0);

	// mask
	hdi.mask = HDI_WIDTH;

	// width
	hdi.cxy = nWidth;

	return SetItem(nCol, &hdi);
}

int CEnHeaderCtrl::GetColumnWidth(int nCol)
{
	HD_ITEM hdi;

	ASSERT(nCol >= 0 && nCol < GetItemCount());

	// mask
	hdi.mask = HDI_WIDTH;
	GetItem(nCol, &hdi);

	return hdi.cxy;
}

void CEnHeaderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// eat the message if not tracking enabled
	if (m_bAllowTracking)
	{
		CHeaderCtrl::OnLButtonDblClk(nFlags, point);
	}
}

void CEnHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// see OnBeginTrackHeader for why the following trickery is required
	if (!m_bAllowTracking)
	{
		// check to see whether the mouse is over the divider and if it is
		// shift the y coord off the header
		int nCol = 0;
		int nDivider = 0;

		while (nCol < GetItemCount())
		{
			nDivider += GetColumnWidth(nCol);

			if ((point.x >= nDivider - 8) && (point.x <= nDivider + 8))
			{
				point.y = -50;
				break;
			}

			// next col
			nCol++;
		}
		// do default processing
		CHeaderCtrl::DefWindowProc(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
	}
	else
	{
		CHeaderCtrl::OnLButtonDown(nFlags, point);
	}
}

void CEnHeaderCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// do nothing
}
