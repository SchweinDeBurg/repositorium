// ColorPopup.cpp : implementation file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Extended by Alexander Bischofberger (bischofb@informatik.tu-muenchen.de)
// Copyright (c) 1998.
//
// Converted for this class library and some aditional coding by Jan van den Baard
//
//	  7   Oct 2004 The control screwed-up when using a non-zero ID. Since the
//		       control is not a child it SetWindowLong() must be used
//		       to set the ID.
//	  13  May 2003 Now sends a CPN_SELCHANGE with the initial selection when
//		       the mouse is moved outside the popup area. (JBa)
//	  19  Aug 2002 Removed the usage of the CPCOOKIE. Replaced by a ClsWindow
//		       based solution to close the popups. (JBa)
//	  11  Jul 2002 Hide the window before destroying it in ClosePopups(). (JBa)
//        21  Jun 2002 Added shadow rendering on all OSs from Win95 to XP. (JBa)
//	  23  May 2002 Added Office XP style rendering. Note that this style
//		       overrules theming on Window XP systems. (JBa)
//	  20  Feb 2002 Added multi-monitor support. The control now gets moved 
//	               completly to the monitor on which it intersects the most. (JBa)
//	  13  Feb 2002 Clicking outside the control area will now close the
//		       control window and requeue the mouse click. (JBa)
//	   8  Jan 2002 Added slide animation when blending is not available. (JBa)
//	   3  Jan 2002 Added window fading using the AnimateWindow() API. (JBa)
//         3  Jan 2002 Added ClosePopups() to find close already opened popups. (JBa)
//	  29  Dec 2001 Added theming when theming is enabled. (JBa)
//	  10  Dec 2001 The control is now created with the "ClsDSWindowClass"
//		       class which causes the control to show a shadow on operating
//		       system which support this feature. ( JBa)
//	   4  Dec 2001 The ID is now sent with the notification messages. (JBa)
//	   1  Dec 2001 Fixed a bug in the SetWindowSize() function.
//                     It moved the popup incorrectly when it opened to far
//		       right or down on the screen. (JBa)
//         7  Aug 2001 Added the possibility to specify a custom
//		       color table. (JBa)
// Updated 30 May 1998 to allow any number of colors, and to
//                     make the appearance closer to Office 97. 
//                     Also added "Default" text area.         (CJM)
//
//         13 June 1998 Fixed change of focus bug (CJM)
//         30 June 1998 Fixed bug caused by focus bug fix (D'oh!!)
//                      Solution suggested by Paul Wilkerson.
//
// ColorPopup is a helper class for the color picker control
// ClsColorPicker. Check out the header file or the accompanying 
// HTML doc file for details.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 

#include "../../stdafx.h"

#include "colorpopup.h"
#include "../../gdi/paintdc.h"
#include "../../gdi/windowdc.h"
#include "../../gdi/getdc.h"
#include "../../gdi/selector.h"
#include "../common dialogs/colordialog.h"
#include "../../tools/theming.h"
#include "../../tools/multimonitor.h"
#include "../../tools/xpcolors.h"
#include "../../tools/drawstate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULT_BOX_VALUE	-3
#define CUSTOM_BOX_VALUE	-2
#define INVALID_COLOR		-1

#define MAX_COLORS		100

#ifndef SPI_GETMENUANIMATION
#define SPI_GETMENUANIMATION	0x1002
#endif

#ifndef SPI_GETMENUFADE
#define SPI_GETMENUFADE		0x1012
#endif

ColorTableEntry crDefaultColors[] =
{
	{ RGB(0x00, 0x00, 0x00),    _T("Black")             },
	{ RGB(0xA5, 0x2A, 0x00),    _T("Brown")             },
	{ RGB(0x00, 0x40, 0x40),    _T("Dark Olive Green")  },
	{ RGB(0x00, 0x55, 0x00),    _T("Dark Green")        },
	{ RGB(0x00, 0x00, 0x5E),    _T("Dark Teal")         },
	{ RGB(0x00, 0x00, 0x8B),    _T("Dark blue")         },
	{ RGB(0x4B, 0x00, 0x82),    _T("Indigo")            },
	{ RGB(0x28, 0x28, 0x28),    _T("Dark grey")         },

	{ RGB(0x8B, 0x00, 0x00),    _T("Dark red")          },
	{ RGB(0xFF, 0x68, 0x20),    _T("Orange")            },
	{ RGB(0x8B, 0x8B, 0x00),    _T("Dark yellow")       },
	{ RGB(0x00, 0x93, 0x00),    _T("Green")             },
	{ RGB(0x38, 0x8E, 0x8E),    _T("Teal")              },
	{ RGB(0x00, 0x00, 0xFF),    _T("Blue")              },
	{ RGB(0x7B, 0x7B, 0xC0),    _T("Blue-grey")         },
	{ RGB(0x66, 0x66, 0x66),    _T("Grey - 40")         },

	{ RGB(0xFF, 0x00, 0x00),    _T("Red")               },
	{ RGB(0xFF, 0xAD, 0x5B),    _T("Light orange")      },
	{ RGB(0x32, 0xCD, 0x32),    _T("Lime")              }, 
	{ RGB(0x3C, 0xB3, 0x71),    _T("Sea green")         },
	{ RGB(0x7F, 0xFF, 0xD4),    _T("Aqua")              },
	{ RGB(0x7D, 0x9E, 0xC0),    _T("Light blue")        },
	{ RGB(0x80, 0x00, 0x80),    _T("Violet")            },
	{ RGB(0x7F, 0x7F, 0x7F),    _T("Grey - 50")         },

	{ RGB(0xFF, 0xC0, 0xCB),    _T("Pink")              },
	{ RGB(0xFF, 0xD7, 0x00),    _T("Gold")              },
	{ RGB(0xFF, 0xFF, 0x00),    _T("Yellow")            },    
	{ RGB(0x00, 0xFF, 0x00),    _T("Bright green")      },
	{ RGB(0x40, 0xE0, 0xD0),    _T("Turquoise")         },
	{ RGB(0xC0, 0xFF, 0xFF),    _T("Skyblue")           },
	{ RGB(0x48, 0x00, 0x48),    _T("Plum")              },
	{ RGB(0xC0, 0xC0, 0xC0),    _T("Light grey")        },

	{ RGB(0xFF, 0xE4, 0xE1),    _T("Rose")              },
	{ RGB(0xD2, 0xB4, 0x8C),    _T("Tan")               },
	{ RGB(0xFF, 0xFF, 0xE0),    _T("Light yellow")      },
	{ RGB(0x98, 0xFB, 0x98),    _T("Pale green ")       },
	{ RGB(0xAF, 0xEE, 0xEE),    _T("Pale turquoise")    },
	{ RGB(0x68, 0x83, 0x8B),    _T("Pale blue")         },
	{ RGB(0xE6, 0xE6, 0xFA),    _T("Lavender")          },
	{ RGB(0xFF, 0xFF, 0xFF),    _T("White")             },
	{ RGB_END,		    NULL		    }
};

