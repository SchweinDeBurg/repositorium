// Copyright (C) 2003-2005 AbstractSpoon Software.
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
// - added AbstractSpoon Software copyright notice and licenese information
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// CDialogHelper.cpp: implementation of the CDialogHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DialogHelper.h"
#include "AutoFlag.h"
#include "WinClasses.h"
#include "WClassDefines.h"
#include "Misc.h"
#include "RuntimeDlg.h"

#include <afxpriv.h>
#include <float.h>
#include <locale.h>
#include <afxtempl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// MFC replacements to prevent popup error messages

BOOL DH_SimpleScanf(LPCTSTR lpszText, LPCTSTR lpszFormat, va_list pData)
{
	ASSERT(lpszText != NULL);
	ASSERT(lpszFormat != NULL);

	ASSERT(*lpszFormat == _T('%'));
	lpszFormat++;        // skip '%'

	BOOL bLong = FALSE;
	BOOL bShort = FALSE;
	if (*lpszFormat == _T('l'))
	{
		bLong = TRUE;
		lpszFormat++;
	}
	else if (*lpszFormat == _T('s'))
	{
		bShort = TRUE;
		lpszFormat++;
	}

	ASSERT(*lpszFormat == _T('d') || *lpszFormat == _T('u'));
	ASSERT(lpszFormat[1] == _T('\0'));

	while (*lpszText == _T(' ') || *lpszText == _T('\t'))
	{
		lpszText++;
	}
	TCHAR chFirst = lpszText[0];
	long l, l2;
	if (*lpszFormat == _T('d'))
	{
		// signed
		l = _tcstol(lpszText, (LPTSTR*)&lpszText, 10);
		l2 = (int)l;
	}
	else
	{
		// unsigned
		if (*lpszText == _T('-'))
		{
			return FALSE;
		}
		l = (long)_tcstoul(lpszText, (LPTSTR*)&lpszText, 10);
		l2 = (unsigned int)l;
	}
	if (l == 0 && chFirst != _T('0'))
	{
		return FALSE;   // could not convert
	}

	while (*lpszText == _T(' ') || *lpszText == _T('\t'))
	{
		lpszText++;
	}
	if (*lpszText != _T('\0'))
	{
		return FALSE;   // not terminated properly
	}

	if (bShort)
	{
		if ((short)l != l)
		{
			return FALSE;   // too big for short
		}
		*va_arg(pData, short*) = (short)l;
	}
	else
	{
		ASSERT(sizeof(long) == sizeof(int));
		ASSERT(l == l2);
		*va_arg(pData, long*) = l;
	}

	// all ok
	return TRUE;
}

BOOL DH_SimpleFloatParse(LPCTSTR lpszText, double& d)
{
	ASSERT(lpszText != NULL);
	while (*lpszText == _T(' ') || *lpszText == _T('\t'))
	{
		lpszText++;
	}

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != _T('0'))
	{
		return FALSE;   // could not convert
	}
	while (*lpszText == _T(' ') || *lpszText == _T('\t'))
	{
		lpszText++;
	}

	if (*lpszText != _T('\0'))
	{
		return FALSE;   // not terminated properly
	}

	return TRUE;
}

void DH_DDX_TextWithFormat(CDataExchange* pDX, int nIDC, LPCTSTR lpszFormat, UINT nIDPrompt, ...)
{
	va_list pData;
	va_start(pData, nIDPrompt);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szT[32];
	if (pDX->m_bSaveAndValidate)
	{
		// the following works for %d, %u, %ld, %lu
		::GetWindowText(hWndCtrl, szT, sizeof(szT) / sizeof(TCHAR));

		if (*szT == 0)
		{
			//fabio_2005
#if _MSC_VER >= 1400
			_tcscpy_s(szT, _T("0"));
#else
			_tcscpy(szT, _T("0"));
#endif
		}

		if (!DH_SimpleScanf(szT, lpszFormat, pData))
		{
		}
	}
	else
	{
		wvsprintf(szT, lpszFormat, pData);
		// does not support floating point numbers - see dlgfloat.cpp
		SetWindowText(hWndCtrl, szT);
	}

	va_end(pData);
}

