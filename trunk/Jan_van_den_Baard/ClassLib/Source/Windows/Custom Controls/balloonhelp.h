// ******************************************************************************
// BalloonHelp.cpp : header file
// Copyright 2001-2002, Joshua Heyer
//  You are free to use this code for whatever you want, provided you
// give credit where credit is due.  (I seem to get a lot of questions
// about that statement...  All i mean is, don't copy huge bits of code
// and then claim you wrote it.  You don't have to put my name in an about
// box or anything.  Though i'm not going to stop you if that's really what
// you want :~) )
//  I'm providing this code in the hope that it is useful to someone, as i have
// gotten much use out of other peoples code over the years.
//  If you see value in it, make some improvements, etc., i would appreciate it
// if you sent me some feedback.
//
// ******************************************************************************
//
// Implementation for this class-library by Jan van den Baard.
//
// 16-Aug-2002 (JBa) - Updated to Release #3
// 30-Dec-2001 (JBa) - Added the m_hTheme private member.
// 11-Dec-2001 (JBa) - Added the m_Body private member.
// 15-Dec-2001 (JBa) - Added unCLOSE_ON_ACTIVATE

#ifndef _BALLOON_HELP_H_INCLUDED_
#define _BALLOON_HELP_H_INCLUDED_

#include "../window.h"
#include "../common controls/imagelist.h"
#include "../../strings/string.h"
#include "../../collectors/array.h"
#include "../../threads/criticalsection.h"
#include "../../tools/theming.h"
#include "infocontrol.h"

// This was introduced to me by Maximilian Hänel in his WTL port.  Cool, eh?
////////////////////////////////////////////////////////////////////////////////
// The class _ThunkImpl is a renamed version of Andrew Nosenko CAuxThunk implementation.
// Thanks Andrew, it's a fantastic class!
//
// Copyright (c) 1997-2001 by Andrew Nosenko <andien@nozillium.com>,
// (c) 1997-1998 by Computer Multimedia System, Inc.,
// (c) 1998-2001 by Mead & Co Limited
//
// http://www.nozillium.com/atlaux/
// Version: 1.10.0021
//
#ifndef _M_IX86
	#pragma message("_ThunkImpl/ is implemented for X86 only!")
#endif

#pragma pack(push, 1)

template <class T>
class _ThunkImpl
{
private:
	BYTE	m_mov;			// mov ecx, %pThis
	DWORD	m_this; 		//
	BYTE	m_jmp;			// jmp func
	DWORD	m_relproc;		// relative jmp

protected:
	typedef void ( T::*TMFP )();
	void InitThunk( TMFP method, const T* pThis )
	{
		union { DWORD func; TMFP method; } addr;
		addr.method = ( TMFP )method;
		m_mov  = 0xB9;
		m_this = ( DWORD )pThis;
		m_jmp  = 0xE9;
		m_relproc = addr.func - ( DWORD )( this + 1 );

		::FlushInstructionCache( GetCurrentProcess(), this, sizeof( *this ));
	}
	FARPROC GetThunk() const
	{
		_ASSERT( m_mov == 0xB9 );
		return ( FARPROC )this;
	}
};
#pragma pack(pop) // _ThunkImpl

// we need these three dummy classes so we can
// derive more than once from _ThunkImpl
template <class T>
class BHMouseHookThunk: public _ThunkImpl<T> {};

template <class T>
class BHKeybHookThunk: public _ThunkImpl<T> {};

template <class T>
class BHCallWndRetHookThunk: public _ThunkImpl<T> {};