ClsColorPopup::ClsColorPopup()
{
	Initialise();
}

ClsColorPopup::ClsColorPopup( ClsPoint p, COLORREF crColor, ClsWindow* pParentWnd, UINT nID,
			      LPCTSTR szDefaultText /* = NULL */,
                              LPCTSTR szCustomText  /* = NULL */,
			      ColorTableEntry *pCustomColors /* = NULL */,
			      BOOL bFullOpen /* = TRUE */,
			      BOOL bRenderXP /* = FALSE */ )
{
	Initialise( pCustomColors );

	m_crColor        = m_crInitialColor = crColor;
	m_pParent        = pParentWnd;
	m_strDefaultText = ( szDefaultText ) ? szDefaultText : _T( "" );
	m_strCustomText  = ( szCustomText  ) ? szCustomText  : _T( "" );

	ClsColorPopup::Create( p, crColor, pParentWnd, nID, szDefaultText, szCustomText, bFullOpen, bRenderXP );
}

void ClsColorPopup::Initialise( ColorTableEntry *pCustomColors /* = NULL  */ )
{
	// Choose the default table if one is not provided.
	m_pcrColors = pCustomColors ? pCustomColors : crDefaultColors;

	// Count the colors.
	m_nNumColors = 0;
	while ( m_pcrColors[ m_nNumColors ].crColor != RGB_END ) m_nNumColors++;
	_ASSERT( m_nNumColors <= MAX_COLORS );

	if ( m_nNumColors > MAX_COLORS )
		m_nNumColors = MAX_COLORS;

	m_nNumColumns       = 0;
	m_nNumRows          = 0;
	m_nBoxSize          = 18;
	m_nMargin           = ::GetSystemMetrics( SM_CXEDGE );
	m_nCurrentSel       = INVALID_COLOR;
	m_nChosenColorSel   = INVALID_COLOR;
	m_pParent           = NULL;
	m_crColor           = m_crInitialColor = RGB( 0, 0, 0 );
	m_bFadeOut	    = TRUE;
	m_bCustomOpen	    = FALSE;

	// Idiot check: Make sure the color square is at least 5 x 5;
	if ( m_nBoxSize - 2 * m_nMargin - 2 < 5 ) m_nBoxSize = 5 + 2 * m_nMargin + 2;

	// Create the font
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof( NONCLIENTMETRICS );
	_VERIFY( SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ), &ncm, 0 ));
	m_Font.CreateFontIndirect( &( ncm.lfMessageFont ));

	// Create the palette
	struct {
		LOGPALETTE    LogPalette;
		PALETTEENTRY  PalEntry[ MAX_COLORS ];
	} pal;

	LOGPALETTE* pLogPalette = ( LOGPALETTE* ) &pal;
	pLogPalette->palVersion    = 0x300;
	pLogPalette->palNumEntries = ( WORD ) m_nNumColors; 

	for ( int i = 0; i < m_nNumColors; i++ )
	{
		pLogPalette->palPalEntry[ i ].peRed   = GetRValue( m_pcrColors[ i ].crColor );
		pLogPalette->palPalEntry[ i ].peGreen = GetGValue( m_pcrColors[ i ].crColor );
		pLogPalette->palPalEntry[ i ].peBlue  = GetBValue( m_pcrColors[ i ].crColor );
		pLogPalette->palPalEntry[ i ].peFlags = 0;
	}

	m_Palette.CreatePalette( pLogPalette );
	m_hTheme = NULL;
}

ClsColorPopup::~ClsColorPopup()
{
	m_Font.DeleteObject();
	m_Palette.DeleteObject();
	if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
}

BOOL ClsColorPopup::Create( ClsPoint p, COLORREF crColor, ClsWindow* pParentWnd, UINT nID,
                             LPCTSTR szDefaultText /* = NULL */,
                             LPCTSTR szCustomText  /* = NULL */,
			     BOOL bFullOpen /* = TRUE */,
			     BOOL bRenderXP /* = FALSE */ )
{
	_ASSERT( pParentWnd && ::IsWindow( pParentWnd->GetSafeHWND()));

	m_pParent = pParentWnd;
	m_crColor = m_crInitialColor = crColor;

	ClosePopups();

	// Determine window animation...
	m_bAnimate = FALSE;
	m_bFade = FALSE;
	::SystemParametersInfo( SPI_GETMENUANIMATION, 0, &m_bAnimate, 0 );
	if ( m_bAnimate )
		::SystemParametersInfo( SPI_GETMENUFADE, 0, &m_bFade, 0 );

	if ( ! ClsWindow::Create( WS_EX_PALETTEWINDOW, _T( "ClsDSWindowClass" ), _T( "" ), WS_POPUP, 
			          p.X(), p.Y(), 100, 100, // size updated soon
			          *pParentWnd, NULL ))
		return FALSE;
	
	// Store the ID.
	m_nID = nID;
 
	// Store CCS_FULLOPEN flag.
	m_bFullOpen = bFullOpen;

	// Store the Custom text
	if ( szCustomText != NULL ) 
		m_strCustomText = szCustomText;

	// Store the Default Area text
	if ( szDefaultText != NULL ) 
		m_strDefaultText = szDefaultText;
        
	// Set the window size
	SetWindowSize();

	// Create the tooltips
	CreateToolTips();

	// Find which cell (if any) corresponds to the initial color
	FindCellFromColor( crColor );

	// Render Office XP style?
	m_bRenderXP = bRenderXP;

	// Capture all mouse events for the life of this window
	SetCapture();
	ShowWindow( SW_SHOW );
	return TRUE;
}

