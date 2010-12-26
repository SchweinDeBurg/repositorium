/*
Module : TOASTERWND.H
Purpose: Defines the interface for an MFC class which implements a MSN Messenger style notification window 
         aka a "Toast" window, since it pops up!. Based in part on a C# implementation of a similiar type
         window by Robert Misiak on codeproject (http://www.codeproject.com/cs/miscctrl/RobMisNotifyWindow.asp)
Created: PJN / 22-04-2005

Copyright (c) 2005 - 2008 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////////////////// Macros / Defines /////////////////////////////////

#pragma once

#ifndef __TOASTERWND_H__
#define __TOASTERWND_H__

#ifndef CTOASTERWND_EXT_CLASS
#define CTOASTERWND_EXT_CLASS
#endif


////////////////////////////// Includes ///////////////////////////////////////

#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
#ifndef __htmlayout_h__
#include <htmlayout.h> //If you get a compilation error on this line, then you need to download, install and purchase a license for HTMLayout (http://www.terrainformatica.com/htmlayout/)
#endif
#endif

#if WINVER >= 0x0500
#ifndef __ATLIMAGE_H__
#pragma message("To avoid this message, please put atlimage.h in your pre compiled header (normally stdafx.h)")
#include <atlimage.h> //CImage support
#endif
#endif


////////////////////////////// Classes ////////////////////////////////////////

//Forward declaration
class CToasterWnd;

//The interface class which clients should derive from if 
//they want to handle notifications from CToasterWnd
class CTOASTERWND_EXT_CLASS CToasterNotification
{
public:
//virtual methods
  virtual void    OnBodyTextClicked(CToasterWnd* pFrom);                                         //called when the user clicks the body text
  virtual void    OnTitleTextClicked(CToasterWnd* pFrom);                                        //called when the user clicks the title text
  virtual void    OnIconClicked(CToasterWnd* pFrom);                                             //called when the user clicks the title text
  virtual void    OnClose(CToasterWnd* pFrom, BOOL bButton);                                     //Called when the notification window is closing
  virtual void    OnStateChange(CToasterWnd* pFrom);                                             //Called when the animation state changes
#ifdef CTOASTERWND_QHTM_SUPPORT  
  virtual LRESULT OnQHTMHyperLink(CToasterWnd* pFrom, LPCTSTR pcszLinkText, LPCTSTR pcszLinkID); //Called when a QHTM hyperlink is clicked
#endif  
#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
  virtual void OnHTMLayoutHyperLink(CToasterWnd* pFrom, NMHL_HYPERLINK* pHyperlink);             //Called when a HTMLayout hyperlink notification occurs
  virtual void OnHTMLayoutCommand(CToasterWnd* pFrom, NMHL_COMMAND_CLICK* pCommand);             //Called when a HTMLayout command notification occurs
  virtual LRESULT OnHTMLayoutLoadData(CToasterWnd* pFrom, NMHDR* pnmh);                          //Called to implement support for HLN_LOAD_DATA message
#endif  

  virtual CRect   CalculateInitialPopupPosition(CToasterWnd* pFrom);                             //Called to calculate the initial popup position and animation style 
};

#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
//A basic CWnd helper class which forwards HTMLayout behavior notifications 
//on to main toaster window
class CTOASTERWND_EXT_CLASS CHtmlLayoutReflectorWnd : public CWnd
{
public:
//Constructors / Destructors
  CHtmlLayoutReflectorWnd() : m_pReflectorWnd(NULL) {};
  
//Member variables
  CToasterWnd* m_pReflectorWnd;  
  
protected:
	afx_msg LRESULT OnBehaviorNotify(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
};
#endif

//The class which encapsulates the "Toaster" notification window
class CTOASTERWND_EXT_CLASS CToasterWnd : public CFrameWnd
{
public:
//enums
  enum BackgroundStyle
  {
    BackwardDiagonalGradient = 0,
    ForwardDiagonalGradient  = 1,
    HorizontalGradient       = 2,
    VerticalGradient         = 3,
    Solid                    = 4
  };

  enum AnimationStyle
  {
    AutoSet     = 0,
    PopUp       = 1,
    PopDown     = 2,
    PopLeft     = 3,
    PopRight    = 4,
    NoAnimation = 5
  };

  enum PopupPosition
  {
    AutoDetect  = 0,
    BottomRight = 1,
    BottomLeft  = 2,
    TopRight    = 3,
    TopLeft     = 4
  };

  enum AnimationState
  {
    Opening = 0,
    Closing = 1,
    Showing = 2,
    None    = 3
  };

//Constructors / Destructors
  CToasterWnd();
  CToasterWnd(const CString& sTitle, const CString& sText);
  CToasterWnd(const CString& sText);
  virtual ~CToasterWnd();

//Methods
  virtual BOOL    Show();                            //Show (aka create and popup) the notification window
  virtual BOOL    Create();                          //Create the notification window (but do not show yet)
  virtual void    Popup();                           //Popup the notification window (assumes it has already been created)  
  void            Hide();                            //Hides the notification window (with closing animation)
  void            Close();                           //Actually destroys the window
  virtual CRect   CalculateInitialPopupPosition();   //Calculates popup position
#if WINVER >= 0x0500
  virtual HRESULT LoadLogo(LPCTSTR pszFilename);     //Load up the logo from a file
  virtual HRESULT LoadLogo(IStream* pStream);        //Load up the logo from a COM stream
#endif  

//Accessors / Mutators
  AnimationState GetAnimationState() const          //Returns the "state" of the notification window 
  { 
    return m_AnimationState; 
  }
                                      
//Member variables   
#if WINVER >= 0x0500
  CImage                m_Logo;                      //The image which will be displayed in the notification window 
  CRect                 m_rectLogoSrc;               //The source rect of the logo
  CRect                 m_rectLogoDest;              //The destination rect of the logo
  BYTE                  m_nLogoSrcAlpha;             //The bSrcAlpha value to pass to AlphaBlend
#endif
  CString               m_sTitle;                    //The title text to display in the notification window
  CFont                 m_fontTitle;                 //The font to use for title text
  COLORREF              m_colorTitle;                //The color to use for the title text
  DWORD                 m_dwTitleStyle;              //The flags to use in the call to DrawText for the title text
  BOOL                  m_bTitleHot;                 //Is the title text clickable (and produces the hand cursor)
  CString               m_sText;                     //The body text to display in the notification window
  CFont                 m_fontText;                  //The font to use for body text 
  COLORREF              m_colorText;                 //The color to use for the body text
  DWORD                 m_dwTextStyle;               //The flags to use in the call to DrawText for the body text
  BOOL                  m_bTextHot;                  //Is the body text clickable (and produces the hand cursor)
  COLORREF              m_colorBackground;           //The color to use for the background
  COLORREF              m_colorGradient;             //The color to use for the gradient on the background
  DWORD                 m_dwWaitTime;                //The time in milliseconds which the notification should be shown for
  int                   m_nHeight;                   //The height to use for the notification window
  int                   m_nWidth;                    //The width to use for the notification window
  BackgroundStyle       m_BackgroundStyle;           //The style to use for the client area of the notification window
  BOOL                  m_bWaitOnMouseOver;          //specifies whether or not the notification window should continue to be displayed it the mouse moves inside the client area
  HICON                 m_TitleIcon;                 //The icon to display just before the Title text
  BOOL                  m_bIconHot;                  //Is the icon clickable (and produces the hand cursor)
  AnimationStyle        m_AnimationStyle;            //How the window should animate
  PopupPosition         m_PopupPosition;             //Where should the notification window be shown at on the primary display
  int                   m_nCXAreaEdge;               //The horizontal spacing in pixels between the boundary of the notification window and the edge of the working area
  int                   m_nCYAreaEdge;               //The vertical spacing in pixels between the boundary of the notification window and the edge of the working area
  BOOL                  m_bUseThemingForCloseButton; //Should the close button use XP theming?
  CToasterNotification* m_pNotifier;                 //Where notifications will be sent to
  BOOL                  m_bTransparent;              //Should the animation also use transparency
#if defined(CTOASTERWND_HTMLAYOUT_SUPPORT) || defined(CTOASTERWND_QHTM_SUPPORT)
  BOOL                  m_bHTML;                     //Should we create a HTMLayout child control instead of doing all our own custom drawing
#endif  
#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
  CHtmlLayoutReflectorWnd m_wndHTMLayout;            //The actual HTMLayout child control
#endif
#ifdef CTOASTERWND_QHTM_SUPPORT
  CWnd                  m_wndQHTM;                   //The actual QHTM child control
#endif          

#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT	  
  DWORD                 m_dwHTML;                    //Use this value in conjuction with m_pbHTML if you would like to use a raw blob as the HTML rather than using "m_sText"
  LPCBYTE               m_pbHTML;                    //The raw html text to use

//Virtual methods used for HTMLayout reflector notifications
  virtual void OnHTMLayoutHyperLink(NMHL_HYPERLINK* pHyperlink);
  virtual void OnHTMLayoutHtmlCommand(NMHL_COMMAND_CLICK* pCommand);
  
  //Load html into the HTMLayout control from a memory buffer
  BOOL LoadHTML(LPCBYTE pbHTML, DWORD nHTMLSize)
  { 
    //Validate our parameters
    ASSERT(m_bHTML);
    ASSERT(::IsWindow(m_wndHTMLayout.m_hWnd));
    
    return ::HTMLayoutLoadHtml(m_wndHTMLayout.m_hWnd, pbHTML, nHTMLSize);
  }
  
  //Load html into the HTMLayout control from a URL
  BOOL LoadUrl(LPCWSTR url)
  {
    //Validate our parameters
    ASSERT(m_bHTML);
    ASSERT(::IsWindow(m_wndHTMLayout.m_hWnd));
    
    return ::HTMLayoutLoadFile(m_wndHTMLayout.m_hWnd, url);
  }
#endif  

protected:
//Methods
  void Construct();
  virtual void DrawCloseButton(CDC* pDC);
  virtual BOOL VisualThemesEnabled();
  virtual void DrawThemeCloseButton(CDC* pDC);
  virtual void DrawLegacyCloseButton(CDC* pDC);
  virtual void CreateFonts();
#if WINVER >= 0x0500  
  virtual void DrawLogo(CDC* pDC);
#endif  
  virtual void HandleClosing(BOOL bButton);
  void UpdateTransparency(BYTE byAlpha);
#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT	  
  virtual BOOL CreateHTMLayoutChildControl();
  virtual LRESULT LoadResourceData(LPNMHL_LOAD_DATA pnm);
#endif  
#ifdef CTOASTERWND_QHTM_SUPPORT
  virtual BOOL CreateQHTMChildControl();
#endif

	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
#ifdef CTOASTERWND_HTMLAYOUT_SUPPORT
	afx_msg void OnHTMLayoutLoadData(NMHDR* nmh, LRESULT* plResult);
	afx_msg void OnHTMLayoutHyperlink(NMHDR* nmh, LRESULT* plResult);
	afx_msg void OnHTMLayoutAttachBehavior(NMHDR* nmh, LRESULT* plResult);
#endif	
#ifdef CTOASTERWND_QHTM_SUPPORT
  afx_msg void OnQHTMHyperlink(NMHDR* nmh, LRESULT* plResult);
#endif

	DECLARE_MESSAGE_MAP()

  UINT_PTR              m_nTimerID;                 //The timer ID we use for the animation and shutdown
  CRect                 m_rectScreen;               //The work area of the primary display
  CRect                 m_rectIcon;                 //Client window location of the icon
  CRect                 m_rectClose;                //Client window location of the close button
  CRect                 m_rectTitle;                //Client window location of the title text
  CRect                 m_rectBodyText;             //Client window location of the body text
  AnimationState        m_AnimationState;           //The animation state of the notification window
  BOOL                  m_bClosePressed;            //Has the closed button been pressed
  BOOL                  m_bCloseHot;                //Is this close button "hot"
  BOOL                  m_bTitlePressed;            //Has the title text been pressed
  BOOL                  m_bIconPressed;             //Has the icon been pressed
  BOOL                  m_bBodyTextPressed;         //Has the body text been pressed
  HCURSOR               m_cursorHand;               //The hand cursor
  AnimationStyle        m_actualAnimationStyle;     //The actual animation style (based on m_AnimationStyle)
  PopupPosition         m_actualPopupPosition;      //The actual position to use (based on m_PopupPosition)
  BOOL                  m_bSafeToClose;             //Set when we want to destroy the window as opposed to just hiding it
  CRect                 m_rectInitialPos;           //The initial creation position
  DWORD                 m_dwTimerInterval;          //The interval in milliseconds for the animation timer
  DWORD                 m_dwAnimationMoveSize;      //The value in pixels in which to increase or decrease the window
                                                    //size during the animation

//Windows XP Themes suport (methods and variables)
  HMODULE               m_hUXTheme;                 //UXThemes instance handle
  typedef HTHEME (WINAPI OPENTHEMEDATA)(HWND, LPCWSTR);
  typedef OPENTHEMEDATA* LPOPENTHEMEDATA;
  LPOPENTHEMEDATA        m_lpfnOpenThemeData;       //XP Themes function pointer
  typedef HRESULT (WINAPI DRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT*, const RECT*);
  typedef DRAWTHEMEBACKGROUND* LPDRAWTHEMEBACKGROUND;
  LPDRAWTHEMEBACKGROUND  m_lpfnDrawThemeBackground; //XP Themes function pointer
  typedef BOOL (WINAPI ISTHEMEACTIVE)();
  typedef ISTHEMEACTIVE* LPISTHEMEACTIVE;
  LPISTHEMEACTIVE        m_lpfnIsThemeActive;       //XP Themes function pointer
  typedef HRESULT (WINAPI CLOSETHEMEDATA)(HTHEME);
  typedef CLOSETHEMEDATA* LPCLOSETHEMEDATA;
  LPCLOSETHEMEDATA       m_lpfnCloseThemeData;      //XP Themes function pointer

//"GradientFill" support (methods and variables)
  HMODULE m_hMsimg32;                               //Msimg32.dll instance handle
  typedef BOOL (WINAPI GRADIENTFILL)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG); 
  typedef GRADIENTFILL* LPGRADIENTFILL;
  LPGRADIENTFILL m_lpfnGradientFill;                //GradientFill function pointer

//"SetLayeredWindowAttributes support (methods and variables)
  typedef BOOL (WINAPI SETLAYEREDWINDOWATTRIBUTES)(HWND, COLORREF, BYTE, DWORD);
  typedef SETLAYEREDWINDOWATTRIBUTES* LPSETLAYEREDWINDOWATTRIBUTES;
  LPSETLAYEREDWINDOWATTRIBUTES m_lpfnSetLayeredWindowAttributes;
};

#endif //__TOASTERWND_H__
