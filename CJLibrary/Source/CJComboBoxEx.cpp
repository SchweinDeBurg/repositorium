// CJComboBoxEx.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new 
// IE4 controls' http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 8/31/99 1:11a $
 * $Revision: 5 $
 * $Archive: /CodeJockey/CJLibrary/CJComboBoxEx.cpp $
 *
 * $History: CJComboBoxEx.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/25/99    Time: 12:05p
 * Created in $/CodeJockey/CJLibrary
 * Copied from MFC, Visual C++ 6. Added to extend functionality to Visual
 * C++ 5 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJComboBoxEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJComboBoxEx

BOOL CCJComboBoxEx::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
	UINT nID)
{
  // initialize common controls
#ifdef _VC_VERSION_5
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_USEREX_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));
#else
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_USEREX_REG));
#endif

	CWnd* pWnd = this;
	return pWnd->Create(WC_COMBOBOXEX, NULL, dwStyle, rect, pParentWnd, nID);
}

int CCJComboBoxEx::DeleteItem(int iIndex)
{
	ASSERT(::IsWindow(m_hWnd));
	return (int) ::SendMessage(m_hWnd, CBEM_DELETEITEM, (WPARAM) iIndex, 0);
}

BOOL CCJComboBoxEx::GetItem(COMBOBOXEXITEM* pCBItem)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pCBItem != NULL);
	ASSERT(AfxIsValidAddress(pCBItem, sizeof(COMBOBOXEXITEM)));
	
	return (int) ::SendMessage(m_hWnd, CBEM_GETITEM, 0, (LPARAM) pCBItem);
}

int CCJComboBoxEx::InsertItem(const COMBOBOXEXITEM* pCBItem)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pCBItem != NULL);
	ASSERT(AfxIsValidAddress(pCBItem, sizeof(COMBOBOXEXITEM), FALSE));
	
	return (int) ::SendMessage(m_hWnd, CBEM_INSERTITEM, 0, (LPARAM) pCBItem);
}

int CCJComboBoxEx::InsertItem(int iItem, UINT nStringID, int iIndent, int iImage, int iSelectedImage, UINT mask)
{
	CString strItem;
	strItem.LoadString(nStringID);
	return InsertItem(iItem, strItem, iIndent, iImage, iSelectedImage, mask);
}

int CCJComboBoxEx::InsertItem(int iItem, LPCTSTR lpszItem, int iIndent, int iImage, int iSelectedImage, UINT mask)
{
	COMBOBOXEXITEM cbItem;

	cbItem.mask				= mask;
	cbItem.iItem			= iItem;
	cbItem.pszText			= const_cast<LPTSTR>(lpszItem);
	cbItem.iImage			= iImage;
	cbItem.iSelectedImage	= iSelectedImage;
	cbItem.iIndent			= iIndent;
	
	return InsertItem(&cbItem);
}

BOOL CCJComboBoxEx::SetItem(const COMBOBOXEXITEM* pCBItem)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pCBItem != NULL);
	ASSERT(AfxIsValidAddress(pCBItem, sizeof(COMBOBOXEXITEM), FALSE));
	
	return (int) ::SendMessage(m_hWnd, CBEM_SETITEM, 0, (LPARAM) pCBItem);
}

CCJComboBoxEx::~CCJComboBoxEx()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CCJComboBoxEx, CComboBox)

/////////////////////////////////////////////////////////////////////////////