// For tooltips
BOOL ClsColorPopup::PreTranslateMessage( LPMSG pMsg ) 
{
	m_ToolTip.RelayEvent( pMsg );

	// Fix (Adrian Roman): Sometimes if the picker loses focus it is never destroyed
	if ( GetCapture() != this )
		SetCapture();

	return ClsWindow::PreTranslateMessage( pMsg );
}

LRESULT ClsColorPopup::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_CREATE:
			m_bIsPopup = TRUE;
			break;

		case	WM_ACTIVATE:
			// Send this to our parent. If it is the
			// colorpicker it will be passed on to
			// it's parent.
			if ( wParam != WA_INACTIVE )
				m_pParent->SendMessage( WM_NCACTIVATE, TRUE );
			break;

		case	WM_SHOWWINDOW:
			// Hiding the window?
			if ( ! wParam && m_bFadeOut == FALSE )
				break;

			// Are we being made visible?
			if ( wParam )
				m_pParent->SendMessage( CPN_DROPDOWN );

			// Animation on?
			if ( m_bAnimate )
			{
				// Fading effect supported by the OS?
				if ( m_bFade && ClsGetApp()->GetPlatformID() == VER_PLATFORM_WIN32_NT && ClsGetApp()->GetMajorVersion() >= 5 && ClsGetApp()->IsMenuFadeEnabled())
				{
					// Blend window.
					if ( AnimateWindow( 100, wParam ? AW_BLEND : AW_HIDE | AW_BLEND ))
						return 0;
				}
				else
				{
					// Roll window.
					if ( AnimateWindow( 100, wParam ? AW_VER_POSITIVE : AW_HIDE | AW_VER_NEGATIVE ))
						return 0;
				}
			}
			break;

		case	WM_KEYDOWN:
		{
			// If an arrow key is pressed, then move the selection
			int row = GetRow( m_nCurrentSel ),
			col = GetColumn( m_nCurrentSel );
			int nChar = ( int )wParam;

			if ( nChar == VK_DOWN ) 
			{
				if ( row == DEFAULT_BOX_VALUE ) 
					row = col = 0;
				else if ( row == CUSTOM_BOX_VALUE )
				{
					if ( m_strDefaultText.GetStringLength())
						row = col = DEFAULT_BOX_VALUE;
					else
						row = col = 0;
				}
				else
				{
					row++;
					if ( GetIndex( row, col ) < 0 )
					{
						if ( m_strCustomText.GetStringLength())
							row = col = CUSTOM_BOX_VALUE;
						else if ( m_strDefaultText.GetStringLength())
							row = col = DEFAULT_BOX_VALUE;
						else
							row = col = 0;
					}
				}
				ChangeSelection( GetIndex( row, col ));
			}

			if ( nChar == VK_UP ) 
			{
				if ( row == DEFAULT_BOX_VALUE )
				{
					if ( m_strCustomText.GetStringLength())
						row = col = CUSTOM_BOX_VALUE;
					else
					{ 
						row = GetRow( m_nNumColors - 1 ); 
						col = GetColumn( m_nNumColors - 1 ); 
					}
				}
				else if ( row == CUSTOM_BOX_VALUE )
				{ 
						row = GetRow( m_nNumColors - 1 ); 
						col = GetColumn( m_nNumColors - 1 ); 
				}
				else if ( row > 0 ) row--;
				else /* row == 0 */
				{
					if ( m_strDefaultText.GetStringLength())
						row = col = DEFAULT_BOX_VALUE;
					else if ( m_strCustomText.GetStringLength())
						row = col = CUSTOM_BOX_VALUE;
					else
					{ 
						row = GetRow( m_nNumColors - 1 ); 
						col = GetColumn( m_nNumColors - 1 ); 
					}
				}
				ChangeSelection( GetIndex( row, col ));
			}

			if ( nChar == VK_RIGHT ) 
			{
				if ( row == DEFAULT_BOX_VALUE ) 
					row = col = 0; 
				else if ( row == CUSTOM_BOX_VALUE )
				{
					if ( m_strDefaultText.GetStringLength())
						row = col = DEFAULT_BOX_VALUE;
					else
						row = col = 0;
				}
				else if ( col < m_nNumColumns - 1 ) 
					col++;
				else 
				{ 
					col = 0; row++;
				}

				if ( GetIndex( row, col ) == INVALID_COLOR )
				{
					if ( m_strCustomText.GetStringLength())
						row = col = CUSTOM_BOX_VALUE;
					else if ( m_strDefaultText.GetStringLength())
						row = col = DEFAULT_BOX_VALUE;
					else
						row = col = 0;
				}
				ChangeSelection( GetIndex( row, col ));
			}

			if ( nChar == VK_LEFT ) 
			{
				if ( row == DEFAULT_BOX_VALUE )
				{
					if ( m_strCustomText.GetStringLength())
						row = col = CUSTOM_BOX_VALUE;
					else
					{ 
						row = GetRow( m_nNumColors - 1 ); 
						col = GetColumn( m_nNumColors - 1 ); 
					}
				}
				else if ( row == CUSTOM_BOX_VALUE )
				{ 
					row = GetRow( m_nNumColors - 1 ); 
					col = GetColumn( m_nNumColors - 1 ); 
				}
				else if ( col > 0 ) col--;
				else /* col == 0 */
				{
					if ( row > 0 ) { row--; col = m_nNumColumns - 1; }
					else 
					{
						if ( m_strDefaultText.GetStringLength())
							row = col = DEFAULT_BOX_VALUE;
						else if ( m_strCustomText.GetStringLength())
							row = col = CUSTOM_BOX_VALUE;
						else
						{ 
							row = GetRow( m_nNumColors - 1 ); 
							col = GetColumn( m_nNumColors - 1 ); 
						}
					}
				}
				ChangeSelection( GetIndex( row, col ));
			}

			if ( nChar == VK_ESCAPE && ! m_bCustomOpen ) 
			{
				m_crColor = m_crInitialColor;
				EndSelection( CPN_SELENDCANCEL );
				return 0;
			}

			if ( ! m_bCustomOpen && ( nChar == VK_RETURN || nChar == VK_SPACE ))
			{
				EndSelection( CPN_SELENDOK );
				return 0;
			}
			break;
		}	
		
		case	WM_MOUSEMOVE:
		{
			int nNewSelection = INVALID_COLOR;
			ClsPoint point( LOWORD( lParam ), HIWORD( lParam ));

			// Translate points to be relative raised window edge
			point.X() -= m_nMargin;
			point.Y() -= m_nMargin;

			// Is the cursor still inside the popup?
			ClsRect rc = GetClientRect();
			rc.Deflate( m_nMargin, m_nMargin );
			if ( rc.PtInRect( point ))
			{
				// check we aren't in text box
				if ( m_strCustomText.GetStringLength() && m_CustomTextRect.PtInRect( point ))
					nNewSelection = CUSTOM_BOX_VALUE;
				else if ( m_strDefaultText.GetStringLength() && m_DefaultTextRect.PtInRect( point ))
					nNewSelection = DEFAULT_BOX_VALUE;
				else
				{
					// Take into account text box
					if ( m_strDefaultText.GetStringLength()) 
						point.Y() -= m_DefaultTextRect.Height();  

					// Get the row and column
					nNewSelection = GetIndex( point.Y() / m_nBoxSize, point.X() / m_nBoxSize );

					// In range? If not, default and exit
					if ( nNewSelection < 0 || nNewSelection >= m_nNumColors )
					{
						ClsWindow::WindowProc( uMsg, wParam, lParam );
						return 0;
					}
				}
			}
			else
				nNewSelection = m_nChosenColorSel;

			// OK - we have the row and column of the current selection (may be CUSTOM_BOX_VALUE)
			// Has the row/col selection changed? If yes, then redraw old and new cells.
			if ( nNewSelection != m_nCurrentSel )
				ChangeSelection( nNewSelection );
			break;
		}

		case	WM_LBUTTONDOWN:
		{
			// Clicked inside the control?
			DWORD dwPos = GetMessagePos();
			ClsPoint point = ClsPoint( LOWORD( dwPos ), HIWORD( dwPos ));
			if ( ! m_WindowRect.PtInRect( point )) 
			{
				// Disappear without fading.
				m_bFadeOut = FALSE;
				EndSelection( CPN_SELENDCANCEL );

				// Dunno if I do this right but the messages which 
				// refresh the top-level window must be evaluated before 
				// I requeue the mouse-click. This prevents re-paint problems
				// when the user clicks on the title-bar or one of the
				// window controls of the top-level window to which this
				// control belongs.
				//
				// Anyhow this method seems to work...
				MSG msg;
				while ( PeekMessage( &msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE ))
				{
					// Get the message and dispatch it
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				
				// Resend mouse click.
				mouse_event( MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, LOWORD( dwPos ), HIWORD( dwPos ), 0, 0 );
			}
			return 0;
		}
		
		case	WM_LBUTTONUP:
		{
			// End selection on LButtonUp
			ClsWindow::WindowProc( uMsg, wParam, lParam);

			DWORD pos = GetMessagePos();
			ClsPoint point = ClsPoint( LOWORD( pos ), HIWORD( pos ));

			if ( ! m_bCustomOpen )
			{
				if ( m_WindowRect.PtInRect( point )) { m_bFadeOut = TRUE; EndSelection( CPN_SELENDOK ); }
				else				     { m_bFadeOut = FALSE; EndSelection( CPN_SELENDCANCEL ); }
			}
			return 0;
		}

		case	WM_QUERYNEWPALETTE:
			Invalidate();    
			break;
	
		case	WM_PALETTECHANGED:
		{
			ClsWindow::WindowProc( uMsg, wParam, lParam );

			if (( HWND )wParam != GetSafeHWND())
				Invalidate();
			return 0;
		}

		case	WM_KILLFOCUS:
			ClsWindow::WindowProc( uMsg, wParam, lParam );
			ReleaseCapture();
			return 0;

		case	WM_ACTIVATEAPP:
			// KillFocus problem fix suggested by Paul Wilkerson.
			ClsWindow::WindowProc( uMsg, wParam, lParam );

			// If Deactivating App, cancel this selection
			if ( ! wParam && ! m_bCustomOpen )
				EndSelection(CPN_SELENDCANCEL);
			return 0;

		case	WM_THEMECHANGED:
			// Theming enabled?
			if ( ThemingAPI.IsThemingEnabled())
			{
				if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
				m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"TOOLBAR" );
			}
			break;

		case	WM_PRINT:
		case	WM_PRINTCLIENT:
		{
			if ( lParam & PRF_ERASEBKGND )
				SendMessage( WM_ERASEBKGND, wParam );
			Paint( ClsDC::FromHandle(( HDC )wParam ));
			return 0;
		}

		case	WM_CLOSE:
			EndSelection( CPN_SELENDCANCEL );
			return 0;
	}
	return ClsWindow::WindowProc( uMsg, wParam, lParam );
}

