/*
Module : TOASTERWND.CPP
Purpose: Defines the implementation for an MFC class which implements a MSN Messenger style notification window 
         aka a "Toast" window, since it pops up!. Based in part on a C# implementation of a similiar type
         window by Robert Misiak on codeproject (http://www.codeproject.com/cs/miscctrl/RobMisNotifyWindow.asp)
Created: PJN / 22-04-2005
History: PJN / 23-04-2005 1. Title and body text can now be optionally made hot or not.
                          2. Icon can now be hot.
         PJN / 24-04-2005 1. Code now uses SHAppBarMessage to determine the task bar location.
                          2. popup window is now by default offset horizontally as well as vertically by a 
                          scrollbar
         PJN / 25-04-2005 1. Now supports a new transparency animation effect (if on Win2k or later), via a new 
                          the m_bTransparent member variable. Thanks to Remio for this really cool addition.
         PJN / 12-05-2006 1. Fixed a minor bug in CToasterWnd::Construct where function pointers were not been 
                          checked correctly. Thanks to Wes Aday for reporting this issue.
                          2. Fixed a bug in CToasterWnd::OnEraseBkgnd where the vertex values were not being set
                          correctly for the BackgroundDiagonalGradient and ForwardDiagonalGradient styles. Again 
                          thanks to Wes Aday for reporting this bug.
         PJN / 08-07-2006 1. Code now uses newer C++ style casts instead of C style casts.
                          2. Updated the code to clean compile on VC 2005
                          3. Close button is now shown hot only when the cursor is within its bounding rect.
                          4. Reworked the notification API somewhat. If you are using notifications, then you 
                          should review the new layout of the CToasterNotification class.
                          5. Class now allows you to put HTML text into the toast window. This is provided by the 
                          QHTM HTML control provided at http://www.gipsysoft.com/qhtm/. You may need to purchase a 
                          license (depending on the type of application you are developing) as well as follow their 
                          instructions on deploying it if you want this functionality with CToasterWnd. This functionality 
                          by default is not included in CToasterWnd and is only available when you define the 
                          preprocessor macro "CTOASTERWND_HTML_SUPPORT".
                          6. Included support for a new "NoAnimation" animation style.
         PJN / 14-12-2006 1. Code now uses class name constant for QHTML control
                          2. Updated sample app code to clean compile on VC 2005
                          3. Macro to control inclusion of QHTM is now "CTOASTERWND_QHTM_SUPPORT"
                          4. Control now allows you to use the HTMLayout control for HTML support for the toast window.
                          For more information about HTMLayout, please see http://www.terrainformatica.com/htmlayout/
         PJN / 17-03-2007 1. Updated copyright details.
                          2. Removed the automatic inclusion of the various HTMLayout behaviors header files. This avoids
                          causing potential linking errors in client apps, since the behavior objects are declared statically 
                          twice. Note that if you want to support HTMLayout behaviors, then it is the responsibility of
                          client apps to pull in the required HTMLayout header files. Thanks to Itamar Syn-Hershko for 
                          reporting this problem.
                          3. Addition of 2 simple LoadHTML and LoadUrl helper methods to allow the toaster window to
                          easily be populated with HTML when using the HTMLayout control. Thanks to Itamar Syn-Hershko for 
                          this nice addition.
                          4. Addition of a m_dwHTML and m_pbHTML member variables when you are using the HTMLayout control.
                          If you populate these values, then the HTML for the control will be taken from these variables
                          instead of from "m_sText". Again thanks to Itamar Syn-Hershko for this nice addition.
         PJN / 31-03-2007 1. The existing CToasterWnd::Show method has been internally refactored into a Create method and
                          Popup method. This allows client code to perform any custom initialization on the control
                          prior to it actually being shown to the end user. Thanks to Itamar Syn-Hershko for prompting
                          this suggestion.
         PJN / 01-04-2007 1. Fixed a bug in the LoadHTML and LoadUrl methods where they were incorrectly using the main
                          window handle of the toaster window instead of the window handle of the HTMLayout child control.
                          Thanks to Itamar Syn-Hershko for reporting this bug.
         PJN / 21-07-2007 1. Updated usage instructions about Platform SDK.
                          2. The CToasterNotification notifier interface has been extended to support a 
                          "OnHTMLayoutLoadData" method. This allows client code to customize the loading of data when 
                          using the HTMLayout control without the need to implement a derived class of CToasterWnd. Thanks 
                          to Itamar Syn-Hershko for reporting this suggestion.
                          3. OnHTMLLayoutHyperLink method in CToasterNotification has been renamed to OnHTMLayoutHyperLink
                          4. OnHTMLLayoutCommand method in CToasterNotification has been renamed to OnHTMLayoutCommand
         PJN / 20-08-2007 1. Removed an unnecessary ASSERT in CToasterWnd::Popup.
                          2. Internal animation state variable is reset in Close(), OnDestroy(), and HandleClosing methods.
                          3. Updated the sample app to show how you can reuse a CToasterWnd window if you so desire. Thanks 
                          to Itamar Syn-Hershko for suggesting this update.
         PJN / 13-07-2008 1. Updated copyright details.
                          2. Updated sample app to clean compile on VC 2008
                          3. The code has now been updated to support VC 2005 or later only. 
                          4. Code now compiles cleanly using Code Analysis (/analyze)
                          5. Removed VC 6 style AppWizard comments from the code.
         PJN / 17-08-2008 1. Fixed a bug where if the calculated body text height was larger than the allowable window size, 
                          you previously got the title text intersecting with the title text. Thanks to Steven Dwyer for 
                          reporting this issue.
                          2. Changed the default DrawText formatting flags for the body text to 
                          DT_NOPREFIX | DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL. This is a better default value for the
                          typical body text
                          3. Changed the default DrawText formatting flags for the title text to 
                          DT_NOPREFIX | DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS. This is a better default value for the
                          typical title text
         PJN / 10-12-2008 1. The timer interval used for the opening and closing animation is now controllable via a member
                          variable
                          2. The size of the pixels in which to increase or decrease the window size during the animation
                          can now be controlled via a member variable
                          3. Fixed a bug where the window could end completely transparent after the initial opening animation
                          4. Fixed a bug where the final size of the window could end up incorrect
                          5. The control now uses double buffered drawing to help eliminate flickering when the opening and
                          closing animation is happening. Thanks to "sky" for prompting this update.
         PJN / 14-12-2008 1. The control now supports drawing an alpha blended image in its client area. This is achieved using
                          the new member variables: m_Logo, m_rectLogoSrc, m_rectLogoDest and m_nLogoSrcAlpha. The code 
                          internally uses the CImage class and its support for alpha blending via the Win32 API function
                          AlphaBlend. To use this functionality in the control you must define the preprocessor variable
                          WINVER to be >= 500. Thanks to "Sky" for prompting this update.

Copyright (c) 2005 - 2008 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "ToasterWnd.h"

#ifndef _UXTHEME_H_
#pragma message("To avoid this message, please put uxtheme.h in your pre compiled header (normally stdafx.h)")
#include <uxtheme.h>
#endif

#ifndef __AFXPRIV_H__
#pragma message("To avoid this message, please put afxpriv.h in your pre compiled header (normally stdafx.h)")
#include <afxpriv.h>
#endif

#ifndef TMSCHEMA_H
#pragma message("To avoid this message, please put tmschema.h in your pre compiled header (normally stdafx.h)")
#include <tmschema.h>
#endif

#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
//Automatically pull in support for HTMLayout
#pragma comment(lib, "HTMLayout.lib")
#endif

#ifdef CTOASTERWND_QHTM_SUPPORT
#ifndef QHTM_H
#include <qhtm.h> //If you get a compilation error on this line, then you need to download, install and possible purchase a license for QHTM (http://www.gipsysoft.com/qhtm/)
#endif
#endif


//////////////////////////////// Statics / Macros /////////////////////////////

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000
#endif

#ifndef LWA_ALPHA
#define LWA_ALPHA 0x00000002
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

const UINT TOASTERWND_HTML_ID = 100;
const UINT TOASTERWND_TIMER_ID = 1;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////// Implementation /////////////////////////////

void CToasterNotification::OnBodyTextClicked(CToasterWnd* /*pFrom*/)
{
  TRACE(_T("CToasterNotification::OnBodyTextClicked called\n"));
}

