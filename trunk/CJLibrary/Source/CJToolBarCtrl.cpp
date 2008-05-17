// CJToolBarCtrl.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new IE4 controls'
// http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 8/31/99 1:11a $
 * $Revision: 5 $
 * $Archive: /CodeJockey/CJLibrary/CJToolBarCtrl.cpp $
 *
 * $History: CJToolBarCtrl.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/28/99    Time: 12:38a
 * Updated in $/CodeJockey/CJLibrary
 * Final pass for release 6.08.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:29p
 * Created in $/CodeJockey/CJLibrary
 * Copied from MFC v6 and techno preview for v5. Added to extend
 * functionality to Visual C++ 5.0 users.
 *
 ***************************************************************************/

#include "stdafx.h"
#include "CJToolBarCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJToolBarCtrl

CCJToolBarCtrl::~CCJToolBarCtrl()
{
  DestroyWindow();
}


BEGIN_MESSAGE_MAP(CCJToolBarCtrl, CToolBarCtrl)
  //{{AFX_MSG_MAP(CCJToolBarCtrl)
  ON_WM_CREATE()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJToolBarCtrl message handlers

CCJToolBarCtrl::CCJToolBarCtrl()
{
  // initialize common controls
  INITCOMMONCONTROLSEX icex;
  icex.dwSize = sizeof(icex);
  icex.dwICC = ICC_BAR_CLASSES;
  VERIFY(InitCommonControlsEx(&icex));
}

BOOL CCJToolBarCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
                            UINT nID)
{
  CWnd* pWnd = this;
  BOOL bCreated = pWnd->Create(TOOLBARCLASSNAME, NULL, dwStyle, rect, pParentWnd, nID);

  return bCreated;
}

int CCJToolBarCtrl::OnCreate(LPCREATESTRUCT lpcs)
{
  if (CWnd::OnCreate(lpcs) == -1)
    return -1;
  SetButtonStructSize(sizeof(TBBUTTON));
  return 0;
}

HRESULT CCJToolBarCtrl::GetDropTarget(IDropTarget** ppDropTarget) const
{
  ASSERT(::IsWindow(m_hWnd));
  ASSERT(ppDropTarget);
  return (HRESULT) ::SendMessage(m_hWnd, TB_GETOBJECT, (WPARAM)&IID_IDropTarget, (LPARAM)ppDropTarget);
}