void ClsColorPopup::PostNcDestroy()
{
	// Destroy the object.
	ClsWindow::PostNcDestroy();
	delete this;
}

LRESULT ClsColorPopup::OnEraseBkgnd( ClsDC *pDC )
{
	// Fill the background with the BTNFACE color.
	ClsRect rc = GetClientRect();

	// Do we render our own shadows?
	if ( ClsGetApp()->IsShadowEnabled() == FALSE )
	{
		// Copy screen bits to the window.
		ClsWindowDC dcw( NULL );
		if ( dcw.IsValid())
		{
			ClsRect wr = GetWindowRect();
			pDC->BitBlt( 0, 0, rc.Width(), rc.Height(), &dcw, wr.Left(), wr.Top(), SRCCOPY );
		}

		// Adjust rectangle for the shadow.
		rc.Right()  -= 4;
		rc.Bottom() -= 4;
	}

	// Render background including the frame.
	if ( ! m_bRenderXP )
	{
		// Normal frame.
                pDC->FillRect( rc, ( HBRUSH )( COLOR_BTNFACE + 1 ));
		pDC->DrawEdge( rc, EDGE_RAISED, BF_RECT );
	}
	else
	{
		// Office XP Frame.
		pDC->OutlinedRectangle( rc, XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED ), XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND ));
	
		// Erase the intersecting part of the parent and the
		// popup only if the parent is a child itself.
		if ( m_pParent->GetParent())
		{
			ClsRect pr = m_pParent->GetWindowRect(), tr = GetWindowRect(), ir;
			if ( pr.IsEmpty() == FALSE )
			{
				// Do the rectangles intersect?
				if ( ir.Intersect( pr, tr ))
				{
					// Offset the rectangle so we get client coordinates.
					ir.Offset( -tr.Left(), -tr.Top() );

					// Do not erase the outer edges of the
					// intersecting rectangle.
					if ( ir.Width() > ir.Height()) ir.Deflate( 1, 0 ); else ir.Deflate( 0, 1 );

					// Erase...
					ClsBrush brush;
					brush.CreateSolidBrush( XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND ));
					pDC->FillRect( ir, brush );
				}
			}
		}
	}

	// Do we render our own shadow?
	if ( ClsGetApp()->IsShadowEnabled() == FALSE )
	{
		// Adjust rectangle to fit the shadow.
		rc.Right()  += 4;
		rc.Bottom() += 4;

		// Render the shadow.
		ClsDrawTools::DrawShadow( *pDC, rc );
	}
	return 1;
}