void CToasterNotification::OnTitleTextClicked(CToasterWnd* /*pFrom*/)
{
  TRACE(_T("CToasterNotification::OnTitleTextClicked called\n"));
}

void CToasterNotification::OnIconClicked(CToasterWnd* /*pFrom*/)
{
  TRACE(_T("CToasterNotification::OnIconClicked called\n"));
}

void CToasterNotification::OnClose(CToasterWnd* /*pFrom*/, BOOL /*bButton*/)
{
  TRACE(_T("CToasterNotification::OnClose called\n"));
}

void CToasterNotification::OnStateChange(CToasterWnd* /*pFrom*/)
{
  TRACE(_T("CToasterNotification::OnStateChange called\n"));
}

CRect CToasterNotification::CalculateInitialPopupPosition(CToasterWnd* pFrom)
{
  AFXASSUME(pFrom);
  return pFrom->CalculateInitialPopupPosition();
}

#ifdef CTOASTERWND_QHTM_SUPPORT
LRESULT CToasterNotification::OnQHTMHyperLink(CToasterWnd* /*pFrom*/, LPCTSTR /*pcszLinkText*/, LPCTSTR /*pcszLinkID*/)
{
  return TRUE; //by default allow all hyperlinks to be handled by QHTM
}
#endif

#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
void CToasterNotification::OnHTMLayoutHyperLink(CToasterWnd* /*pFrom*/, NMHL_HYPERLINK* /*pHyperlink*/)
{
}

void CToasterNotification::OnHTMLayoutCommand(CToasterWnd* /*pFrom*/, NMHL_COMMAND_CLICK* /*pCommand*/)
{
}

LRESULT CToasterNotification::OnHTMLayoutLoadData(CToasterWnd* /*pFrom*/, NMHDR* /*pnmh*/)
{
	return 0;
}
#endif


#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
BEGIN_MESSAGE_MAP(CHtmlLayoutReflectorWnd, CWnd)
	ON_MESSAGE(WM_BEHAVIOR_NOTIFY, OnBehaviorNotify)
END_MESSAGE_MAP()

LRESULT CHtmlLayoutReflectorWnd::OnBehaviorNotify(WPARAM wParam, LPARAM lParam)
{
  //If we have a reflector setup, then pass handling on to it
	if (m_pReflectorWnd)
  {
    NMHDR* pHDR = reinterpret_cast<NMHDR*>(lParam);
	  switch (pHDR->code) 
    {
		  case HLN_HYPERLINK:         
		  {
        m_pReflectorWnd->OnHTMLayoutHyperLink(reinterpret_cast<NMHL_HYPERLINK*>(lParam));
        break;
      }
      case HLN_COMMAND_CLICK:
      {
        m_pReflectorWnd->OnHTMLayoutHtmlCommand(reinterpret_cast<NMHL_COMMAND_CLICK*>(lParam));
        break;
      }
      default:
      {
        break;
      }
	  }
  }
  return 0;
}
#endif

BEGIN_MESSAGE_MAP(CToasterWnd, CFrameWnd)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_CLOSE()
  ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
	ON_NOTIFY(HLN_LOAD_DATA, TOASTERWND_HTML_ID, OnHTMLayoutLoadData)
	ON_NOTIFY(HLN_ATTACH_BEHAVIOR, TOASTERWND_HTML_ID, OnHTMLayoutAttachBehavior)
#endif
#ifdef CTOASTERWND_QHTM_SUPPORT
	ON_NOTIFY(QHTMN_HYPERLINK, TOASTERWND_HTML_ID, OnQHTMHyperlink)
#endif
END_MESSAGE_MAP()

CToasterWnd::CToasterWnd() 
{
  Construct();
}

CToasterWnd::CToasterWnd(const CString& sTitle, const CString& sText)
{
  Construct();
  m_sTitle = sTitle;
  m_sText = sText;
}

CToasterWnd::CToasterWnd(const CString& sText)
{
  Construct();
  m_sText = sText;
}

CToasterWnd::~CToasterWnd()
{
  if (m_hUXTheme)
    FreeLibrary(m_hUXTheme);
  if (m_hMsimg32)
    FreeLibrary(m_hMsimg32);
  if (m_cursorHand)
    DestroyCursor(m_cursorHand);
  if (m_TitleIcon)
    DestroyIcon(m_TitleIcon);
}

void CToasterWnd::Construct()
{
	m_dwTimerInterval = 15;
	m_dwAnimationMoveSize = 5;
  m_colorTitle = GetSysColor(COLOR_WINDOWTEXT);
  m_colorText = m_colorTitle;
  m_colorBackground = RGB(70, 130, 180);  //steel blue
  m_colorGradient = RGB(0xF5, 0xF5, 0xF5); //white smoke
  m_dwWaitTime = 10000; //Default to 10 seconds
  m_nHeight = 116;
  m_nWidth = 181;
#if WINVER >= 0x0500
  m_nLogoSrcAlpha = 255;
  m_rectLogoSrc = CRect(0, 0, m_nWidth, m_nHeight);
  m_rectLogoDest = m_rectLogoSrc;
#endif  
  m_BackgroundStyle = BackwardDiagonalGradient;
  m_bWaitOnMouseOver = TRUE;
  m_nTimerID = 0;
  m_AnimationState = None;
  m_bClosePressed = FALSE;
  m_bCloseHot = FALSE;
  m_bTitlePressed = FALSE;
  m_bIconPressed = FALSE;
  m_bBodyTextPressed = FALSE;
  m_dwTitleStyle = DT_NOPREFIX | DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS;
  m_dwTextStyle = DT_NOPREFIX | DT_CENTER | DT_WORDBREAK | DT_EDITCONTROL;
  m_TitleIcon = NULL;
  m_AnimationStyle = AutoSet;
  m_PopupPosition = AutoDetect;
  m_nCXAreaEdge = GetSystemMetrics(SM_CXVSCROLL);
  m_nCYAreaEdge = GetSystemMetrics(SM_CYHSCROLL);
  m_pNotifier = NULL;
  m_bUseThemingForCloseButton = TRUE;
  m_bSafeToClose = FALSE;
  m_bTitleHot = TRUE;
  m_bTextHot = TRUE;
  m_bIconHot = TRUE;
  m_bTransparent = TRUE;
#if defined(CTOASTERWND_HTMLAYOUT_SUPPORT) || defined(CTOASTERWND_QHTM_SUPPORT)
  m_bHTML = FALSE;
#endif

#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
  m_dwHTML = 0;
  m_pbHTML = NULL;
  m_wndHTMLayout.m_pReflectorWnd = this;
#endif

  //Dynamically pull in the uxtheme functions
  m_hUXTheme = LoadLibrary(_T("UxTheme.dll"));
  if (m_hUXTheme)
  {
    m_lpfnOpenThemeData = reinterpret_cast<LPOPENTHEMEDATA>(GetProcAddress(m_hUXTheme, "OpenThemeData"));
    m_lpfnDrawThemeBackground = reinterpret_cast<LPDRAWTHEMEBACKGROUND>(GetProcAddress(m_hUXTheme, "DrawThemeBackground"));
    m_lpfnIsThemeActive = reinterpret_cast<LPISTHEMEACTIVE>(GetProcAddress(m_hUXTheme, "IsThemeActive"));
    m_lpfnCloseThemeData = reinterpret_cast<LPCLOSETHEMEDATA>(GetProcAddress(m_hUXTheme, "CloseThemeData"));

    if ((m_lpfnOpenThemeData == NULL) || (m_lpfnDrawThemeBackground == NULL) || 
        (m_lpfnIsThemeActive == NULL) || (m_lpfnCloseThemeData == NULL))
    {
      m_lpfnOpenThemeData = NULL;
      m_lpfnDrawThemeBackground = NULL;
      m_lpfnIsThemeActive = NULL;
      m_lpfnCloseThemeData = NULL;

      FreeLibrary(m_hUXTheme);
      m_hUXTheme = NULL;
    }
  }
  else
  {
    m_lpfnOpenThemeData = NULL;
    m_lpfnDrawThemeBackground = NULL;
    m_lpfnIsThemeActive = NULL;
    m_lpfnCloseThemeData = NULL;
  }

  //Dynamically pull in the GradientFill function
  m_hMsimg32 = LoadLibrary(_T("Msimg32.dll"));
  if (m_hMsimg32)
  {
    m_lpfnGradientFill = reinterpret_cast<LPGRADIENTFILL>(GetProcAddress(m_hMsimg32, "GradientFill"));
    if (m_lpfnGradientFill == NULL)
    {
      FreeLibrary(m_hMsimg32);
      m_hMsimg32 = NULL;
    }
  }
  else
    m_lpfnGradientFill = NULL;

  //Determine if the OS supports layered windows
  HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
  if (hUser32)
    m_lpfnSetLayeredWindowAttributes = reinterpret_cast<LPSETLAYEREDWINDOWATTRIBUTES>(GetProcAddress(hUser32, "SetLayeredWindowAttributes"));
  else 
    m_lpfnSetLayeredWindowAttributes = NULL;
  
  //Load up the hand cursor
  m_cursorHand = LoadCursor(NULL, IDC_HAND);
}

