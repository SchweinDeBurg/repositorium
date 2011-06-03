// Copyright (C) 2003-2011 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// EnToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "EnToolBar.h"
#include "EnBitmapEx.h"
#include "ImageProcessors.h"
#include "OSVersion.h"
#include "GraphicsMisc.h"

#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT WM_REFRESHBUTTONSTATES = WM_APP + 1;

/////////////////////////////////////////////////////////////////////////////
// CEnToolBar

CEnToolBar::CEnToolBar() : m_crFrom(NO_COLOR), m_crTo(NO_COLOR)
{
}

CEnToolBar::~CEnToolBar()
{
	m_ilNormal.DeleteImageList();
	m_ilDisabled.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CEnToolBar, CToolBar)
	//{{AFX_MSG_MAP(CEnToolBar)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_REFRESHBUTTONSTATES, OnRefreshButtonStates)
	ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnToolBar message handlers

BOOL CEnToolBar::LoadToolBar(LPCTSTR lpszResourceName, LPCTSTR szImagePath)
{
	if (CToolBar::LoadToolBar(lpszResourceName))
	{
		if (szImagePath && ::GetFileAttributes(szImagePath) != 0xffffffff)
		{
			SetImage(szImagePath);
		}
		else
		{
			GetToolBarCtrl().SetDisabledImageList(NULL);
			m_ilDisabled.DeleteImageList();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CEnToolBar::LoadToolBar(UINT nIDResource, LPCTSTR szImagePath)
{
	return LoadToolBar(MAKEINTRESOURCE(nIDResource), szImagePath);
}

BOOL CEnToolBar::LoadToolBar(UINT nIDResource, UINT nIDImage)
{
	if (CToolBar::LoadToolBar(MAKEINTRESOURCE(nIDResource)))
	{
		if (nIDImage)
		{
			SetImage(nIDImage);
		}
		else
		{
			GetToolBarCtrl().SetDisabledImageList(NULL);
			m_ilDisabled.DeleteImageList();
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CEnToolBar::SetImage(UINT nIDImage, COLORREF crMask)
{
	CEnBitmapEx bitmap;

	if (!bitmap.LoadBitmap(nIDImage))
	{
		return FALSE;
	}

	return SetImage(&bitmap, crMask);
}

BOOL CEnToolBar::SetImage(const CString& sImagePath, COLORREF crMask)
{
	CEnBitmapEx bitmap;

	if (!bitmap.LoadImage(sImagePath))
	{
		return FALSE;
	}

	return SetImage(&bitmap, crMask);
}

BOOL CEnToolBar::SetImage(CEnBitmapEx* pBitmap, COLORREF crMask)
{
	CEnBitmapEx bmDis;
	bmDis.CopyImage(pBitmap); // for later

	if (crMask == NO_COLOR) // map 3d colors
	{
		pBitmap->RemapSysColors();
	}

	// button size
	BITMAP BM;
	pBitmap->GetBitmap(&BM);

	int nCount = GetButtonCount(TRUE);
	ASSERT(nCount);

	if (!nCount)
	{
		return FALSE;
	}

	CSize sizeBmp((BM.bmWidth / nCount), BM.bmHeight);
	CSize sizeBtn(sizeBmp.cx + 7, sizeBmp.cy + 7);

	SetSizes(sizeBtn, sizeBmp);

	m_ilNormal.DeleteImageList();

	if (m_ilNormal.Create(sizeBmp.cx, sizeBmp.cy, ILC_COLOR32 | ILC_MASK, 0, 1))
	{
		m_ilNormal.Add(pBitmap, crMask);

		CImageList* pILPrev = GetToolBarCtrl().SetImageList(&m_ilNormal);

		if (pILPrev)
		{
			pILPrev->DeleteImageList();   // cleanup
		}

		pILPrev = GetToolBarCtrl().SetHotImageList(&m_ilNormal);

		if (pILPrev)
		{
			pILPrev->DeleteImageList();   // cleanup
		}

		RefreshDisabledImageList(&bmDis, crMask);
		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CEnToolBar::GrayScale(CEnBitmapEx* pBitmap, COLORREF crMask)
{
	// create 'nice' disabled imagelist
	C32BIPArray aProcessors;
	CImageGrayer ip2(0.33, 0.33, 0.33);
	CImageLightener ip3(0.2);
	CImageTinter ip4(GetSysColor(COLOR_3DSHADOW), 10);
	CImageContraster ip5(-30);

	aProcessors.Add(&ip2);
	aProcessors.Add(&ip3);
	aProcessors.Add(&ip4);
	aProcessors.Add(&ip5);

	return pBitmap->ProcessImage(aProcessors, crMask);
}

int CEnToolBar::GetButtonCount(BOOL bIgnoreSeparators) const
{
	int nCount = GetToolBarCtrl().GetButtonCount();

	if (bIgnoreSeparators)
	{
		int nBtn = nCount;

		while (nBtn--)
		{
			if (GetItemID(nBtn) == ID_SEPARATOR)
			{
				nCount--;
			}
		}
	}

	return nCount;
}

void CEnToolBar::SetBackgroundColors(COLORREF crFrom, COLORREF crTo, BOOL bGradient)
{
	m_crFrom = crFrom;

	if (crTo == (COLORREF) - 1)
	{
		m_crTo = m_crFrom;
	}
	else
	{
		m_crTo = crTo;
	}

	m_bGradient = bGradient;

	if (GetSafeHwnd())
	{
		Invalidate();
	}
}

void CEnToolBar::RefreshButtonStates(BOOL bImmediate)
{
	if (bImmediate)
	{
		OnUpdateCmdUI((CFrameWnd*)GetParent(), FALSE);
	}
	else
	{
		PostMessage(WM_REFRESHBUTTONSTATES);
	}
}

LRESULT CEnToolBar::OnRefreshButtonStates(WPARAM /*wp*/, LPARAM /*lp*/)
{
	RefreshButtonStates(TRUE);
	return 0L;
}

void CEnToolBar::RefreshDisabledImageList(CEnBitmapEx* pBitmap, COLORREF crMask)
{
	// not under win9x
	if (COSVersion() >= OSV_NT4)
	{
		// create 'nice' disabled imagelist

		if (GrayScale(pBitmap, crMask))
		{
			if (crMask == NO_COLOR) // map 3d colors
			{
				pBitmap->RemapSysColors();
			}

			// button size
			int nCx = m_sizeImage.cx, nCy = m_sizeImage.cy;

			m_ilDisabled.DeleteImageList();
			m_ilDisabled.Create(nCx, nCy, ILC_COLOR24 | ILC_MASK, 0, 1);
			m_ilDisabled.Add(pBitmap, crMask);

			CImageList* pILPrev = GetToolBarCtrl().SetDisabledImageList(&m_ilDisabled);

			if (pILPrev)
			{
				pILPrev->DeleteImageList();   // cleanup
			}

			Invalidate();
		}
	}
}
//fabio_2005
void CEnToolBar::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = CDRF_DODEFAULT;
	LPNMTBCUSTOMDRAW lpNMCustomDraw = (LPNMTBCUSTOMDRAW)pNMHDR;

	switch (lpNMCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		OnEraseBkgnd(CDC::FromHandle(lpNMCustomDraw->nmcd.hdc));

		*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		break;

	case CDDS_POSTPAINT:
		*pResult = CDRF_DODEFAULT | CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_PREERASE:
		*pResult = CDRF_SKIPDEFAULT | CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		break;

	case CDDS_ITEMPREPAINT:
		*pResult = OnItemPrePaint(lpNMCustomDraw) | CDRF_NOTIFYPOSTPAINT;
		break;

	case CDDS_ITEMPOSTPAINT:
		*pResult = OnItemPostPaint(lpNMCustomDraw);
		break;

	default:
		break;
	}
}

BOOL CEnToolBar::OnEraseBkgnd(CDC* pDC)
{
	CRect rClient;
	GetClientRect(rClient);

	if (HasBkgndColor())
	{
		if (m_crTo == m_crFrom)
		{
			pDC->FillSolidRect(rClient, m_crFrom);
		}
		else
		{
			int nRows = GetRowCount();
			int nRowHeight = rClient.Height() / nRows;

			for (int nRow = 0; nRow < nRows; nRow++)
			{
				CRect rRow(rClient);
				rRow.top = nRow * nRowHeight;
				rRow.bottom = rRow.top + nRowHeight;

				if (m_bGradient)
				{
					GraphicsMisc::DrawGradient(pDC->GetSafeHdc(), rRow, m_crFrom, m_crTo, FALSE);
				}
				else
				{
					rRow.bottom = rRow.top + (nRowHeight * 2) / 5; // based on Outlook 2007
					pDC->FillSolidRect(rRow, m_crFrom);

					rRow.top = rRow.bottom;
					rRow.bottom = nRow * nRowHeight + nRowHeight;
					pDC->FillSolidRect(rRow, m_crTo);
				}
			}
		}
	}
	else
	{
		pDC->FillSolidRect(rClient, GetSysColor(COLOR_3DFACE));
	}

	return TRUE;
}

void CEnToolBar::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	// toolbar button sizes get screwed if any button has the drop style applied
	// at the time this is handled. to get round it we remove any such button styles
	// and then readd them after the default processing
	CUIntArray aDropBtns;
	int nBtn = GetToolBarCtrl().GetButtonCount();

	while (nBtn--)
	{
		UINT nBtnID = GetItemID(nBtn);
		DWORD dwStyle = GetButtonStyle(nBtn);

		if (dwStyle & TBSTYLE_DROPDOWN)
		{
			SetButtonStyle(nBtn, dwStyle & ~TBSTYLE_DROPDOWN);
			aDropBtns.Add(nBtnID);
		}
	}

	// default processing
	CToolBar::OnSettingChange(uFlags, lpszSection);

	// restore styles
	int nItem = aDropBtns.GetSize();

	while (nItem--)
	{
		int nBtn = CommandToIndex(aDropBtns[nItem]);
		DWORD dwStyle = GetButtonStyle(nBtn);

		SetButtonStyle(nBtn, dwStyle | TBSTYLE_DROPDOWN);
	}
}

int CEnToolBar::CalcHeightRequired(int cx) const
{
	int nRows = CalcRowsRequired(cx);
	int nRowHeight = (nRows == 1) ? (m_sizeButton.cy + 2) :
		(nRows < 4) ? (m_sizeButton.cy + 4) : (m_sizeButton.cy + 6);

	return nRows * nRowHeight;
}

int CEnToolBar::CalcRowsRequired(int cx) const
{
	int nRows = 1;
	int nXPos = 0;

	for (int nBtn = 0; nBtn < GetButtonCount(); nBtn++)
	{
		CRect rBtn;
		GetItemRect(nBtn, rBtn);

		if (nXPos) // a button has been added
		{
			if (nXPos + rBtn.Width() > cx)
			{
				nRows++;
				nXPos = 0;
			}
		}

		// else
		nXPos += rBtn.Width();
	}

	return nRows;
}

int CEnToolBar::Resize(int cx, CPoint ptTopLeft)
{
	int nHeight = CalcHeightRequired(cx);
	CRect rToolbar(ptTopLeft, CSize(cx, nHeight));

	MoveWindow(rToolbar);
	return nHeight;
}

int CEnToolBar::GetRowCount() const
{
	CRect rClient;
	GetClientRect(rClient);

	return CalcRowsRequired(rClient.Width());
}

int CEnToolBar::GetRowHeight() const
{
	CRect rClient;
	GetClientRect(rClient);

	return rClient.Height() / GetRowCount();
}