void ClsColorPopup::Paint( ClsDC *pDC )
{
	// Draw the Default Area text
	if ( m_strDefaultText.GetStringLength())
		DrawCell( pDC, DEFAULT_BOX_VALUE );
 
	// Draw color cells
	for ( int i = 0; i < m_nNumColors; i++ )
		DrawCell( pDC, i );
    
	// Draw custom text
	if ( m_strCustomText.GetStringLength())
		DrawCell( pDC, CUSTOM_BOX_VALUE );
}

LRESULT ClsColorPopup::OnPaint( ClsDC *pDC ) 
{
	ClsPaintDC dc( this ); // device context for painting
	Paint( &dc );
	return 0;
}

int ClsColorPopup::GetIndex( int row, int col ) const
{ 
	if (( row == CUSTOM_BOX_VALUE || col == CUSTOM_BOX_VALUE ) && m_strCustomText.GetStringLength())
		return CUSTOM_BOX_VALUE;
	else if (( row == DEFAULT_BOX_VALUE || col == DEFAULT_BOX_VALUE ) && m_strDefaultText.GetStringLength())
		return DEFAULT_BOX_VALUE;
	else if ( row < 0 || col < 0 || row >= m_nNumRows || col >= m_nNumColumns )
		return INVALID_COLOR;
	else
	{
		if ( row * m_nNumColumns + col >= m_nNumColors )
			return INVALID_COLOR;
		else
			return row * m_nNumColumns + col;
	}
}

int ClsColorPopup::GetRow( int nIndex ) const    
{ 
	if ( nIndex == CUSTOM_BOX_VALUE && m_strCustomText.GetStringLength())
		return CUSTOM_BOX_VALUE;
	else if ( nIndex == DEFAULT_BOX_VALUE && m_strDefaultText.GetStringLength())
		return DEFAULT_BOX_VALUE;
	else if ( nIndex < 0 || nIndex >= m_nNumColors )
		return INVALID_COLOR;
	else
		return nIndex / m_nNumColumns; 
}

int ClsColorPopup::GetColumn( int nIndex ) const            
{ 
	if ( nIndex == CUSTOM_BOX_VALUE && m_strCustomText.GetStringLength())
		return CUSTOM_BOX_VALUE;
	else if ( nIndex == DEFAULT_BOX_VALUE && m_strDefaultText.GetStringLength())
		return DEFAULT_BOX_VALUE;
	else if ( nIndex < 0 || nIndex >= m_nNumColors )
		return INVALID_COLOR;
	else
		return nIndex % m_nNumColumns; 
}

void ClsColorPopup::FindCellFromColor( COLORREF crColor )
{
	if ( crColor == CLR_DEFAULT && m_strDefaultText.GetStringLength())
	{
		m_nChosenColorSel = DEFAULT_BOX_VALUE;
		return;
	}

	for ( int i = 0; i < m_nNumColors; i++ )
	{
		if ( GetColor( i ) == crColor )
		{
			m_nChosenColorSel = i;
			return;
		}
	}

	if ( m_strCustomText.GetStringLength())
		m_nChosenColorSel = CUSTOM_BOX_VALUE;
	else
		m_nChosenColorSel = INVALID_COLOR;
}