void DH_TextFloatFormat(CDataExchange* pDX, int nIDC, void* pData, double value, int nSizeGcvt)
{
	// handle locale specific decimal separator
	_tsetlocale(LC_NUMERIC, _T(""));

	ASSERT(pData != NULL);

	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	TCHAR szBuffer[32];

	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hWndCtrl, szBuffer, sizeof(szBuffer) / sizeof(TCHAR));

		if (*szBuffer == 0)
		{
			//fabio_2005
#if _MSC_VER >= 1400
			_tcscpy_s(szBuffer, _T("0"));
#else
			_tcscpy(szBuffer, _T("0"));
#endif
		}

		double d;

		if (!DH_SimpleFloatParse(szBuffer, d))
		{
		}
		if (nSizeGcvt == FLT_DIG)
		{
			*((float*)pData) = (float)d;
		}
		else
		{
			*((double*)pData) = d;
		}
	}
	else
	{
		//fabio_2005
#if _MSC_VER >= 1400
		_stprintf_s(szBuffer, _T("%.*g"), nSizeGcvt, value);
#else
		_stprintf(szBuffer, _T("%.*g"), nSizeGcvt, value);
#endif
		AfxSetWindowText(hWndCtrl, szBuffer);
	}

	// restore decimal separator to '.'
	_tsetlocale(LC_NUMERIC, _T("English"));
}

//////////////////////////////////////////////////////////////////////

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, BYTE& value)
{
	int n = (int)value;
	if (pDX->m_bSaveAndValidate)
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_BYTE, &n);

		if (n > 255)
		{
		}
		value = (BYTE)n;
	}
	else
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_BYTE, n);
	}
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, short& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%sd"), AFX_IDP_PARSE_INT, &value);
	}
	else
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%hd"), AFX_IDP_PARSE_INT, value);
	}
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, int& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, &value);
	}
	else
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%d"), AFX_IDP_PARSE_INT, value);
	}
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, UINT& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_UINT, &value);
	}
	else
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%u"), AFX_IDP_PARSE_UINT, value);
	}
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, long& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%ld"), AFX_IDP_PARSE_INT, &value);
	}
	else
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%ld"), AFX_IDP_PARSE_INT, value);
	}
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, DWORD& value)
{
	if (pDX->m_bSaveAndValidate)
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%lu"), AFX_IDP_PARSE_UINT, &value);
	}
	else
	{
		DH_DDX_TextWithFormat(pDX, nIDC, _T("%lu"), AFX_IDP_PARSE_UINT, value);
	}
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, CString& value)
{
	::DDX_Text(pDX, nIDC, value);
}

////////////////////////////////////////////////////////////////////////////////
// floats

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, float& value)
{
	DH_TextFloatFormat(pDX, nIDC, &value, value, FLT_DIG);
}

void CDialogHelper::DDX_Text(CDataExchange* pDX, int nIDC, double& value)
{
	DH_TextFloatFormat(pDX, nIDC, &value, value, DBL_DIG);
}

