#ifndef _COLORPOPUP_H_
#define _COLORPOPUP_H_

// ColorPopup.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.
//
// Converted for this class library and some aditional coding by Jan van den Baard

#include "../window.h"
#include "../common controls/tooltip.h"
#include "../../gdi/dc.h"
#include "../../coords/point.h"
#include "../../tools/xpcolors.h"

// ClsColorPopup messages
#define CPN_SELCHANGE        WM_USER + 1001        // Color Picker Selection change
#define CPN_DROPDOWN         WM_USER + 1002        // Color Picker drop down
#define CPN_CLOSEUP          WM_USER + 1003        // Color Picker close up
#define CPN_SELENDOK         WM_USER + 1004        // Color Picker end OK
#define CPN_SELENDCANCEL     WM_USER + 1005        // Color Picker end (cancelled)

// To hold the colors and their names
typedef struct 
{
	COLORREF  crColor;
	TCHAR    *szName;
} ColorTableEntry;
#define RGB_END	0xFFFFFFFF // Table terminator. Set as crColor.

// ColorPopup class.
class ClsColorPopup : public ClsWindow
{
	_NO_COPY( ClsColorPopup );
public:
	// Construction/destruction.
	ClsColorPopup();
	ClsColorPopup( ClsPoint p, COLORREF crColor, ClsWindow* pParentWnd, UINT nID,
		       LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL, ColorTableEntry *pCustomColors = NULL, BOOL bFullOpen = TRUE, BOOL bRenderXP = FALSE );
	virtual ~ClsColorPopup();

	// Implementation.
	void Initialise( ColorTableEntry *pCustomColors = NULL );
	BOOL Create( ClsPoint p, COLORREF crColor, ClsWindow* pParentWnd, UINT nID,
		     LPCTSTR szDefaultText = NULL, LPCTSTR szCustomText = NULL, BOOL bFullOpen = TRUE, BOOL bRenderXP = FALSE );

protected:
	// Overidables.
	virtual BOOL PreTranslateMessage( LPMSG pMsg );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void PostNcDestroy();
	
	// Helpers.
	void Paint( ClsDC *pDC );
	BOOL GetCellRect( int nIndex, const LPRECT& rect );
	void FindCellFromColor( COLORREF crColor );
	void SetWindowSize();
	void CreateToolTips();
	void ChangeSelection( int nIndex );
	void EndSelection( int nMessage );
	void DrawCell( ClsDC* pDC, int nIndex );
	void DrawCellBackground( ClsDC *pDC, int nIndex, LPCRECT pRect );
	inline COLORREF GetColor( int nIndex )	   { return m_pcrColors[ nIndex ].crColor; }
	inline LPCTSTR GetColorName( int nIndex ) { return m_pcrColors[ nIndex ].szName; }
	int  GetIndex( int row, int col ) const;
	int  GetRow( int nIndex ) const;
	int  GetColumn( int nIndex ) const;

	// Data.
	ColorTableEntry	*m_pcrColors;
	int		 m_nNumColors;
	int		 m_nNumColumns, m_nNumRows;
	int		 m_nBoxSize, m_nMargin;
	int		 m_nCurrentSel;
	int		 m_nChosenColorSel;
	ClsString	 m_strDefaultText;
	ClsString	 m_strCustomText;
	ClsRect		 m_CustomTextRect, m_DefaultTextRect, m_WindowRect;
	ClsFont		 m_Font;
	ClsPalette	 m_Palette;
	COLORREF	 m_crInitialColor, m_crColor;
	ClsTooltip	 m_ToolTip;
	ClsWindow*	 m_pParent;
	BOOL		 m_bFullOpen;
	HANDLE		 m_hTheme;
	HWND		 m_hOpenPopup;
	BOOL		 m_bFadeOut;
	BOOL		 m_bCustomOpen;
	BOOL		 m_bRenderXP;
	BOOL		 m_bFade;
	BOOL		 m_bAnimate;
	UINT		 m_nID;
};

#endif // _COLORPOPUP_H_