class ClsBalloonHelp : public ClsWindow,
                       public BHKeybHookThunk<ClsBalloonHelp>,
                       public BHMouseHookThunk<ClsBalloonHelp>,
                       public BHCallWndRetHookThunk<ClsBalloonHelp>
{
	_NO_COPY( ClsBalloonHelp );
public:
	// Construction/destruction.
	ClsBalloonHelp();
	virtual ~ClsBalloonHelp();

	// options
	static const unsigned int unCLOSE_ON_ACTIVATE;		// closes window on activation.
	static const unsigned int unCLOSE_ON_LBUTTON_UP;	// closes window on WM_LBUTTON_UP
	static const unsigned int unCLOSE_ON_MBUTTON_UP;	// closes window on WM_MBUTTON_UP
	static const unsigned int unCLOSE_ON_RBUTTON_UP;	// closes window on WM_RBUTTON_UP
	static const unsigned int unCLOSE_ON_LBUTTON_DOWN;	// closes window on WM_LBUTTON_DOWN
	static const unsigned int unCLOSE_ON_MBUTTON_DOWN;	// closes window on WM_MBUTTON_DOWN
	static const unsigned int unCLOSE_ON_RBUTTON_DOWN;	// closes window on WM_RBUTTON_DOWN
	static const unsigned int unCLOSE_ON_MOUSE_MOVE;	// closes window when user moves mouse past threshhold
	static const unsigned int unCLOSE_ON_KEYPRESS;		// closes window on the next keypress message sent to this thread.
	static const unsigned int unCLOSE_ON_ANYTHING;		// all of the above
	static const unsigned int unDELAY_CLOSE;		// when a user action triggers the close, begins timer.  closes when timer expires.
	static const unsigned int unDELETE_THIS_ON_CLOSE;	// deletes object when window is closed.  Used by LaunchBalloon(), use with care
	static const unsigned int unSHOW_CLOSE_BUTTON;		// shows close button in upper right
	static const unsigned int unSHOW_INNER_SHADOW;		// draw inner shadow in balloon
	static const unsigned int unSHOW_TOPMOST;		// place balloon above all other windows
	static const unsigned int unDISABLE_XP_SHADOW;		// disable Windows XP's drop-shadow effect (overrides system and user settings)
	static const unsigned int unDISABLE_FADEIN;		// disable the fade-in effect (overrides system and user settings)
	static const unsigned int unDISABLE_FADEOUT;		// disable the fade-out effect (overrides system and user settings)
	static const unsigned int unDISABLE_FADE;		// disable the fade-in/fade-out effects (overrides system and user settings)

	BOOL Create( const ClsString& strTitle,			// title of balloon
		     const ClsString& strContent,		// content of balloon
		     const ClsPoint& ptAnchor,			// anchor (tail position) of balloon
		     unsigned int unOptions,			// options (see above)
		     ClsWindow* pParentWnd = NULL,		// parent window (NULL == MFC main window)
		     const ClsString strURL = _T( "" ),		// URL to open (ShellExecute()) when clicked
		     unsigned int unTimeout = 0,		// delay before closing automatically (milliseconds)
		     HICON hIcon = NULL );			// icon to display

	// Show a help balloon on screen.
	static void LaunchBalloon( const ClsString& strTitle, const ClsString& strContent,
				   const ClsPoint& ptAnchor,
				   LPCTSTR szIcon = IDI_EXCLAMATION,
				   unsigned int unOptions = unSHOW_CLOSE_BUTTON,
				   ClsWindow* pParentWnd = NULL,
				   const ClsString strURL = _T( "" ),
				   unsigned int unTimeout = 10000 );

	// Sets the font used for drawing the balloon title.  Deleted by balloon, do not use CFont* after passing to this function.
	void SetTitleFont(ClsFont* pFont);
	// Sets the font used for drawing the balloon content.  Deleted by balloon, do not use CFont* after passing to this function.
	void SetContentFont(ClsFont* pFont);
	// Sets the icon displayed in the top left of the balloon (pass NULL to hide icon)
	void SetIcon(HICON hIcon);
	// Sets the icon displayed in the top left of the balloon (pass NULL to hide icon)
	void SetIconScaled(HICON hIcon, int cx, int cy);
	// Sets the icon displayed in the top left of the balloon (pass NULL hBitmap to hide icon)
	void SetIcon(HBITMAP hBitmap, COLORREF crMask);
	// Sets the icon displayed in the top left of the balloon
	void SetIcon(HBITMAP hBitmap, HBITMAP hMask);
	// Set icon displayed in the top left of the balloon to image # nIconIndex from pImageList
	void SetIcon(ClsImageList* pImageList, int nIconIndex);
	// Sets the URL to be opened when balloon is clicked.  Pass "" to disable.
	void SetURL(const ClsString& strURL);
	// Sets the number of milliseconds the balloon can remain open.  Set to 0 to disable timeout.
	void SetTimeout(unsigned int unTimeout);
	// Sets the distance the mouse must move before the balloon closes when the unCLOSE_ON_MOUSE_MOVE option is set.
	void SetMouseMoveTolerance(int nTolerance);
	// Sets the point to which the balloon is "anchored"
	void SetAnchorPoint(ClsPoint ptAnchor, ClsWindow* pWndAnchor = NULL);
	// Sets the title of the balloon
	void SetTitle(const ClsString& strTitle);
	// Sets the content of the balloon (plain text only)
	void SetContent(const ClsString& strContent);
	// Sets the background color of the balloon
	void SetBackgroundColor(COLORREF crBackground);
protected:
	// layout constants
	static const int nTIP_TAIL;
	static const int nTIP_MARGIN;

	// calculate anchor position (adjust for client coordinates if used)
	ClsPoint GetAnchorPoint();

	// determine bounds of screen anchor is on (Multi-Monitor compatibility)
	void GetAnchorScreenBounds(ClsRect& rect);

	// determine section of the screen balloon is on
	enum BALLOON_QUADRANT { BQ_TOPRIGHT, BQ_TOPLEFT, BQ_BOTTOMRIGHT, BQ_BOTTOMLEFT };
	BALLOON_QUADRANT GetBalloonQuadrant();

	// Draw the non-client area
	virtual void DrawNonClientArea(ClsDC* pDC);
	// Draw the client area
	virtual void DrawClientArea(ClsDC* pDC);
	// Calculate the dimensions and draw the balloon header
	virtual ClsSize DrawHeader(ClsDC* pDC, bool bDraw = TRUE);
	// Calculate the dimensions required to draw the balloon header
	ClsSize CalcHeaderSize(ClsDC* pDC) { return DrawHeader(pDC, FALSE); }
	// Calculate the dimensions required to draw the balloon content
	ClsSize CalcContentSize(ClsDC* pDC) { ClsSize sz( m_Body.TotalWidth( pDC, m_strContent ), m_Body.TotalHeight( pDC, m_strContent )); return sz; }
	// Calculate the total size needed by the balloon window
	ClsSize CalcWindowSize();
	// Calculate the total size needed by the client area of the balloon window
	ClsSize CalcClientSize();
	// Size and position the balloon window on the screen.
	void PositionWindow();

	// Displays the balloon on the screen, performing fade-in if enabled.
	void ShowBalloon(void);
	// Removes the balloon from the screen, performing the fade-out if enabled
	void HideBalloon();
	// Get close button width
	inline static int GetCloseButtonWidth() { return ::GetSystemMetrics( ThemingAPI.IsThemingEnabled() ? SM_CXSMSIZE : SM_CXSIZE ); }
	// Get close button height
	inline static int GetCloseButtonHeight() { return ::GetSystemMetrics( ThemingAPI.IsThemingEnabled() ? SM_CYSMSIZE : SM_CYSIZE ); }

	// message handlers
	virtual LRESULT OnEraseBkgnd( ClsDC* pDC );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT WindowProc( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void PostNcDestroy();

private:
	// Keyboard hook
	void SetKeyboardHook();
	void RemoveKeyboardHook();
	// Mouse hook
	void SetMouseHook();
	void RemoveMouseHook();
	// Call Window Return hook
	void SetCallWndRetHook();
	void RemoveCallWndRetHook();

	// Keyboard hook callback
	LRESULT KeyboardHookProc( int code, WPARAM wParam, LPARAM lParam);
	// Mouse hook callback
	LRESULT MouseHookProc(int code, WPARAM wParam, LPARAM lParam);
	// Call Window Return hook callback (automatic following)
	LRESULT CallWndRetProc(int code, WPARAM wParam, LPARAM lParam);

	// hook handles, if set
	HHOOK           m_hKeyboardHook;
	HHOOK           m_hMouseHook;
	HHOOK           m_hCallWndRetHook;

	ClsInfoControl	m_Body;		  // Content control.
	HANDLE		m_hTheme;	  // Theme handle.

	unsigned int	m_unOptions;
	unsigned int	m_unTimeout;      // max time to show, in milliseconds
	unsigned int	m_unTimerClose;   // ID of kill timer
	ClsString	m_strURL;         // url to open, if clicked.
	ClsPoint	m_ptAnchor;       // "anchor" (point of tail)
	HICON		m_hIcon;          // icon (if not NULL)
	ClsString	m_strContent;     // text to show in content area
	HWND		m_hwndAnchor;     // window to anchor to (can be NULL for desktop anchor)
	ClsImageList	m_ilIcon;         // icon

	ClsFont	       *m_pTitleFont;     // font to use for title
	ClsFont	       *m_pContentFont;   // font to use for content

	ClsRect		m_screenRect;     // bounds of screen anchor is on
	ClsRgn		m_rgnComplete;    // Clipping / Drawing region
	ClsPoint	m_ptMouseOrig;    // original mouse position; for hiding on mouse move
	UINT		m_uCloseState;    // current state of the close button
	int		m_nMouseMoveTolerance;  // distance mouse has to move before balloon will close.

	COLORREF	m_crBackground;   // Background color for balloon
};

#endif // _BALLOON_HELP_H_INCLUDED_