////////////////////////////////////////////////////////////////////////////////

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, BYTE& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, short& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, int& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	// this can be used for a variety of control types so we need
	// to figure out what nIDC points to and then code accordingly
	HWND hCtrl = dx.PrepareCtrl(nIDC);

	if (!hCtrl)
	{
		return FALSE;
	}

	CString sClass;

	::GetClassName(hCtrl, sClass.GetBuffer(100), 100);
	sClass.ReleaseBuffer();

	if (CWinClasses::IsEditControl(hCtrl))
	{
		DDX_Text(&dx, nIDC, value);
	}

	else if (CWinClasses::IsClass(hCtrl, WC_COMBOBOX))
	{
		DDX_CBIndex(&dx, nIDC, value);
	}

	else if (CWinClasses::IsClass(hCtrl, WC_LISTBOX))
	{
		DDX_LBIndex(&dx, nIDC, value);
	}

	else if (CWinClasses::IsClass(hCtrl, WC_SCROLLBAR))
	{
		DDX_Scroll(&dx, nIDC, value);
	}

	else if (CWinClasses::IsClass(hCtrl, WC_BUTTON))
	{
		DWORD dwStyle = GetWindowLong(hCtrl, GWL_STYLE);

		if ((dwStyle & BS_CHECKBOX) == BS_CHECKBOX)
		{
			DDX_Check(&dx, nIDC, value);
		}
		else if ((dwStyle & BS_RADIOBUTTON) == BS_RADIOBUTTON)
		{
			DDX_Radio(&dx, nIDC, value);
		}
		else
		{
			ASSERT(0);
			return FALSE;
		}
	}
	else
	{
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, UINT& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, long& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, DWORD& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, CString& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	// this can be used for a variety of control types so we need
	// to figure out what nIDC points to and then code accordingly
	HWND hCtrl = dx.PrepareCtrl(nIDC);

	if (!hCtrl)
	{
		return FALSE;
	}

	if (CWinClasses::IsEditControl(hCtrl))
	{
		DDX_Text(&dx, nIDC, value);
	}

	else if (CWinClasses::IsClass(hCtrl, WC_COMBOBOX))
	{
		DDX_CBString(&dx, nIDC, value);
	}

	else if (CWinClasses::IsClass(hCtrl, WC_LISTBOX))
	{
		DDX_LBString(&dx, nIDC, value);
	}

	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CDialogHelper::UpdateDataExact(CWnd* pWnd, int nIDC, CString& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	// this can be used for a variety of control types so we need
	// to figure out what nIDC points to and then code accordingly
	HWND hCtrl = dx.PrepareCtrl(nIDC);

	if (!hCtrl)
	{
		return FALSE;
	}

	else if (CWinClasses::IsClass(hCtrl, WC_COMBOBOX))
	{
		DDX_CBStringExact(&dx, nIDC, value);
	}

	else if (CWinClasses::IsClass(hCtrl, WC_LISTBOX))
	{
		DDX_LBStringExact(&dx, nIDC, value);
	}

	else
	{
		ASSERT(0);
		return FALSE;
	}

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, float& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, double& value, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Text(&dx, nIDC, value);

	return TRUE;
}

BOOL CDialogHelper::UpdateDataEx(CWnd* pWnd, int nIDC, CWnd& ctrl, BOOL bSaveAndValidate)
{
	CAutoFlag af(m_bInUpdateEx, TRUE);
	CDataExchange dx(pWnd, bSaveAndValidate);

	DDX_Control(&dx, nIDC, ctrl);

	return TRUE;
}

void CDialogHelper::SetFont(CWnd* pWnd, HFONT hFont, BOOL bRedraw)
{
	// don't send to toolbar as it causes all sorts of problems with drop buttons
	// but do send to a toolbar's children
	if (!pWnd->IsKindOf(RUNTIME_CLASS(CToolBar)))
	{
		pWnd->SendMessage(WM_SETFONT, (WPARAM)hFont, bRedraw);
	}

	// children
	CWnd* pChild = pWnd->GetWindow(GW_CHILD);

	while (pChild)
	{
		SetFont(pChild, hFont, bRedraw);
		pChild = pChild->GetNextWindow(GW_HWNDNEXT);
	}
}

HFONT CDialogHelper::GetFont(CWnd* pWnd)
{
	if (pWnd)
	{
		return GetFont(pWnd->GetSafeHwnd());
	}

	return (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
}

HFONT CDialogHelper::GetFont(HWND hWnd)
{
	if (hWnd)
	{
		HFONT hFont = (HFONT)::SendMessage(hWnd, WM_GETFONT, 0, 0);

		if (hFont)
		{
			return hFont;
		}
	}

	return (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
}

int CDialogHelper::SetComboBoxItems(CComboBox& combo, const CStringArray& aItems)
{
	combo.ResetContent();

	for (int nItem = 0; nItem < aItems.GetSize(); nItem++)
	{
		combo.AddString(aItems[nItem]);
	}

	return combo.GetCount();
}

BOOL CDialogHelper::IsDroppedComboBox(HWND hCtrl)
{
	if (IsComboBox(hCtrl))
	{
		return ::SendMessage(hCtrl, CB_GETDROPPEDSTATE, 0, 0);
	}

	return FALSE;
}

BOOL CDialogHelper::ProcessDialogControlShortcut(const MSG* pMsg)
{
	// message id must be WM_KEYDOWN and alt key must be pressed
	if (pMsg->message != WM_SYSKEYDOWN || pMsg->wParam == VK_MENU)
	{
		return FALSE;
	}

	if (!Misc::ModKeysArePressed(MKS_ALT))
	{
		return FALSE;
	}

	// convert shortcut from virtual key to char
	UINT nShortcut = MapVirtualKey(pMsg->wParam, 2);

	if (!nShortcut)
	{
		return FALSE;
	}

	// find the next control having this accelerator
	CWnd* pFocus = CWnd::GetFocus();

	if (pFocus)
	{
		CWnd* pNext = FindNextMatch(pFocus, nShortcut);

		if (pNext)
		{
			pNext->SetFocus();
			return TRUE;
		}
	}

	return FALSE;
}

void CDialogHelper::ClipChild(CDC* pDC, CWnd* pWnd, UINT nChildID)
{
	CWnd* pChild = pWnd->GetDlgItem(nChildID);

	if (pChild)
	{
		CRect rChild;

		pChild->GetWindowRect(rChild);
		pWnd->ScreenToClient(rChild);
		pDC->ExcludeClipRect(rChild);
	}
}

void CDialogHelper::ClipChild(CDC* pDC, CWnd* pChild)
{
	CWnd* pParent = pChild->GetParent();

	if (pParent)
	{
		CRect rChild;

		pChild->GetWindowRect(rChild);
		pParent->ScreenToClient(rChild);
		pDC->ExcludeClipRect(rChild);
	}
}

CWnd* CDialogHelper::FindNextMatch(CWnd* pCurrent, UINT nShortcut)
{
	// only the brute force method would appear to work here
	nShortcut = toupper(nShortcut);

	// if pCurrent is the edit of a combo then we want the combo
	if (CWinClasses::IsEditControl(*pCurrent) &&
		CWinClasses::IsClass(::GetParent(*pCurrent), WC_COMBOBOX))
	{
		pCurrent = pCurrent->GetParent();
	}

	// 1. find the first parent popup window
	CWnd* pTop = pCurrent->GetParent();

	while (pTop && !(pTop->GetStyle() & WS_CAPTION))
	{
		pTop = pTop->GetParent();
	}

	if (pTop && pTop != pCurrent)
	{
		// get a list of all tabbable items within this parent
		CTypedPtrList<CPtrList, CWnd*> lstWnds;

		CWnd* pFirst = pTop->GetNextDlgTabItem(pTop->GetWindow(GW_CHILD));
		CWnd* pChild = pFirst;

		while (pChild)
		{
			lstWnds.AddTail(pChild);
			pChild = pTop->GetNextDlgTabItem(pChild);

			if (pChild == pFirst)
			{
				break;
			}
		}

		if (lstWnds.GetCount() > 1)
		{
			// keep moving the head to the tail until pCurrent is at the head
			// unless pCurrent is not in the list (been disabled or made invisible)
			if (lstWnds.Find(pCurrent))
			{
				CWnd* pHead = lstWnds.GetHead();

				while (pHead != pCurrent && !pHead->IsChild(pCurrent))
				{
					lstWnds.AddTail(lstWnds.RemoveHead());
					pHead = lstWnds.GetHead();
				}

				// remove pCurrent
				lstWnds.RemoveHead();
			}
			else
			{
				TRACE(_T("CDialogHelper::FindNextMatch(pCurrent not found)\n"));
			}

			// now traverse the list looking for preceding static
			// labels with a matching accelerator
			POSITION pos = lstWnds.GetHeadPosition();

			while (pos)
			{
				pChild = lstWnds.GetNext(pos);

				CWnd* pPrev = pChild->GetWindow(GW_HWNDPREV);

				if (CWinClasses::IsClass(pPrev->GetSafeHwnd(), WC_STATIC) &&
					pPrev->IsWindowVisible())
				{
					CString sText;
					pPrev->GetWindowText(sText);

					if (!sText.IsEmpty() && GetShortcut(sText) == nShortcut)
					{
						return pChild;
					}
				}
			}
		}
	}

	// all else
	return NULL;
}

UINT CDialogHelper::GetShortcut(const CString& sText)
{
	for (int nChar = 0; nChar < sText.GetLength() - 1; nChar++)
	{
		if (sText[nChar] == _T('&') && sText[nChar + 1] != _T('&'))
		{
			return toupper(sText[nChar + 1]);
		}
	}

	// no shortcut
	return 0;
}

UINT CDialogHelper::MessageBoxEx(CWnd* pWnd, UINT nIDText, UINT nIDCaption, UINT nType)
{
	CString sText;

	sText.LoadString(nIDText);

	return MessageBoxEx(pWnd, sText, nIDCaption, nType);
}

UINT CDialogHelper::MessageBoxEx(CWnd* pWnd, LPCTSTR szText, UINT nIDCaption, UINT nType)
{
	CString sCaption;

	sCaption.LoadString(nIDCaption);

	return pWnd->MessageBox(szText, sCaption, nType);
}

int CDialogHelper::RefreshMaxDropWidth(CComboBox& combo, CDC* pDCRef)
{
	int nWidth = CalcMaxTextWidth(combo, 0, TRUE, pDCRef);
	combo.SetDroppedWidth(nWidth);

	return nWidth;
}

int CDialogHelper::CalcMaxTextWidth(CComboBox& combo, int nMinWidth, BOOL bDropped, CDC* pDCRef)
{
	CDC* pDC = pDCRef;
	CFont* pOldFont = NULL;

	if (!pDC)
	{
		pDC = combo.GetDC();
		pOldFont = pDC->SelectObject(combo.GetFont());
	}

	CString sText;
	int nMaxWidth = nMinWidth;
	int nItem = combo.GetCount();

	while (nItem--)
	{
		combo.GetLBText(nItem, sText);

		int nWidth = pDC->GetTextExtent(sText).cx;
		nMaxWidth = max(nMaxWidth, nWidth);
	}

	// check window text too
	combo.GetWindowText(sText);

	int nWidth = pDC->GetTextExtent(sText).cx;
	nMaxWidth = max(nMaxWidth, nWidth);

	// dropped width needs some more space
	if (bDropped)
	{
		// Add the avg width to prevent clipping
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		nMaxWidth += tm.tmAveCharWidth;

		// Adjust the width for the vertical scroll bar and the left and right border.
		if (combo.GetStyle() & WS_VSCROLL)
		{
			nMaxWidth += ::GetSystemMetrics(SM_CXVSCROLL);
		}

		nMaxWidth += (2 * ::GetSystemMetrics(SM_CXEDGE));
	}

	// cleanup
	if (!pDCRef)
	{
		pDC->SelectObject(pOldFont);
		combo.ReleaseDC(pDC);
	}

	return nMaxWidth;
}

BOOL CDialogHelper::IsChildOrSame(HWND hWnd, HWND hwndChild)
{
	return (hWnd == hwndChild || ::IsChild(hWnd, hwndChild));
}

BOOL CDialogHelper::SelectItemByValue(CComboBox& combo, int nValue)
{
	CString sNum;
	sNum.Format(_T("%d"), nValue);

	return (CB_ERR != combo.SelectString(-1, sNum));
}

int CDialogHelper::GetSelectedItemAsValue(const CComboBox& combo)
{
	CString sNum;
	combo.GetLBText(combo.GetCurSel(), sNum);

	return _ttoi(sNum);
}

CString CDialogHelper::GetControlLabel(const CWnd* pWnd)
{
	if (!pWnd)
	{
		return _T("");
	}

	CString sText;

	// check for combo edit
	if (IsEdit(*pWnd) && IsComboBox(*(pWnd->GetParent())))
	{
		pWnd = pWnd->GetParent();
	}

	CWnd* pPrev = pWnd->GetNextWindow(GW_HWNDPREV);

	while (pPrev)
	{
		if (CWinClasses::IsClass(*pPrev, WC_STATIC))
		{
			pPrev->GetWindowText(sText);
			sText.Replace(_T("&"), _T(""));
			break;
		}

		pPrev = pPrev->GetNextWindow(GW_HWNDPREV);
	}

	return sText;
}

BOOL CDialogHelper::ControlWantsEnter(HWND hwnd)
{
	CString sClass = CWinClasses::GetClass(hwnd);
	DWORD dwStyle = ::GetWindowLong(hwnd, GWL_STYLE);

	if (CWinClasses::IsClass(sClass, WC_COMBOBOX))
	{
		// check if dropped
		if (SendMessage(hwnd, CB_GETDROPPEDSTATE, 0, 0))
		{
			return TRUE;
		}
	}
	// also check for combo's edit box and edits with ES_WANTRETURN
	else if (CWinClasses::IsClass(sClass, WC_EDIT))
	{
		if (dwStyle & ES_WANTRETURN)
		{
			return TRUE;
		}

		HWND hwndParent = ::GetParent(hwnd);

		if (hwndParent)
		{
			// check if parent is dropped combo
			if (CWinClasses::IsClass(hwndParent, WC_COMBOBOX) &&
				::SendMessage(hwndParent, CB_GETDROPPEDSTATE, 0, 0))
			{
				return TRUE;
			}

		}
	}
	// and also richedit with ES_WANTRETURN
	else if (CWinClasses::IsRichEditControl(sClass))
	{
		if (dwStyle & ES_WANTRETURN)
		{
			return TRUE;
		}
	}

	return FALSE;
}

CString CDialogHelper::GetClassName(CWnd* pWnd)
{
	ASSERT(pWnd);

	if (!pWnd)
	{
		return _T("");
	}

	CString sName;

	::GetClassName(*pWnd, sName.GetBuffer(128), 127);
	sName.ReleaseBuffer();

	return sName;
}

// static
CRect CDialogHelper::OffsetCtrl(CWnd* pParent, UINT nCtrlID, int x, int y)
{
	CWnd* pCtrl = pParent->GetDlgItem(nCtrlID);

	if (pCtrl)
	{
		return OffsetCtrl(pParent, pCtrl, x, y);
	}

	return CRect(0, 0, 0, 0);
}

CRect CDialogHelper::OffsetCtrl(CWnd* pParent, CWnd* pChild, int x, int y)
{
	ASSERT(pParent);
	ASSERT(pChild);

	CRect rChild;

	pChild->GetWindowRect(rChild);
	pParent->ScreenToClient(rChild);

	if (x || y)
	{
		rChild.OffsetRect(x, y);
		pChild->MoveWindow(rChild);
	}

	return rChild;
}

// static
CRect CDialogHelper::MoveCtrl(CWnd* pParent, UINT nCtrlID, int x, int y)
{
	CWnd* pCtrl = pParent->GetDlgItem(nCtrlID);

	if (pCtrl)
	{
		CRect rChild;
		pCtrl->GetWindowRect(rChild);
		pParent->ScreenToClient(rChild);

		if (x || y)
		{
			rChild.OffsetRect(x - rChild.left, y - rChild.top);
			pCtrl->MoveWindow(rChild);
		}

		return rChild;
	}

	return CRect(0, 0, 0, 0);
}

// static
CRect CDialogHelper::ResizeCtrl(CWnd* pParent, UINT nCtrlID, int cx, int cy)
{
	CWnd* pCtrl = pParent->GetDlgItem(nCtrlID);

	if (pCtrl)
	{
		CRect rChild, rParent;
		pCtrl->GetWindowRect(rChild);
		pParent->ScreenToClient(rChild);
		pParent->GetClientRect(rParent);

		if (cx || cy)
		{
			rChild.right += cx;
			rChild.bottom += cy;

			// make sure it also intersects with parent
			if (rChild.IntersectRect(rChild, rParent))
			{
				pCtrl->MoveWindow(rChild);
			}
		}

		return rChild;
	}

	return CRect(0, 0, 0, 0);
}

void CDialogHelper::SetControlState(CWnd* pParent, UINT nCtrlID, RT_CTRLSTATE nState)
{
	SetControlState(::GetDlgItem(*pParent, nCtrlID), nState);
}

void CDialogHelper::SetControlsState(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, RT_CTRLSTATE nState)
{
	ASSERT(pParent);

	for (UINT nID = nCtrlIDFrom; nID <= nCtrlIDTo; nID++)
	{
		SetControlState(::GetDlgItem(*pParent, nID), nState);
	}
}

void CDialogHelper::SetControlState(HWND hCtrl, RT_CTRLSTATE nState)
{
	if (hCtrl)
	{
		switch (nState)
		{
		case RTCS_ENABLED:
			::EnableWindow(hCtrl, TRUE);

			if (IsEdit(hCtrl))
			{
				::SendMessage(hCtrl, EM_SETREADONLY, FALSE, 0);
			}
			break;

		case RTCS_DISABLED:
			::EnableWindow(hCtrl, FALSE);
			break;

		case RTCS_READONLY:
			if (IsEdit(hCtrl))
			{
				::EnableWindow(hCtrl, TRUE);
				::SendMessage(hCtrl, EM_SETREADONLY, TRUE, 0);
			}
			else
			{
				BOOL bStatic = CWinClasses::IsClass(hCtrl, _T("static"));
				::EnableWindow(hCtrl, bStatic);
			}
			break;
		}
	}
}

BOOL CDialogHelper::IsEdit(HWND hCtrl)
{
	CString sClass = CWinClasses::GetClass(hCtrl);

	return (CWinClasses::IsClass(sClass, WC_EDIT) ||
		CWinClasses::IsClass(sClass, WC_RICHEDIT) ||
		CWinClasses::IsClass(sClass, WC_RICHEDIT20));
}

BOOL CDialogHelper::IsEdit(CWnd* pParent, UINT nCtrlID)
{
	ASSERT(pParent);

	return IsEdit(::GetDlgItem(*pParent, nCtrlID));
}

BOOL CDialogHelper::IsComboBox(HWND hCtrl)
{
	CString sClass = CWinClasses::GetClass(hCtrl);

	return (CWinClasses::IsClass(sClass, WC_COMBOBOX) ||
			CWinClasses::IsClass(sClass, WC_COMBOBOXEX));
}

BOOL CDialogHelper::IsComboBox(CWnd* pParent, UINT nCtrlID)
{
	ASSERT(pParent);

	return IsComboBox(::GetDlgItem(*pParent, nCtrlID));
}

void CDialogHelper::ShowControls(CWnd* pParent, UINT nCtrlIDFrom, UINT nCtrlIDTo, BOOL bShow)
{
	ASSERT(pParent);

	for (UINT nID = nCtrlIDFrom; nID <= nCtrlIDTo; nID++)
	{
		ShowControl(pParent, nID, bShow);
	}
}

void CDialogHelper::ShowControl(CWnd* pParent, UINT nCtrlID, BOOL bShow)
{
	ShowControl(pParent->GetDlgItem(nCtrlID), bShow);
}

void CDialogHelper::ShowControl(CWnd* pCtrl, BOOL bShow)
{
	if (!pCtrl)
	{
		return;
	}

	pCtrl->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}

void CDialogHelper::ExcludeControls(CWnd* pParent, CDC* pDC, UINT nCtrlIDFrom, UINT nCtrlIDTo)
{
	ASSERT(pParent);

	for (UINT nID = nCtrlIDFrom; nID <= nCtrlIDTo; nID++)
	{
		HWND hWnd = ::GetDlgItem(*pParent, nID);

		// don't clip transparent controls
		DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

		if (::IsWindowVisible(hWnd) && !(dwExStyle & WS_EX_TRANSPARENT))
		{
			pDC->ExcludeClipRect(OffsetCtrl(pParent, nID));
		}
	}
}

void CDialogHelper::ExcludeChildren(CWnd* pParent, CDC* pDC)
{
	ASSERT(pParent);

	CWnd* pChild = pParent->GetWindow(GW_CHILD);

	while (pChild)
	{
		ExcludeChild(pParent, pDC, pChild);
		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}
}

void CDialogHelper::ExcludeChild(CWnd* pParent, CDC* pDC, CWnd* pChild)
{
	ASSERT(pParent);
	ASSERT(pChild);

	// don't clip transparent controls
	DWORD dwExStyle = ::GetWindowLong(*pChild, GWL_EXSTYLE);

	if (pChild->IsWindowVisible() && !(dwExStyle & WS_EX_TRANSPARENT))
	{
		pDC->ExcludeClipRect(OffsetCtrl(pParent, pChild));
	}
}