// Gets the dimensions of the color cell given by (row,col)
BOOL ClsColorPopup::GetCellRect( int nIndex, const LPRECT& rect )
{
	if ( nIndex == CUSTOM_BOX_VALUE )
	{
		::SetRect( rect, 
			   m_CustomTextRect.Left(),  m_CustomTextRect.Top(),
			   m_CustomTextRect.Right(), m_CustomTextRect.Bottom());
		return TRUE;
	}
	else if ( nIndex == DEFAULT_BOX_VALUE )
	{
		::SetRect( rect, 
			   m_DefaultTextRect.Left(),  m_DefaultTextRect.Top(),
			   m_DefaultTextRect.Right(), m_DefaultTextRect.Bottom());
		return TRUE;
	}

	if ( nIndex < 0 || nIndex >= m_nNumColors )
		return FALSE;

	rect->left = GetColumn( nIndex ) * m_nBoxSize + m_nMargin;
	rect->top  = GetRow( nIndex ) * m_nBoxSize + m_nMargin;

	// Move everything down if we are displaying a default text area
	if ( m_strDefaultText.GetStringLength()) 
		rect->top += ( m_nMargin + m_DefaultTextRect.Height());

	rect->right = rect->left + m_nBoxSize;
	rect->bottom = rect->top + m_nBoxSize;

	return TRUE;
}

// Works out an appropriate size and position of this window
void ClsColorPopup::SetWindowSize()
{
	ClsSize TextSize;

	// If we are showing a custom or default text area, get the font and text size.
	if ( m_strCustomText.GetStringLength() || m_strDefaultText.GetStringLength())
	{
		ClsGetDC dc( this );
		ClsFont* pOldFont = ( ClsFont* )dc.SelectObject( &m_Font );

		// Get the size of the custom text (if there IS custom text)
		TextSize = ClsSize( 0, 0 );
		if ( m_strCustomText.GetStringLength())
			TextSize = dc.GetTextExtent( m_strCustomText );

		// Get the size of the default text (if there IS default text)
		if ( m_strDefaultText.GetStringLength())
		{
			ClsSize DefaultSize = dc.GetTextExtent( m_strDefaultText );
			if ( DefaultSize.CX() > TextSize.CX()) TextSize.CX() = DefaultSize.CX();
			if ( DefaultSize.CY() > TextSize.CY()) TextSize.CY() = DefaultSize.CY();
		}

		dc.SelectObject( pOldFont );
		TextSize += ClsSize( 2 * m_nMargin, 2 * m_nMargin );

		// Add even more space to draw the horizontal line
		TextSize.CY() += 2 * m_nMargin + 2;
	}

	// Get the number of columns and rows
	//m_nNumColumns = (int) sqrt((double)m_nNumColors);    // for a square window (yuk)
	m_nNumColumns = 8;
	m_nNumRows = m_nNumColors / m_nNumColumns;
	if ( m_nNumColors % m_nNumColumns ) m_nNumRows++;

	// Get the current window position, and set the new size
	ClsRect rect = GetWindowRect();

	m_WindowRect = ClsRect( rect.Left(), rect.Top(), 
			        rect.Left() + m_nNumColumns * m_nBoxSize + 2 * m_nMargin,
	                        rect.Top()  + m_nNumRows * m_nBoxSize + 2 * m_nMargin );

	// if custom text, then expand window if necessary, and set text width as
	// window width
	if ( m_strDefaultText.GetStringLength()) 
	{
		if ( TextSize.CX() > m_WindowRect.Width())
			m_WindowRect.Right() = m_WindowRect.Left() + TextSize.CX();
		TextSize.CX() = m_WindowRect.Width() - 2 * m_nMargin;

		// Work out the text area
		m_DefaultTextRect = ClsRect( m_nMargin, m_nMargin, 
			                     m_nMargin + TextSize.CX(), 2 * m_nMargin + TextSize.CY());
		m_WindowRect.Bottom() += m_DefaultTextRect.Height() + 2 * m_nMargin;
	}

	// if custom text, then expand window if necessary, and set text width as
	// window width
	if ( m_strCustomText.GetStringLength()) 
	{
		if ( TextSize.CX() > m_WindowRect.Width())
			m_WindowRect.Right() = m_WindowRect.Left() + TextSize.CX();
		TextSize.CX() = m_WindowRect.Width() - 2 * m_nMargin;

		// Work out the text area
		m_CustomTextRect = ClsRect( m_nMargin, m_WindowRect.Height(), 
			                    m_nMargin + TextSize.CX(), 
				            m_WindowRect.Height() + 2 * m_nMargin + TextSize.CY());
		m_WindowRect.Bottom() += m_CustomTextRect.Height() + 2 * m_nMargin;
	}

	// Should we render the shadow ourselves?
	if ( ClsGetApp()->IsShadowEnabled() == FALSE )
	{
		// Make room for the shadow.
		m_WindowRect.Right()  += 4;
		m_WindowRect.Bottom() += 4;
	}

	// Need to check it'll fit on screen
	ClsMultiMon mon;
	ClsRect ScreenRect;
	int nMonitor;
	mon.MonitorNumberFromWindow( GetSafeHWND(), MONITOR_DEFAULTTONEAREST, nMonitor );
	mon.GetMonitorRect( nMonitor, ScreenRect, TRUE);
	
	// To far right?
	if ( m_WindowRect.Right() > ScreenRect.Right())
		m_WindowRect.Offset( -( m_WindowRect.Right() - ScreenRect.Right()), 0 );
	
	// Too far left?
	if ( m_WindowRect.Left() < ScreenRect.Left())
		m_WindowRect.Offset( ScreenRect.Left() - m_WindowRect.Left(), 0 );
	
	// Bottom falling out of screen?
	if ( m_WindowRect.Bottom() > ScreenRect.Bottom())
		m_WindowRect.Offset( 0,  -( m_WindowRect.Bottom() - ScreenRect.Bottom()));

	// To far up?
	if ( m_WindowRect.Top() < ScreenRect.Top())
		m_WindowRect.Offset( 0, ScreenRect.Top() - m_WindowRect.Top());

	// Set the window size and position
	MoveWindow( m_WindowRect, TRUE );
}