CRect CToasterWnd::CalculateInitialPopupPosition()
{
  //Get the task bar position
  APPBARDATA abd;
  abd.cbSize = sizeof(abd);
  abd.hWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);
  SHAppBarMessage(ABM_GETTASKBARPOS, &abd);

  //Work out the popup position to use if "AutoDetect" was specified
  m_actualPopupPosition = m_PopupPosition;
  if (m_actualPopupPosition == AutoDetect)
  {
    switch (abd.uEdge)
    {
      case ABE_BOTTOM:
      {
        m_actualPopupPosition = BottomRight;
        break;
      }
      case ABE_LEFT:
      {
        m_actualPopupPosition = BottomLeft;
        break;
      }
      case ABE_TOP:
      {
        m_actualPopupPosition = TopRight;
        break;
      }
      case ABE_RIGHT:
      {
        m_actualPopupPosition = BottomRight;
        break;
      }
      default:
      {
        m_actualPopupPosition = BottomRight; //Default to something if SHAppBarMessage failed (e.g Explorer gone AWOL)
        break;
      }
    }
  }

  //Work out the animation style if "AutoSet" was specified
  m_actualAnimationStyle = m_AnimationStyle;
  if (m_actualAnimationStyle == AutoSet)
  {
    switch (abd.uEdge)
    {
      case ABE_BOTTOM:
      {
        m_actualAnimationStyle = PopUp;
        break;
      }
      case ABE_LEFT:
      {
        m_actualAnimationStyle = PopLeft;
        break;
      }
      case ABE_TOP:
      {
        m_actualAnimationStyle = PopDown;
        break;
      }
      case ABE_RIGHT:
      {
        m_actualAnimationStyle = PopRight;
        break;
      }
      default:
      {
        m_actualAnimationStyle = PopUp; //Default to something if SHAppBarMessage failed (e.g Explorer gone AWOL)
        break;
      }
    }
  }

  //Work out where the window should be initially shown
  CRect rectInitialPos;
  switch (m_actualAnimationStyle)
  {
    case PopUp:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.top + m_nCYAreaEdge + m_nHeight);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(FALSE);
          break;
        }
      }
      break;
    }
    case PopDown:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.top + m_nCYAreaEdge);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.right - 8, m_rectScreen.top + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(FALSE);
          break;
        }
      }
      break;
    }
    case PopLeft:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(FALSE);
          break;
        }
      }
      break;
    }
    case PopRight:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nWidth + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.left + m_nWidth + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nWidth + m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.left + m_nWidth + m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(FALSE);
          break;
        }
      }
      break;
    }
    case NoAnimation:
    {
      switch (m_actualPopupPosition)
      {
        case BottomRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case BottomLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.bottom - m_nCYAreaEdge - m_nHeight, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.bottom - m_nCYAreaEdge);
          break;
        }
        case TopLeft:
        {
          rectInitialPos = CRect(m_rectScreen.left + m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.left + m_nCXAreaEdge + m_nWidth, m_rectScreen.top + m_nCYAreaEdge + m_nHeight);
          break;
        }
        case TopRight:
        {
          rectInitialPos = CRect(m_rectScreen.right - m_nWidth - m_nCXAreaEdge, m_rectScreen.top + m_nCYAreaEdge, m_rectScreen.right - m_nCXAreaEdge, m_rectScreen.top + m_nHeight + m_nCYAreaEdge);
          break;
        }
        default:
        {
          ASSERT(FALSE);
          break;
        }
      }
      break;
    }
    default:
    {
      ASSERT(FALSE);
      break;
    }
  }

  return rectInitialPos;
}

void CToasterWnd::CreateFonts()
{
  //Create the various fonts if not already created
  CFont* pCurrentFont = GetFont();
  CFont defaultGUIFont;
  if (pCurrentFont == NULL)
  {
    defaultGUIFont.Attach(GetStockObject(DEFAULT_GUI_FONT));
    pCurrentFont = &defaultGUIFont;
  }
  ASSERT(pCurrentFont);
  if (m_fontTitle.operator HFONT() == NULL)
  {
    LOGFONT lf;
    pCurrentFont->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    m_fontTitle.CreateFontIndirect(&lf);
  }
  if (m_fontText.operator HFONT() == NULL)
  {
    LOGFONT lf;
    pCurrentFont->GetLogFont(&lf);
    m_fontText.CreateFontIndirect(&lf);
  }
}

BOOL CToasterWnd::Create()
{
  //validate our parameters
#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
  if (m_bHTML)
    ASSERT((m_pbHTML && m_dwHTML) || m_sText.GetLength()); //If we are using HTMLayout, we support loading from the raw byte pointer or from the CString member variable
#else  
  ASSERT(m_sText.GetLength()); //What no Text!!
#endif  

  //Get the working area for the primary monitor
  if (!SystemParametersInfo(SPI_GETWORKAREA, NULL, &m_rectScreen, 0))
    return FALSE;
  
  //Do the initial calculation. Note we allow the notifier an initial crack
  //at deciding at where the window should be located. This means that 
  //notifiers could potentially handle stacking of multiple windows similiar
  //to what MSN Messenger does
  CRect rectInitialPos;
  if (m_pNotifier)
    m_rectInitialPos = m_pNotifier->CalculateInitialPopupPosition(this);
  else
    m_rectInitialPos = CalculateInitialPopupPosition();  

  //create the window using the appropiate styles 
  DWORD dwExFlags = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
  if (m_bTransparent && m_lpfnSetLayeredWindowAttributes && (m_actualAnimationStyle != NoAnimation))
    dwExFlags |= WS_EX_LAYERED;
  if (!CFrameWnd::Create(NULL, m_sTitle, WS_POPUP, m_rectInitialPos, NULL, NULL, dwExFlags))
    return FALSE;

  //Do all the various calculation / setup code (if required)
  BOOL bNeedInitCode = TRUE;
#if defined(CTOASTERWND_HTMLAYOUT_SUPPORT) || defined(CTOASTERWND_QHTM_SUPPORT)
  if (m_bHTML)
  {
    bNeedInitCode = FALSE;

    m_rectClose = CRect(-1, -1, -1, -1);
    m_rectIcon = CRect(-1, -1, -1, -1);
    m_rectTitle = CRect(-1, -1, -1, -1);
    m_rectBodyText = CRect(-1, -1, -1, -1);
  }
#endif
  if (bNeedInitCode)
  {
    //Create the fonts
    CreateFonts();

    //Calculate the position of the close button
    CRect rectClient;
    GetClientRect(&rectClient);

    if (m_actualAnimationStyle == PopLeft || m_actualAnimationStyle == PopRight)
      m_rectClose = CRect(rectClient.left + m_nWidth - 26, 5, rectClient.left + m_nWidth - 13, 18);
    else
      m_rectClose = CRect(rectClient.right - 26, 5, rectClient.right - 13, 18);

    //Calculate the position of the icon
    if (m_TitleIcon)
      m_rectIcon = CRect(rectClient.left + 8, 5, GetSystemMetrics(SM_CXSMICON) + rectClient.left + 8, GetSystemMetrics(SM_CYSMICON) + 5);
    else
      m_rectIcon = CRect(-1, -1, -1, -1);

    //Calculate the position of the title text
    int nOffset;
    if (m_sTitle.GetLength())
    {
      CDC* pDC = GetDC();
      CFont* pOldFont = pDC->SelectObject(&m_fontTitle);

      CSize sizeTitle = pDC->GetTextExtent(m_sTitle);
      m_rectTitle = CRect(max(5, m_rectIcon.right + 5), 5, m_rectClose.left - 5, 5 + sizeTitle.cy);
      nOffset = max(max(m_rectTitle.bottom + 5, m_rectClose.bottom + 5), m_rectIcon.bottom + 5);

      if (pOldFont)
        pDC->SelectObject(pOldFont);

      ReleaseDC(pDC);
    }
    else
    {
      nOffset = m_rectClose.bottom + 2;
      m_rectTitle = CRect(-1, -1, -1, -1);
    }

    //Calculate the position of the body text
    int nBodyTextRight;
    if (m_actualAnimationStyle == PopLeft || m_actualAnimationStyle == PopRight)
    {
      nBodyTextRight = rectClient.left + m_nWidth - 7;
      m_rectBodyText = CRect(7, nOffset, nBodyTextRight, nOffset);
    }
    else
    {
      nBodyTextRight = rectClient.right - 7;
      m_rectBodyText = CRect(7, nOffset, nBodyTextRight, nOffset);
    }
    CDC* pDC = GetDC();
    CFont* pOldFont = pDC->SelectObject(&m_fontText);
    pDC->DrawText(m_sText, &m_rectBodyText, m_dwTextStyle | DT_CALCRECT);
    m_rectBodyText.right = nBodyTextRight;
    int nBodyHeight = m_rectBodyText.Height();
    int nNewTop = (m_nHeight - nBodyHeight - nOffset - 5) / 2 + nOffset;
    m_rectBodyText = CRect(m_rectBodyText.left, nNewTop, m_rectBodyText.right, nNewTop + nBodyHeight);
    if (m_rectBodyText.top < nOffset)
      m_rectBodyText.top = nOffset;
    if (m_rectBodyText.bottom > (m_nHeight - 7))
      m_rectBodyText.bottom = (m_nHeight - 7);
  
    if (pOldFont)
      pDC->SelectObject(pOldFont);
  
    ReleaseDC(pDC);
  }

  return TRUE;
}

void CToasterWnd::Popup()
{
  //Do the initial calculation. Note we allow the notifier an initial crack
  //at deciding at where the window should be located. This means that 
  //notifiers could potentially handle stacking of multiple windows similiar
  //to what MSN Messenger does
  CRect rectInitialPos;
  if (m_pNotifier)
    m_rectInitialPos = m_pNotifier->CalculateInitialPopupPosition(this);
  else
    m_rectInitialPos = CalculateInitialPopupPosition();  
    
  //Reset the window position to be the initial popup position
  SetWindowPos(NULL, m_rectInitialPos.left, m_rectInitialPos.top, m_rectInitialPos.Width(), m_rectInitialPos.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);

  //Create the timer and set the state machine state
  if (m_actualAnimationStyle != NoAnimation)
  {
    m_nTimerID = SetTimer(TOASTERWND_TIMER_ID, m_dwTimerInterval, NULL); 
    m_AnimationState = Opening;
  }
  else
  {
    m_nTimerID = SetTimer(TOASTERWND_TIMER_ID, m_dwWaitTime, NULL);
    m_AnimationState = Showing;
  }

  //Show the window (without activation)
  ShowWindow(SW_SHOWNOACTIVATE);

  //Inform the notifier
  if (m_pNotifier)
    m_pNotifier->OnStateChange(this);
}

BOOL CToasterWnd::Show()
{
  //Call our two helper functions to do the heavy lifting
  if (!Create())
    return FALSE;
    
  Popup();
  
  return TRUE;
}

void CToasterWnd::Hide()
{
  if (m_AnimationState == Opening || m_AnimationState == Showing)
  {
    //Change to the closing state and reset the timer
    m_AnimationState = Closing;
    SetTimer(m_nTimerID, m_dwTimerInterval, NULL);
    if (m_pNotifier)
      m_pNotifier->OnStateChange(this);
  }
}

void CToasterWnd::Close()
{
  m_bSafeToClose = TRUE;
  m_AnimationState = None;
  DestroyWindow();
}

void CToasterWnd::OnDestroy() 
{
  //Kill the timer if active
  if (m_nTimerID)
  {
    KillTimer(m_nTimerID);
    m_nTimerID = 0;
  }
  
  //Let the base class do its thing
	CFrameWnd::OnDestroy();
	
  m_AnimationState = None;
}

void CToasterWnd::UpdateTransparency(BYTE byAlpha)
{
  //Call the SetLayeredWindowAttributes to do the work for us
  if (m_bTransparent && m_lpfnSetLayeredWindowAttributes)
    m_lpfnSetLayeredWindowAttributes(m_hWnd, 0, byAlpha, LWA_ALPHA);
}