void ClsColorPopup::CreateToolTips()
{
	// Create the tool tip
	if ( ! m_ToolTip.Create( this ))
		return;

	// Add a tool for each cell
	for ( int i = 0; i < m_nNumColors; i++ )
	{
		ClsRect rect;
		TOOLINFO ti;
		if ( ! GetCellRect( i, rect )) continue;
		ti.cbSize   = sizeof( ti );
		ti.hwnd     = GetSafeHWND();
		ti.lpszText = ( LPTSTR )GetColorName( i );
		ti.rect     = rect;
		ti.uId      = 1;
		ti.hinst    = ClsGetResourceHandle();
		ti.uFlags   = 0;
		m_ToolTip.AddTool( ti );
	}
}

void ClsColorPopup::ChangeSelection( int nIndex )
{
	ClsGetDC dc( this );        // device context for drawing

	if ( nIndex > m_nNumColors )
		nIndex = CUSTOM_BOX_VALUE; 

	if (( m_nCurrentSel >= 0 && m_nCurrentSel < m_nNumColors ) ||
		m_nCurrentSel == CUSTOM_BOX_VALUE || m_nCurrentSel == DEFAULT_BOX_VALUE )
	{
		// Set Current selection as invalid and redraw old selection (this way
		// the old selection will be drawn unselected)
		int OldSel = m_nCurrentSel;
		m_nCurrentSel = INVALID_COLOR;
		DrawCell( &dc, OldSel );
	}

	// Set the current selection as row/col and draw (it will be drawn selected)
	m_nCurrentSel = nIndex;
	DrawCell( &dc, m_nCurrentSel );

	// Store the current color
	if ( m_nCurrentSel == CUSTOM_BOX_VALUE )
		m_pParent->SendMessage( CPN_SELCHANGE, ( WPARAM ) m_crInitialColor, 0 );
	else if ( m_nCurrentSel == DEFAULT_BOX_VALUE )
	{
		m_crColor = CLR_DEFAULT;
		m_pParent->SendMessage( CPN_SELCHANGE, ( WPARAM ) CLR_DEFAULT, 0 );
	}
	else
	{
		m_crColor = GetColor( m_nCurrentSel );
		m_pParent->SendMessage( CPN_SELCHANGE, ( WPARAM ) m_crColor, 0 );
	}
}

void ClsColorPopup::EndSelection( int nMessage )
{
	ReleaseCapture();
	
	// Closing...
	m_pParent->SendMessage( CPN_CLOSEUP );

	// If custom text selected, perform a custom color selection
	if ( nMessage != CPN_SELENDCANCEL && m_nCurrentSel == CUSTOM_BOX_VALUE )
	{
		ClsColorDialog dlg;
		ClsRect rc = GetWindowRect();
		ShowWindow( SW_HIDE );
		m_bCustomOpen = TRUE;
		if ( dlg.DoModal( m_pParent, m_crInitialColor, ( m_bFullOpen ? CC_FULLOPEN : 0 ) | CC_ANYCOLOR, rc.Left(), rc.Top()))
			m_crColor = dlg.GetColor();
		else
			nMessage = CPN_SELENDCANCEL;
		m_bCustomOpen = FALSE;
	}

	if ( nMessage == CPN_SELENDCANCEL )
		m_crColor = m_crInitialColor;

	m_pParent->SendMessage( nMessage, ( WPARAM ) m_crColor, m_nID );

	if ( IsWindowVisible())
		ShowWindow( SW_HIDE );

	// Kill focus bug fixed by Martin Wawrusch
	Destroy();
}

void ClsColorPopup::DrawCellBackground( ClsDC* pDC, int nIndex, LPCRECT pRect )
{
	// Colors.
	COLORREF crFg = XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND ), crBg = crFg;

	// fill background
	if ( m_nChosenColorSel == nIndex && m_nCurrentSel != nIndex )
	{
		if ( ! m_bRenderXP ) 
		{
			COLORREF crOldTxtColor = pDC->SetTextColor( ::GetSysColor( COLOR_3DHILIGHT ));
			COLORREF crOldBkColor = pDC->SetBkColor( ::GetSysColor( COLOR_BTNFACE ));
			pDC->FillRect( pRect, ClsGetApp()->GetPatternBrush());// HBRUSH )( COLOR_3DLIGHT + 1 ));
			pDC->SetTextColor( crOldTxtColor );
			pDC->SetBkColor( crOldBkColor );
		}
		else
		{
			// Setup colors.
			crBg = XPColors.GetXPColor( ClsXPColors::XPC_INNER_CHECKED );
			crFg = XPColors.GetXPColor( ClsXPColors::XPC_OUTER_CHECKED );
		}
	}
	else if ( m_bRenderXP && m_nChosenColorSel == nIndex && m_nCurrentSel == nIndex )
	{
		// Setup colors.
		crBg = XPColors.GetXPColor( ClsXPColors::XPC_INNER_CHECKED_SELECTED );
		crFg = XPColors.GetXPColor( ClsXPColors::XPC_OUTER_CHECKED );
	}
	else
	{
		// Setup colors.
		if ( ! m_bRenderXP ) pDC->FillRect( pRect, ( HBRUSH )( COLOR_3DFACE + 1 ));
		else if ( m_nCurrentSel == nIndex )
		{
			crBg = XPColors.GetXPColor( ClsXPColors::XPC_INNER_SELECTION );
			crFg = XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION );
		}
	}

	// XP look?
	if ( m_bRenderXP )
		// Render the background.
		pDC->OutlinedRectangle( pRect, crFg, crBg );
}