void CToasterWnd::OnTimer(UINT_PTR nIDEvent) 
{
  if (nIDEvent == m_nTimerID) //Is it the timer we are interested in?
  {
    switch (m_AnimationState)
    {
      case Opening:
      {
        //Get the window rect for the control
        CRect rectWindow;
        GetWindowRect(&rectWindow);
        
        switch (m_actualAnimationStyle)
        {
          case PopUp:
          {
            if ((rectWindow.Height() >= m_nHeight))
            {
              //We have fully opened, change to the showing state and reset the timer
              m_AnimationState = Showing;
              SetTimer(m_nTimerID, m_dwWaitTime, NULL);
              if (m_pNotifier)
                m_pNotifier->OnStateChange(this);
            }
            else
            {
              rectWindow.top -= m_dwAnimationMoveSize;
              int nFinalTop = rectWindow.bottom - m_nHeight;
              if (rectWindow.top < nFinalTop)
								rectWindow.top = nFinalTop;
              SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Height() / m_nHeight)); 
            }
            break;
          }
          case PopDown:
          {
            if (rectWindow.Height() >= m_nHeight)
            {
              //We have fully opened, change to the showing state and reset the timer
              m_AnimationState = Showing;
              UpdateTransparency(255);
              SetTimer(m_nTimerID, m_dwWaitTime, NULL);
              if (m_pNotifier)
                m_pNotifier->OnStateChange(this);
            }
            else
            {
              rectWindow.bottom += m_dwAnimationMoveSize;
              int nFinalBottom = rectWindow.top + m_nHeight;
              if (rectWindow.bottom > nFinalBottom)
								rectWindow.bottom = nFinalBottom;
              SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Height() / m_nHeight)); 
            }
            break;
          }
          case PopLeft:
          {
            if ((rectWindow.Width() >= m_nWidth))
            {
              //We have fully opened, change to the showing state and reset the timer
              m_AnimationState = Showing;
              UpdateTransparency(255);
              SetTimer(m_nTimerID, m_dwWaitTime, NULL);
              if (m_pNotifier)
                m_pNotifier->OnStateChange(this);
            }
            else
            {
              rectWindow.right += m_dwAnimationMoveSize;
              int nFinalRight = rectWindow.left + m_nWidth;
              if (rectWindow.right > nFinalRight)
								rectWindow.right = nFinalRight;
              SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Width() / m_nWidth)); 
            }
            break;
          }
          case PopRight:
          {
            if ((rectWindow.Width() >= m_nWidth))
            {
              //We have fully opened, change to the showing state and reset the timer
              m_AnimationState = Showing;
              UpdateTransparency(255);
              SetTimer(m_nTimerID, m_dwWaitTime, NULL);
              if (m_pNotifier)
                m_pNotifier->OnStateChange(this);
            }
            else
            {
              rectWindow.left -= m_dwAnimationMoveSize;
              int nFinalLeft = rectWindow.right - m_nWidth;
              if (rectWindow.left < nFinalLeft)
								rectWindow.left = nFinalLeft;
              SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Width() / m_nWidth)); 
            }
            break;
          }
          default:
          {
            ASSERT(FALSE);
            break;
          }
        }
        break;
      }
      case Showing:
      {
        //Get the window rect for the control
        CRect rectWindow;
        GetWindowRect(&rectWindow);

        CPoint cursorPos;
        GetCursorPos(&cursorPos);
        if (!m_bWaitOnMouseOver || !rectWindow.PtInRect(cursorPos))
        {
          if (m_actualAnimationStyle != NoAnimation)
            Hide();
          else
            HandleClosing(FALSE);
        }
        break;
      }
      case Closing:
      {
        //Get the window rect for the control
        CRect rectWindow;
        GetWindowRect(&rectWindow);

        switch (m_actualAnimationStyle)
        {
          case PopUp:
          {
            rectWindow.top += m_dwAnimationMoveSize;
            if (rectWindow.Height() <= 0)
              HandleClosing(FALSE);
            else
            {
              SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Height() / m_nHeight)); 
            }
            break;
          }
          case PopDown:
          {
            rectWindow.bottom -= m_dwAnimationMoveSize;
            if (rectWindow.Height() <= 0)
              HandleClosing(FALSE);
            else
            {
              SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Height() / m_nHeight)); 
            }
            break;
          }
          case PopLeft:
          {
            rectWindow.right -= m_dwAnimationMoveSize;
            if (rectWindow.Width() <= 0)
              HandleClosing(FALSE);
            else
            {
              SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Width() / m_nWidth)); 
            }
            break;
          }
          case PopRight:
          {
            rectWindow.left += m_dwAnimationMoveSize;
            if (rectWindow.Width() <= 0)
              HandleClosing(FALSE);
            else
            {
              SetWindowPos(NULL, rectWindow.left, rectWindow.top, rectWindow.Width(), rectWindow.Height(), SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER);
              UpdateTransparency(static_cast<BYTE>(255 * rectWindow.Width() / m_nWidth)); 
            }
            break;
          }
          case NoAnimation:
          {
            //Close the window down straight away
            HandleClosing(FALSE);
            break;
          }
          default:
          {
            ASSERT(FALSE);
            break;
          }
        }
        break;
      }
      default:
      {
        ASSERT(FALSE);
        break;
      }
    }
  }
  else
  {
    //Let the base class do its thing
    CFrameWnd::OnTimer(nIDEvent);
  }
}

#if WINVER >= 0x0500
HRESULT CToasterWnd::LoadLogo(LPCTSTR pszFilename)
{
  HRESULT hr = m_Logo.Load(pszFilename);
  if (SUCCEEDED(hr))
  {
    //By default set the source and destination rects of the logo to be the same size as the bitmap
    //Derived classes can change this or you could directly change "m_rectLogoSrc" or "m_rectLogoDest"
    //after calling this function
    m_rectLogoSrc = CRect(0, 0, m_Logo.GetWidth(), m_Logo.GetHeight());
    m_rectLogoDest = m_rectLogoSrc;
  }

  return hr;
}

HRESULT CToasterWnd::LoadLogo(IStream* pStream)
{
  HRESULT hr = m_Logo.Load(pStream);
  if (SUCCEEDED(hr))
  {
    //By default set the source and destination rects of the logo to be the same size as the bitmap
    //Derived classes can change this or you could directly change "m_rectLogoSrc" or "m_rectLogoDest"
    //after calling this function
    m_rectLogoSrc = CRect(0, 0, m_Logo.GetWidth(), m_Logo.GetHeight());
    m_rectLogoDest = m_rectLogoSrc;
  }

  return hr;
}
#endif

void CToasterWnd::OnPaint() 
{
#if defined(CTOASTERWND_HTMLAYOUT_SUPPORT) || defined(CTOASTERWND_QHTM_SUPPORT)
  if (m_bHTML)
    return;
#endif

	//create the device context for painting
	CPaintDC dc(this); 

	//Get the client area
  CRect rectClient;
  GetClientRect(&rectClient);
  int nClientWidth = rectClient.Width();
  int nClientHeight = rectClient.Height();

	//Create a memory device context to implement our double buffered painting
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, nClientWidth, nClientHeight);
	CBitmap* pOldBitmap = memDC.SelectObject(&bmp);

	BOOL bDoFillSolidRect = FALSE;  

	if (m_lpfnGradientFill) //Handle the case where GradientFill is not available i.e. Windows 95
	{
		switch (m_BackgroundStyle)
		{
			case BackwardDiagonalGradient:
			{
				TRIVERTEX vert[4];
				vert[0].x = rectClient.left;
				vert[0].y = rectClient.top;
				vert[0].Red = static_cast<COLOR16>((GetRValue(m_colorBackground) - GetRValue(m_colorGradient)) * 128);
				vert[0].Green = static_cast<COLOR16>((GetGValue(m_colorBackground) - GetGValue(m_colorGradient)) * 128);
				vert[0].Blue = static_cast<COLOR16>((GetBValue(m_colorBackground) - GetBValue(m_colorGradient)) * 128);
				vert[0].Alpha = 0;

				vert[1].x = rectClient.right;
				vert[1].y = rectClient.top;
				vert[1].Red = static_cast<COLOR16>(GetRValue(m_colorBackground) * 256);
				vert[1].Green = static_cast<COLOR16>(GetGValue(m_colorBackground) * 256);
				vert[1].Blue = static_cast<COLOR16>(GetBValue(m_colorBackground) * 256);
				vert[1].Alpha = 0;

				vert[2].x = rectClient.right;
				vert[2].y = rectClient.bottom;
				vert[2].Red = vert[0].Red;
				vert[2].Green = vert[0].Green;
				vert[2].Blue = vert[0].Blue;
				vert[2].Alpha = 0;

				vert[3].x = rectClient.left;
				vert[3].y = rectClient.bottom;
				vert[3].Red = static_cast<COLOR16>(GetRValue(m_colorGradient) * 256);
				vert[3].Green = static_cast<COLOR16>(GetGValue(m_colorGradient) * 256);
				vert[3].Blue = static_cast<COLOR16>(GetBValue(m_colorGradient) * 256);
				vert[3].Alpha = 0;

				GRADIENT_TRIANGLE gt[2];
				gt[0].Vertex1 = 0;
				gt[0].Vertex2 = 1;
				gt[0].Vertex3 = 2;
				gt[1].Vertex1 = 0;
				gt[1].Vertex2 = 2;
				gt[1].Vertex3 = 3;
				m_lpfnGradientFill(memDC.GetSafeHdc(), vert, 4, &gt, 2, GRADIENT_FILL_TRIANGLE);

				break;
			}
			case ForwardDiagonalGradient:
			{
				TRIVERTEX vert[4];
				vert[0].x = rectClient.left;
				vert[0].y = rectClient.top;
				vert[0].Red = static_cast<COLOR16>((GetRValue(m_colorBackground) - GetRValue(m_colorGradient)) * 128);
				vert[0].Green = static_cast<COLOR16>((GetGValue(m_colorBackground) - GetGValue(m_colorGradient)) * 128);
				vert[0].Blue = static_cast<COLOR16>((GetBValue(m_colorBackground) - GetBValue(m_colorGradient)) * 128);
				vert[0].Alpha = 0;

				vert[1].x = rectClient.right;
				vert[1].y = rectClient.top;
				vert[1].Red = static_cast<COLOR16>(GetRValue(m_colorGradient) * 256);
				vert[1].Green = static_cast<COLOR16>(GetGValue(m_colorGradient) * 256);
				vert[1].Blue = static_cast<COLOR16>(GetBValue(m_colorGradient) * 256);
				vert[1].Alpha = 0;

				vert[2].x = rectClient.right;
				vert[2].y = rectClient.bottom;
				vert[2].Red = vert[0].Red;
				vert[2].Green = vert[0].Green;
				vert[2].Blue = vert[0].Blue;
				vert[2].Alpha = 0;

				vert[3].x = rectClient.left;
				vert[3].y = rectClient.bottom;
				vert[3].Red = static_cast<COLOR16>(GetRValue(m_colorBackground) * 256);
				vert[3].Green = static_cast<COLOR16>(GetGValue(m_colorBackground) * 256);
				vert[3].Blue = static_cast<COLOR16>(GetBValue(m_colorBackground) * 256);
				vert[3].Alpha = 0;

				GRADIENT_TRIANGLE gt[2];
				gt[0].Vertex1 = 0;
				gt[0].Vertex2 = 1;
				gt[0].Vertex3 = 2;
				gt[1].Vertex1 = 0;
				gt[1].Vertex2 = 2;
				gt[1].Vertex3 = 3;
				m_lpfnGradientFill(memDC.GetSafeHdc(), vert, 4, &gt, 2, GRADIENT_FILL_TRIANGLE);

				break;
			}
			case HorizontalGradient: //deliberate fallthrough
			case VerticalGradient:
			{
				TRIVERTEX vert[2];
				vert[0].x = rectClient.left;
				vert[0].y = rectClient.top;
				vert[0].Red = static_cast<COLOR16>(GetRValue(m_colorGradient) * 256);
				vert[0].Green = static_cast<COLOR16>(GetGValue(m_colorGradient) * 256);
				vert[0].Blue = static_cast<COLOR16>(GetBValue(m_colorGradient) * 256);
				vert[0].Alpha = 0;

				vert[1].x = rectClient.right;
				vert[1].y = rectClient.bottom;
				vert[1].Red = static_cast<COLOR16>(GetRValue(m_colorBackground) * 256);
				vert[1].Green = static_cast<COLOR16>(GetGValue(m_colorBackground) * 256);
				vert[1].Blue = static_cast<COLOR16>(GetBValue(m_colorBackground) * 256);
				vert[1].Alpha = 0;

				GRADIENT_RECT gr;
				gr.UpperLeft = 0;
				gr.LowerRight = 1;
				m_lpfnGradientFill(memDC.GetSafeHdc(), vert, 2, &gr, 1, (m_BackgroundStyle == HorizontalGradient) ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
				break;
			}
			case Solid:
			{
				bDoFillSolidRect = TRUE;
				break;
			}
			default:
			{
				ASSERT(FALSE);
				break;
			}
		}
	}
	else
		bDoFillSolidRect = TRUE;

	if (bDoFillSolidRect)
		memDC.FillSolidRect(&rectClient, m_colorBackground);

  //Draw the logo
#if WINVER >= 0x0500  
  DrawLogo(&memDC);
#endif  

  //Draw the icon
  if (m_TitleIcon)
    DrawIconEx(memDC.GetSafeHdc(), m_rectIcon.left, m_rectIcon.top, m_TitleIcon, m_rectIcon.Width(), m_rectIcon.Height(), 0, NULL, DI_NORMAL); 
  
  //Draw the close button
  DrawCloseButton(&memDC); 

  //Draw text transparently
  int nOldBkMode = memDC.SetBkMode(TRANSPARENT);

  //Draw the title text	
  if (m_sTitle.GetLength())
  { 
    CFont* pOldFont = memDC.SelectObject(&m_fontTitle);
    COLORREF nOldColor = memDC.SetTextColor(m_colorTitle);
    memDC.DrawText(m_sTitle, m_rectTitle, m_dwTitleStyle);
    memDC.SetTextColor(nOldColor);
    if (pOldFont)
      memDC.SelectObject(pOldFont);
  }

  //Draw the body text
  CFont* pOldFont = memDC.SelectObject(&m_fontText);
  COLORREF nOldColor = memDC.SetTextColor(m_colorText);
  memDC.DrawText(m_sText, m_rectBodyText, m_dwTextStyle);
  memDC.SetTextColor(nOldColor);
  if (pOldFont)
    memDC.SelectObject(pOldFont);

  memDC.SetBkMode(nOldBkMode);

	//Transfer the memory DC to the screen  
  dc.BitBlt(0, 0, nClientWidth, nClientHeight, &memDC, 0, 0, SRCCOPY);
	
	//Restore the DC	
	memDC.SelectObject(pOldBitmap);
}

BOOL CToasterWnd::OnEraseBkgnd(CDC* /*pDC*/) 
{
  //Do not do any background drawing since all our drawing is done in OnPaint
  return TRUE;
}

#if WINVER >= 0x0500
void CToasterWnd::DrawLogo(CDC* pDC)
{
  if (m_Logo.operator HBITMAP())
    m_Logo.AlphaBlend(pDC->GetSafeHdc(), m_rectLogoDest, m_rectLogoSrc, m_nLogoSrcAlpha, AC_SRC_OVER);
}
#endif

void CToasterWnd::DrawCloseButton(CDC* pDC)
{
  if (m_bUseThemingForCloseButton && VisualThemesEnabled())
    DrawThemeCloseButton(pDC);
  else
    DrawLegacyCloseButton(pDC);
}

BOOL CToasterWnd::VisualThemesEnabled()
{
  if (m_lpfnIsThemeActive)
    return m_lpfnIsThemeActive();
  else
    return FALSE;
}

void CToasterWnd::DrawThemeCloseButton(CDC* pDC)
{
  //Validate our parameters
  ASSERT(m_lpfnOpenThemeData);

  HTHEME hTheme = m_lpfnOpenThemeData(GetSafeHwnd(), L"Window");
  if (hTheme == NULL)
    DrawLegacyCloseButton(pDC);
  else
  {
    ASSERT(m_lpfnCloseThemeData);
    ASSERT(m_lpfnDrawThemeBackground);

    m_lpfnDrawThemeBackground(hTheme, pDC->GetSafeHdc(), WP_CLOSEBUTTON, m_bClosePressed ? CBS_PUSHED : (m_bCloseHot ? CBS_HOT : 0), m_rectClose, NULL);

    m_lpfnCloseThemeData(hTheme);
  }
}