void ClsColorPopup::DrawCell( ClsDC* pDC, int nIndex )
{
	// Theming enabled?
	if ( ThemingAPI.IsThemingEnabled() && ! m_hTheme && ! m_bRenderXP )
		m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), L"TOOLBAR" );

	// If XP style rendering is enabled we destroy any theme
	// that may still be loaded...
	if ( m_hTheme && m_bRenderXP )
	{
		ThemingAPI.CloseThemeData( m_hTheme );
		m_hTheme = NULL;
	}

	// For the Custom Text area
	if ( m_strCustomText.GetStringLength() && nIndex == CUSTOM_BOX_VALUE )
	{
		// The extent of the actual text button
		ClsRect TextButtonRect = m_CustomTextRect;
		TextButtonRect.Top() += 2 * m_nMargin;

		// Fill background
		if ( ! m_bRenderXP ) pDC->FillRect( TextButtonRect, ( HBRUSH )( COLOR_3DFACE + 1 ));
		else
		{
			ClsBrush b( XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND ));
			pDC->FillRect( TextButtonRect, &b );
		}

		// Draw horizontal line
		ClsRect r = m_CustomTextRect;
		r.Deflate(4, 0);
		pDC->DrawEdge( r, EDGE_ETCHED, BF_TOP );
		TextButtonRect.Deflate( 1, 1 );

		// fill background
		DrawCellBackground( pDC, nIndex, TextButtonRect );

		// Draw button
		if ( ! m_bRenderXP )
		{
			if ( m_nCurrentSel == nIndex ) 
			{
				if ( ! m_hTheme ) pDC->DrawEdge( TextButtonRect, BDR_RAISEDINNER, BF_RECT );
				else		  ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, TP_BUTTON, TS_HOT, TextButtonRect, NULL );
			}
			else if ( m_nChosenColorSel == nIndex )
			{
				if ( ! m_hTheme ) pDC->DrawEdge( TextButtonRect, BDR_SUNKENOUTER, BF_RECT );
				else		  ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, TP_BUTTON, TS_PRESSED, TextButtonRect, NULL );
			}
		}

		// Draw custom text
		ClsFont *pOldFont = ( ClsFont* )pDC->SelectObject( &m_Font );
		pDC->SetBkMode( TRANSPARENT );
		pDC->DrawText( m_strCustomText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		pDC->SelectObject( pOldFont );
		return;
	}        

	// For the Default Text area
	if ( m_strDefaultText.GetStringLength() && nIndex == DEFAULT_BOX_VALUE )
	{
		// Fill background
		if ( ! m_bRenderXP ) pDC->FillRect( m_DefaultTextRect, ( HBRUSH )( COLOR_3DFACE + 1 ));
		else
		{
			ClsBrush b( XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND ));
			pDC->FillRect( m_DefaultTextRect, &b );
		}

		// The extent of the actual text button
		ClsRect TextButtonRect = m_DefaultTextRect;
		TextButtonRect.Deflate( 1, 1 );

		// fill background
		DrawCellBackground( pDC, nIndex, TextButtonRect );

		// Draw thin line around text
		ClsRect LineRect = TextButtonRect;
		LineRect.Deflate( 2 * m_nMargin, 2 * m_nMargin );
		ClsPen pen( PS_SOLID, 1, m_bRenderXP ? XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED ) : ::GetSysColor( COLOR_3DSHADOW ));
		ClsPen* pOldPen = ( ClsPen * )pDC->SelectObject( &pen );
		pDC->SelectObject( ::GetStockObject( NULL_BRUSH ));
		pDC->Rectangle( LineRect );
		pDC->SelectObject( pOldPen );

		if ( ! m_bRenderXP )
		{
			// Draw button
			if ( m_nCurrentSel == nIndex ) 
			{
				if ( ! m_hTheme ) pDC->DrawEdge( TextButtonRect, BDR_RAISEDINNER, BF_RECT );
				else		  ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, TP_BUTTON, TS_HOT, TextButtonRect, NULL );
			}
			else if ( m_nChosenColorSel == nIndex )
			{
				if ( ! m_hTheme ) pDC->DrawEdge( TextButtonRect, BDR_SUNKENOUTER, BF_RECT );
				else		  ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, TP_BUTTON, TS_PRESSED, TextButtonRect, NULL );
			}
		}

		// Draw custom text
		ClsFont *pOldFont = ( ClsFont* )pDC->SelectObject( &m_Font );
		pDC->SetBkMode( TRANSPARENT );
		pDC->DrawText( m_strDefaultText, TextButtonRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		pDC->SelectObject( pOldFont );
		return;
	}        

	ClsRect rect;
	if ( ! GetCellRect( nIndex, rect )) return;

	// Select and realize the palette
	ClsPalette* pOldPalette = NULL;
	if ( pDC->GetDeviceCaps( RASTERCAPS ) & RC_PALETTE )
	{
		pOldPalette = pDC->SelectPalette( &m_Palette, FALSE );
		pDC->RealizePalette();
	}

	DrawCellBackground( pDC, nIndex, rect );

	// Draw button
	if ( ! m_bRenderXP )
	{
		if ( m_nCurrentSel == nIndex ) 
		{
			if ( ! m_hTheme ) pDC->DrawEdge( rect, BDR_RAISEDINNER, BF_RECT );
			else		  ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, TP_BUTTON, TS_HOT, rect, NULL );
		}
		else if ( m_nChosenColorSel == nIndex )
		{
			if ( ! m_hTheme ) pDC->DrawEdge( rect, BDR_SUNKENOUTER, BF_RECT );
			else		  ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, TP_BUTTON, TS_PRESSED, rect, NULL );
		}
	}
	ClsBrush brush( PALETTERGB( GetRValue( GetColor( nIndex )), 
				    GetGValue( GetColor( nIndex )), 
				    GetBValue( GetColor( nIndex ))));
	ClsPen   pen;
	pen.CreatePen( PS_SOLID, 1, m_bRenderXP ? XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED ) : ::GetSysColor( COLOR_3DSHADOW ));

	ClsBrush* pOldBrush = ( ClsBrush* )pDC->SelectObject( &brush );
	ClsPen*   pOldPen   = ( ClsPen* )  pDC->SelectObject( &pen );

	// Draw the cell color
	rect.Deflate( m_nMargin + 1, m_nMargin + 1 );
	pDC->Rectangle( rect );

	// restore DC and cleanup
	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );
	brush.DeleteObject();
	pen.DeleteObject();

	if ( pOldPalette && pDC->GetDeviceCaps( RASTERCAPS ) & RC_PALETTE )
		pDC->SelectPalette( pOldPalette, FALSE );
}