void CToasterWnd::DrawLegacyCloseButton(CDC* pDC)
{
  if (m_bClosePressed)
    pDC->DrawFrameControl(&m_rectClose, DFC_CAPTION, DFCS_CAPTIONCLOSE | DFCS_PUSHED);
  else
    pDC->DrawFrameControl(&m_rectClose, DFC_CAPTION, DFCS_CAPTIONCLOSE);
}

void CToasterWnd::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
  if (m_rectClose.PtInRect(point))
  {
    m_bClosePressed = TRUE;
    InvalidateRect(&m_rectClose);
  }
  else if (m_bTitleHot && m_rectTitle.PtInRect(point))
  {
    m_bTitlePressed = TRUE;
  }
  else if (m_bTextHot && m_rectBodyText.PtInRect(point))
  {
    m_bBodyTextPressed = TRUE;
  }
  else if (m_bIconHot && m_rectIcon.PtInRect(point))
  {
    m_bIconPressed = TRUE;
  }
}

void CToasterWnd::HandleClosing(BOOL bButton)
{
  //Inform the notifier if we have one
  if (m_pNotifier)
    m_pNotifier->OnClose(this, bButton);

  //Kill the timer if running
  if (m_nTimerID)
  {
    KillTimer(m_nTimerID);
    m_nTimerID = 0;
  }

  //just hide ourselves
  ShowWindow(SW_HIDE);   
  
  m_AnimationState = None;   
  
  //Reset our transparency
  UpdateTransparency(255); 
}

void CToasterWnd::OnLButtonUp(UINT /*nFlags*/, CPoint point) 
{
  if (m_bClosePressed)
  {
    m_bClosePressed = FALSE;
    InvalidateRect(&m_rectClose);
    if (m_rectClose.PtInRect(point))
      HandleClosing(TRUE);
  }
  else if (m_bTitlePressed)
  {
    m_bTitlePressed = FALSE;
    if (m_rectTitle.PtInRect(point) && m_pNotifier)
      m_pNotifier->OnTitleTextClicked(this);
  }
  else if (m_bBodyTextPressed)
  {
    m_bBodyTextPressed = FALSE;
    if (m_rectBodyText.PtInRect(point) && m_pNotifier)
      m_pNotifier->OnBodyTextClicked(this);
  }
  else if (m_bIconPressed)
  {
    m_bIconPressed = FALSE;
    if (m_rectIcon.PtInRect(point) && m_pNotifier)
      m_pNotifier->OnIconClicked(this);
  }
}

BOOL CToasterWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
  const MSG* pMsg = GetCurrentMessage();
  CPoint pt(pMsg->pt);
  ScreenToClient(&pt);

  if (m_rectClose.PtInRect(pt) || (m_bTitleHot && m_rectTitle.PtInRect(pt)) || (m_bTextHot && m_rectBodyText.PtInRect(pt)) || (m_bIconHot && m_rectIcon.PtInRect(pt)))
  {
    SetCursor(m_cursorHand);
    return TRUE;
  }
	else
	  return CFrameWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CToasterWnd::OnClose() 
{
  //Only really close the window if the m_bSafeToClose flag
  //is set. Doing this ensures that client code will always
  //has a valid pointer to us, which they can then use to 
  //synchronously close this instance via the Close() method
  if (m_bSafeToClose)	
	  CFrameWnd::OnClose();
  else
    ShowWindow(SW_HIDE);
}

#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
BOOL CToasterWnd::CreateHTMLayoutChildControl()
{
  USES_CONVERSION;

  //create the HTMLayout control the full size of the final client area, this avoids
  //any scroll bars appearing as the toast wnd is being expanded or shrunk
  CRect rectHTML(0, 0, m_nWidth, m_nHeight);

  //Create HTMLayout control
  if (!m_wndHTMLayout.Create(HTMLayoutClassNameT(), NULL, WS_VISIBLE | WS_CHILD, rectHTML, this, TOASTERWND_HTML_ID))
  {
    TRACE(_T("CToasterWnd::OnCreate, Failed to create HTMLayout control\n"));
    return FALSE;
  }
  
  //Set the HTML of the HTMLayout control
  if (m_dwHTML && m_pbHTML)
    LoadHTML(m_pbHTML, m_dwHTML);
  else if (m_sText.GetLength())
  {
    char* pszAsciiText = T2A(const_cast<LPTSTR>(m_sText.operator LPCTSTR()));
    ::HTMLayoutLoadHtml(m_wndHTMLayout.GetSafeHwnd(), reinterpret_cast<LPCBYTE>(pszAsciiText), static_cast<UINT>(strlen(pszAsciiText)));
  }
  
  return TRUE;
}
#endif

#ifdef CTOASTERWND_QHTM_SUPPORT
BOOL CToasterWnd::CreateQHTMChildControl()
{
  //create the QHTM control the full size of the final client area, this avoids
  //any scroll bars appearing as the toast wnd is being expanded or shrunk
  CRect rectHTML(0, 0, m_nWidth, m_nHeight);

  //Create QHTM control
  if (!m_wndQHTM.Create(QHTM_CLASSNAME, m_sText, WS_VISIBLE | WS_CHILD, rectHTML, this, TOASTERWND_HTML_ID)) //If this function call fails, more that likely you forget to call QHTM_Initialize as part of your startup code
  {
    TRACE(_T("CToasterWnd::OnCreate, Failed to create QHTM control, check that you called QHTM_Initialize!\n"));
    return FALSE;
  }
  
  return TRUE;
}
#endif

int CToasterWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  USES_CONVERSION;

  //Let the base class do its thing
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

#if defined(CTOASTERWND_HTMLAYOUT_SUPPORT)
  //Hook up HTMLayout if we are doing HTML
  if (m_bHTML)
  {
    if (!CreateHTMLayoutChildControl())
      return -1;
  }
#elif defined(CTOASTERWND_QHTM_SUPPORT)
  //Hook up QHTM if we are doing HTML
  if (m_bHTML)
  {
    if (!CreateQHTMChildControl())
      return -1;
  }
#endif

  return 0;
}

void CToasterWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
  //Let the base class do its thing
	CFrameWnd::OnMouseMove(nFlags, point);

  //See if we are over the close button and if so make it "hot"
  //and force a redraw of it
  CPoint ptCursor;
  GetCursorPos(&ptCursor);
  ScreenToClient(&ptCursor);
  BOOL bNewCloseHot = m_rectClose.PtInRect(ptCursor);
  if (bNewCloseHot != m_bCloseHot)
  {
    m_bCloseHot = bNewCloseHot;
    InvalidateRect(m_rectClose);
  } 
}

#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
void CToasterWnd::OnHTMLayoutLoadData(NMHDR* nmh, LRESULT* plResult)
{
  //Allow our notifier a chance to handle the request
  if (m_pNotifier) 
  {
	  LRESULT lr = m_pNotifier->OnHTMLayoutLoadData(this, nmh);
	  if (lr) 
    {
      ASSERT(plResult);
		  *plResult = lr;
		  return;
	  }
  }

	LPNMHL_LOAD_DATA pnm = reinterpret_cast<LPNMHL_LOAD_DATA>(nmh);
  ASSERT(pnm);
  
  //To make it easier to create easy to use HTML toast windows with HTMLayout, support a "RES:" protocol
  //similiar to the built in support in QHTM
  CString sURL(pnm->uri);
  sURL.MakeUpper();
  if (sURL.Find(_T("RES:")) == 0)
  {
    ASSERT(plResult);
    *plResult = LoadResourceData(pnm);
  }
}

LRESULT CToasterWnd::LoadResourceData(LPNMHL_LOAD_DATA pnm)
{
  USES_CONVERSION;

  //Validate our parameters
  ASSERT(pnm);
  if (pnm->uri == NULL || !pnm->uri[0])
    return LOAD_DISCARD;

  //Get the extension and filename from the uri
  TCHAR szExt[_MAX_EXT];
  TCHAR szFname[_MAX_FNAME];
  CString sURL(pnm->uri);
  ASSERT(sURL.GetLength() > 4); //Should only be called with a "RES:" prefix
  sURL = sURL.Right(sURL.GetLength() - 4); //Throw away the RES: prefix
  _tsplitpath(sURL, NULL, NULL, szFname, szExt);
  
  //Convert the raw string to a CString and uppercase it and throw away any prefix period
  CString sExt(szExt);
  sExt.MakeUpper();
  if (sExt.GetAt(0) == _T('.'))
    sExt = sExt.Right(sExt.GetLength() - 1);
  
  //Try to find the specified resource type
  HINSTANCE hInstance = AfxGetResourceHandle();
  HRSRC hrsrc = 0;
  if (sExt == _T("HTML") || sExt == _T("HTM"))
    hrsrc = FindResource(hInstance, szFname, RT_HTML);
  else
    hrsrc = FindResource(hInstance, szFname, sExt);
  if (hrsrc == NULL)
    return LOAD_OK; //Resource not found, proceed with the default loader

  //Load up the resource    
  HGLOBAL hRes = ::LoadResource(hInstance, hrsrc);
  if (hRes == NULL)
    return LOAD_DISCARD;
	LPBYTE pResource = static_cast<LPBYTE>(::LockResource(hRes)); 
	if (pResource == NULL) 
	  return LOAD_DISCARD;
	DWORD dwSizeOfResource = ::SizeofResource(hInstance, hrsrc); 
	if (dwSizeOfResource == 0) 
	  return LOAD_DISCARD;

	//Report that the specified uri has its data ready
  HTMLayoutDataReady(m_wndHTMLayout.GetSafeHwnd(), pnm->uri, pResource, dwSizeOfResource);
	
	return LOAD_OK;
}

void CToasterWnd::OnHTMLayoutHyperLink(NMHL_HYPERLINK* pHyperlink)
{
  //Lets handle a few "built in commands" which synthesize the same effect as if we drew the 
  //toaster wnd in non HTML mode. The hyperlinks we support are "close", "titletext", "bodytext"
  //and "icon". This means that there is less of a need for client code to implement derived
  //versions of CToasterWnd, although you can do this if you absolutely want to!
  if (pHyperlink->action == NMHL_HYPERLINK::CLICK)
  {
	  if (wcsicmp(pHyperlink->szHREF, L"close") == 0)
	  {
		  HandleClosing(TRUE); //lets consider a hyperlink the same as a button
	  }
    else if (wcsicmp(pHyperlink->szHREF, L"titletext") == 0)
	  {
		  if (m_pNotifier)
        m_pNotifier->OnTitleTextClicked(this);
	  }
    else if (wcsicmp(pHyperlink->szHREF, L"bodytext") == 0)
	  {
		  if (m_pNotifier)
        m_pNotifier->OnBodyTextClicked(this);
	  }
    else if (wcsicmp(pHyperlink->szHREF, L"icon") == 0)
	  {
		  if (m_pNotifier)
        m_pNotifier->OnIconClicked(this);
	  }
    else
    {
      //Give the notifier a crack at the hyperlink
      if (m_pNotifier)
        m_pNotifier->OnHTMLayoutHyperLink(this, pHyperlink);
    }
  }
  else if (m_pNotifier)
  {
    //Give the notifier a crack at the hyperlink
    m_pNotifier->OnHTMLayoutHyperLink(this, pHyperlink);
  }
}

void CToasterWnd::OnHTMLayoutHtmlCommand(NMHL_COMMAND_CLICK* pCommand)
{
  //Lets handle a few "built in commands" which synthesize the same effect as if we drew the 
  //toaster wnd in non HTML mode. The hyperlinks we support are "close", "titletext", "bodytext"
  //and "icon". This means that there is less of a need for client code to implement derived
  //versions of CToasterWnd, although you can do this if you absolutely want to!
  if (wcsicmp(pCommand->szElementID, L"close") == 0)
  {
	  HandleClosing(TRUE); //lets consider a hyperlink the same as a button
  }
  else if (wcsicmp(pCommand->szElementID, L"titletext") == 0)
  {
	  if (m_pNotifier)
      m_pNotifier->OnTitleTextClicked(this);
  }
  else if (wcsicmp(pCommand->szElementID, L"bodytext") == 0)
  {
	  if (m_pNotifier)
      m_pNotifier->OnBodyTextClicked(this);
  }
  else if (wcsicmp(pCommand->szElementID, L"icon") == 0)
  {
	  if (m_pNotifier)
      m_pNotifier->OnIconClicked(this);
  }
  else
  {
    //Give the notifier a crack at the command
    if (m_pNotifier)
      m_pNotifier->OnHTMLayoutCommand(this, pCommand);
  }
}

void CToasterWnd::OnHTMLayoutAttachBehavior(NMHDR* nmh, LRESULT* plResult)
{
	NMHL_ATTACH_BEHAVIOR* pnm = reinterpret_cast<NMHL_ATTACH_BEHAVIOR*>(nmh);

  //attach custom behaviors
  htmlayout::event_handler* pEventHandler = htmlayout::behavior::find(pnm->behaviorName, pnm->element);
  if (pEventHandler)
  {
    pnm->elementTag  = pEventHandler;
    pnm->elementProc = htmlayout::behavior::element_proc;
    pnm->elementEvents = pEventHandler->subscribed_to;
  }
 
  *plResult = 0;
}
#endif

//To make it easier for derived classes to handle QHTM notifications, lets map the notification message to a 
//virtual function in the notifier
#ifdef CTOASTERWND_QHTM_SUPPORT
void CToasterWnd::OnQHTMHyperlink(NMHDR* nmh, LRESULT* /*plResult*/)
{
	LPNMQHTM pnm = reinterpret_cast<LPNMQHTM>(nmh);
  AFXASSUME(pnm);

  //By default allow QHTM to handle the hyperlink
  pnm->resReturnValue = TRUE;

  //Lets handle a few "built in commands" which synthesize the same effect as if we drew the 
  //toaster wnd in non HTML mode. The hyperlinks we support are "close", "titletext", "bodytext"
  //and "icon". This means that there is less of a need for client code to implement derived
  //versions of CToasterWnd, although you can do this if you absolutely want to!
	if (_tcsicmp(pnm->pcszLinkText, _T("close")) == 0)
	{
		HandleClosing(TRUE); //lets consider a hyperlink the same as a button
    pnm->resReturnValue = FALSE; //We've handled the link, so tell QHTM not to do anything
	}
  else if (_tcsicmp(pnm->pcszLinkText, _T("titletext")) == 0)
	{
		if (m_pNotifier)
      m_pNotifier->OnTitleTextClicked(this);
    pnm->resReturnValue = FALSE; //We've handled the link, so tell QHTM not to do anything
	}
  else if (_tcsicmp(pnm->pcszLinkText, _T("bodytext")) == 0)
	{
		if (m_pNotifier)
      m_pNotifier->OnBodyTextClicked(this);
    pnm->resReturnValue = FALSE; //We've handled the link, so tell QHTM not to do anything
	}
  else if (_tcsicmp(pnm->pcszLinkText, _T("icon")) == 0)
	{
		if (m_pNotifier)
      m_pNotifier->OnIconClicked(this);
    pnm->resReturnValue = FALSE; //We've handled the link, so tell QHTM not to do anything
	}
  else
  {
    //Give the notifier a crack at the hyperlink
    if (m_pNotifier)
      pnm->resReturnValue = m_pNotifier->OnQHTMHyperLink(this, pnm->pcszLinkText, pnm->pcszLinkID);
  }
}
#endif
