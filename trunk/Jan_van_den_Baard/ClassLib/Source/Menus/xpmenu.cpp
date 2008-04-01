//
// xpmenu.cpp
//
// (C) Copyright 2002-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "xpmenu.h"
#include "../gdi/selector.h"
#include "../gdi/windowdc.h"
#include "../tools/all.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Render radioitem bullet.
void ClsXPMenu::RenderRadioBullet( ClsDC *pDC, ClsRect& rcRect, COLORREF crColor, DWORD dwItemState )
{
	// Snapshot...
	int sDC = pDC->SaveDC();

	// Make ellipse rectangle.
	ClsRect rc = rcRect;
	rc.Right() = rc.Left() + 12;
	rc.Bottom() = rc.Top() + 12;
	rc.Offset( rcRect.Width() / 2 - 6, rcRect.Height() / 2 - 6 );

	// Create GDI objects for unchecked radio-button.
	ClsPen p( PS_SOLID, 0, XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED ));
	ClsBrush b( XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND ));

	// Brace the code so that the ClsSelector objects
	// go out of scope before the device context
	// is restored.
	{
		// Select brush and pen.
		ClsSelector sb( pDC, b );
		ClsSelector sp( pDC, p );

		// Render first ellipse... This yields better
		// results than the Ellipse() method...
		pDC->RoundRect( rc.Left(), rc.Top(), rc.Right(), rc.Bottom(), 11, 11 );

		// Deflate rectangle by three pixels.
		rc.Deflate( 3, 3 );

		// Checked?
		if ( dwItemState & ODS_CHECKED )
		{
			// Destroy current GDI objects and create new
			// ones for the inner ellipse.
			b.Delete(); p.Delete();
			b.CreateSolidBrush( crColor );
			p.CreatePen( PS_SOLID, 1, crColor );

			// Select them in the DC.
			ClsSelector sb( pDC, b );
			ClsSelector sp( pDC, p );

			// Rendert the ellipse.
			pDC->RoundRect( rc.Left(), rc.Top(), rc.Right(), rc.Bottom(), rc.Width() - 1, rc.Width() - 1 );
		}
	}

	// Restore original content.
	pDC->RestoreDC( sDC );
}

// Render a menu item in Office XP style.
#define ISKEYDOWN( code ) ( BOOL )( ::GetAsyncKeyState( code ) & ( 1 << 15 ))
void ClsXPMenu::RenderItem( ClsDC *pDC, LPDRAWITEMSTRUCT pdis, ItemData *pData )
{
	// Determine the image, checkbox and text parts of the menu.
	ClsRect  rcCaption( pdis->rcItem ), rcImage( pdis->rcItem ), rcCheck;
	rcImage.Right() = rcImage.Left() + m_cxBitmap + 8;
	rcCaption.Left() += m_cxBitmap + 8;
	rcCheck = rcImage;
	rcCheck.Deflate( 1, 1 );
	rcCheck.Right()--;

	// A toplevel item?
	if ( pData->m_bToplevel )
	{
		// Selected or hot?
		if ( pdis->itemState & ODS_SELECTED || pdis->itemState & ODS_HOTLIGHT )
		{
			// Select appropiate colors.
			COLORREF crFg, crBg;
			if ( pdis->itemState & ODS_HOTLIGHT || ! IsDropped())
			{
				crFg = XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION);
				crBg = XPColors.GetXPColor( ClsXPColors::XPC_INNER_SELECTION );
			}
			else
			{
				crFg = XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED );
				crBg = XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND );
			}
			// Render the background.
			pDC->OutlinedRectangle( &pdis->rcItem, crFg, crBg );
		}
		else
			// Simply clear background.
			pDC->FillRect( &pdis->rcItem, ( HBRUSH )( COLOR_BTNFACE + 1 ));

		// Adjust the caption rectangle.
		rcCaption = pdis->rcItem;
		rcCaption.Offset( 6, 0 );
	}
	else
	{
		// Is the item selected?
		if (( pdis->itemState & ODS_SELECTED ) && pData->m_bSeparator == FALSE )
		{
			// Determine background color.
			COLORREF crBkgnd;

			// Is the item disabled or not?
			if ( ! ( pdis->itemState & ODS_DISABLED ))
				crBkgnd = XPColors.GetXPColor( ClsXPColors::XPC_INNER_SELECTION );
			else
			{
				// See if either the up or the down arrow key is pressed.
				// If so the item is selected using one of these keys. In
				// That case we render the item differently.
				// 
				// Sorry about the goto...
				if ( ! ISKEYDOWN( VK_UP ) && ! ISKEYDOWN( VK_DOWN ) && ! ISKEYDOWN( VK_LEFT ) && ! ISKEYDOWN( VK_RIGHT )) goto normal;
				crBkgnd = XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND );
			}
			// Render the selection rectangle.
			pDC->OutlinedRectangle( &pdis->rcItem, XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION ), crBkgnd );
		}
		else
		{
normal:
			// Render the image background.
			ClsBrush brush( XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND ));
			pDC->FillRect( rcImage, &brush );

			// render the text background.
			brush.Delete(); 
			brush.CreateSolidBrush( XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND ));
			pDC->FillRect( rcCaption, &brush );
		}

		// Is the item checked?
		if ( pdis->itemState & ODS_CHECKED )
		{
			// Disabled?
			COLORREF crBk, crFg;
			if ( ! ( pdis->itemState & ODS_DISABLED ))
			{
				// Select colors to use.
				crBk = XPColors.GetXPColor( pdis->itemState & ODS_SELECTED ? ClsXPColors::XPC_INNER_CHECKED_SELECTED : ClsXPColors::XPC_INNER_CHECKED );
				crFg = XPColors.GetXPColor( ClsXPColors::XPC_OUTER_CHECKED );
			}
			else
			{
				// Select colors to use.
				crBk = XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND );
				crFg = XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED );
			}
			// Render checkbox image.
			pDC->OutlinedRectangle( rcCheck, crFg, crBk );

			// Does this item have a bitmap?
			if ( pData->m_iBitmap == -1 )
			{
				// Get the item type and checkmark information.
				ClsMenuItemInfo	mi;
				mi.fMask = MIIM_CHECKMARKS | MIIM_TYPE;
				if ( GetItemInfo( pdis->itemID, &mi ))
				{
					// Radiobutton?
					if ( mi.fType & MFT_RADIOCHECK )
						// Render the radiobutton.
						RenderRadioBullet( pDC, 
									rcCheck, 
									XPColors.GetXPColor( pdis->itemState & ODS_DISABLED ? ClsXPColors::XPC_IMAGE_DISABLED : ClsXPColors::XPC_TEXT ),
									pdis->itemState );
					else
						// Render the checkmark or checkmark bitmap.
						RenderCheckmark( pDC, 
									rcCheck, 
									XPColors.GetXPColor( pdis->itemState & ODS_DISABLED ? ClsXPColors::XPC_IMAGE_DISABLED : ClsXPColors::XPC_TEXT ), 
									XPColors.GetXPColor( ClsXPColors::XPC_TEXT ), 
									pdis->itemState & ~ODS_DISABLED, 
									&mi );
				}
			}
		}
		// If we do not have a bitmap and we are
		// not a seperator we check to see if we
		// are a Radio-check item.
		else if ( pData->m_iBitmap == -1 && pData->m_bSeparator == FALSE )
		{
			// Radio item?
			ClsMenuItemInfo	mi;
			mi.fMask = MIIM_CHECKMARKS | MIIM_TYPE;
			if ( GetItemInfo( pdis->itemID, &mi ) && mi.fType & MFT_RADIOCHECK )
			{
				// Render the radiobutton.
				RenderRadioBullet( pDC, 
							rcCheck, 
							XPColors.GetXPColor( pdis->itemState & ODS_DISABLED ? ClsXPColors::XPC_IMAGE_DISABLED : ClsXPColors::XPC_TEXT ),
							pdis->itemState );
			}
		}

		// Item data present?
		if ( pData )
		{
			// Separator?
			if ( pData->m_bSeparator == TRUE )
			{
				// Create and select pen to render the separator.
				ClsPen pen( PS_SOLID, 1, XPColors.GetXPColor( ClsXPColors::XPC_SEPARATOR ));
				ClsSelector s( pDC, pen );

				// Render the separator.
				rcCaption.Top() += rcCaption.Height() / 2;
				rcCaption.Left() += 8;
				pDC->MoveTo( rcCaption.TopLeft());
				pDC->LineTo( rcCaption.Right(), rcCaption.Top());
			}
			else
			{
				// Does the item have a bitmap assigned to it?
				if ( pData->m_iBitmap >= 0 && reinterpret_cast<ClsXPMenu *>( pData->m_pOwner )->m_hImageList )
				{
					// Adjust image position.
					rcImage.Offset( -1, 0 );

					// Determine image rendering flags.
					DWORD dwFlags = 0;
					if ( pdis->itemState & ODS_SELECTED && ( ! ( pdis->itemState & ODS_DISABLED )) && ( ! ( pdis->itemState & ODS_CHECKED )))
						dwFlags |= ClsDrawTools::CDSF_HOT;
					if ( pdis->itemState & ODS_DISABLED )
						dwFlags |= ClsDrawTools::CDSF_DISABLED;

					// Render image...
					ClsDrawTools::RenderXPBitmap( *pDC, reinterpret_cast<ClsXPMenu *>( pData->m_pOwner )->m_hImageList, pData->m_iBitmap, rcImage, dwFlags | ClsDrawTools::CDSF_TRANSPARENT );
					
					// Restore image rectangle position.
					rcImage.Offset( 1, 0 );
				}
			}
		}
	}

	if ( pData )
	{
		// Any text?
		if ( pData->m_strItemName )
		{
			// Setup text color and draw mode.
			pDC->SetTextColor( pdis->itemState & ODS_INACTIVE ? ::GetSysColor( COLOR_GRAYTEXT ) : ( XPColors.GetXPColor( pdis->itemState & ODS_DISABLED ? ClsXPColors::XPC_TEXT_DISABLED : ClsXPColors::XPC_TEXT )));
			pDC->SetBkMode( TRANSPARENT );

			// Adjust the rectangle.
			if ( ! pData->m_bToplevel )
			{
				rcCaption.Left() += 8;
				rcCaption.Right() -= 6;
			}

			// Split caption in a left an right part.
			ClsString Left, Right;
			SplitCaption( pData->m_strItemName, Left, Right );

			// Render the parts.
			if ( Left.GetStringLength())  ClsDrawTools::RenderText( *pDC, Left,  rcCaption, ClsDrawTools::CDSF_LEFTALIGN | ( pdis->itemState & ODS_NOACCEL ? ClsDrawTools::CDSF_HIDEACCEL : 0 ));
			if ( Right.GetStringLength()) ClsDrawTools::RenderText( *pDC, Right, rcCaption, ClsDrawTools::CDSF_RIGHTALIGN );
		}
	}
}

// Reflected WM_MEASUREITEM message handler.
LRESULT ClsXPMenu::OnReflectedMeasureItem( LPMEASUREITEMSTRUCT pmis, BOOL &bNotifyParent )
{
	// Menu?
	if ( pmis->CtlType != ODT_MENU )
		return FALSE;

	// Old style?
	if ( m_bDrawOldStyle ) 
		return ClsBitmapMenu::OnReflectedMeasureItem( pmis, bNotifyParent );

	// Are we a separator? If so we set the values
	// ourselves.
	ItemData *pData = ( ItemData * )pmis->itemData;
	if ( pData == NULL ) return FALSE;
	if ( pData->m_bSeparator == TRUE )
	{
		bNotifyParent = FALSE;
		pmis->itemHeight = 3;
		pmis->itemWidth = 4;
		return TRUE;
	}

	// We let the base class do the computation.
	if ( ClsBitmapMenu::OnReflectedMeasureItem( pmis, bNotifyParent ) == TRUE )
	{
		// Adjust the base class results to suit
		// our needs.
		if ( ! pData->m_bToplevel )
		{
			pmis->itemWidth += 10;
			pmis->itemHeight += 2;
		}
		return TRUE;
	}
	return FALSE;
}

// Reflected WM_DRAWITEM message handler.
LRESULT ClsXPMenu::OnReflectedDrawItem( LPDRAWITEMSTRUCT pdis, BOOL& bNotifyParent )
{
	// Old style?
	if ( m_bDrawOldStyle )
		// Let the baseclass take care of rendering.
		return ClsBitmapMenu::OnReflectedDrawItem( pdis, bNotifyParent );

	// Menu?
	if ( pdis->CtlType != ODT_MENU )
		return FALSE;

	// No need to bother the parent.
	bNotifyParent = FALSE;

	// Get the item data.
	ItemData *pData = ( ItemData * )pdis->itemData;

	// Wrap the DC.
	ClsDC	cDC( pdis->hDC );
	int	sDC = cDC.SaveDC();

	// Render the item.
	RenderItem( &cDC, pdis, pData );

	// Restore DC changes.
	cDC.RestoreDC( sDC );
	return TRUE;
}

// Refresh the parent rectangle.
void ClsXPMenu::RefreshParentRectangle( ClsWindow *pWindow )
{
	// Do we have a parent rectangle?
	ClsRect rc( m_LastParent );
	if ( ! rc.IsEmpty())
	{
		// Add 4 pixels for the shadows.
		rc.Right() += 4;
		rc.Bottom() += 4;

		// Convert to client coordinates.
		pWindow->ScreenToClient( rc );

		// Refresh the rectangle.
		pWindow->InvalidateRect( rc );
		pWindow->UpdateWindow();
	}

}

// Popup menu closed.
void ClsXPMenu::OnReflectedUnInitMenuPopup( ClsWindow *pWindow, HMENU hPopup, LPARAM lParam )
{
	RefreshParentRectangle( pWindow );
	ClsBitmapMenu::OnReflectedUnInitMenuPopup( pWindow, hPopup, lParam ); 
}

// Menu loop exited.
void ClsXPMenu::OnReflectedExitMenuLoop( ClsWindow *pWindow, BOOL bShortcut )
{
	// Obtain menu-bar information.
	MENUBARINFO mbi;
	mbi.cbSize = sizeof( mbi );
	if ( ::GetMenuBarInfo(( HWND )pWindow->GetMenu(), OBJID_MENU, 0, &mbi ))
	{
		// Refresh the rectangle including the space
		// the shadow took up.
		mbi.rcBar.bottom += 4;
		pWindow->InvalidateRect( &mbi.rcBar );
	}
	else
		// Entire window...
		pWindow->Invalidate();

	// Update.
	pWindow->UpdateWindow();
	ClsBitmapMenu::OnReflectedExitMenuLoop( pWindow, bShortcut );
}

// Do we render frames?
BOOL ClsXPMenu::DoFrameRendering() 
{ 
	// Old style, let the baseclass decide.
	if ( m_bDrawOldStyle ) 
		return ClsBitmapMenu::DoFrameRendering(); 

	// Yes, we do it ourselves.
	return TRUE; 
}

// Measure the frame.
void ClsXPMenu::OnMeasureFrame( LPRECT pRect ) 
{ 
	// Old style, let the baseclass decide.
	if ( m_bDrawOldStyle )
	{
		ClsBitmapMenu::OnMeasureFrame( pRect ); 
		return;
	}

	// Setup frame size in pixels.
	pRect->left = pRect->top = 2;
	pRect->right = pRect->bottom = m_bDrawShadow ? ( ClsGetApp()->IsShadowEnabled() ? 2 : 6 ) : 2;
}

// Adjust popup menu position size.
void ClsXPMenu::OnChangeWindowPos( LPWINDOWPOS pWindowPos )
{
	// Old style, let the baseclass decide.
	if ( m_bDrawOldStyle )
	{
		ClsBitmapMenu::OnChangeWindowPos( pWindowPos ); 
		return;
	}

	// If the menu opens below the parent we will put it one
	// pixel up. If it opens above we put it 3 pixels down. This
	// will make it intersect with the parent item.
	if ( ! ( pWindowPos->flags & SWP_NOMOVE ))
	{
		if ( m_LastParent.Bottom() <= pWindowPos->y ) pWindowPos->y--;
		else					      pWindowPos->y += 3;

		// Windows 2000 has the anoying habbit to open the menu right
		// of the parent item the first time it has to open above
		// the parent when we use custom frames. We adjust it here.
		if ( pWindowPos->x == m_LastParent.Right()) pWindowPos->x = m_LastParent.Left();
	}
}

// Called just before the OnDrawFrame() overidable is called.
void ClsXPMenu::PreDrawFrame( LPCRECT pRect, LPCRECT pRectScr )
{
	// Old style?
	if ( m_bDrawOldStyle )
	{
		// Let the baseclass take care of rendering.
		ClsBitmapMenu::PreDrawFrame( pRect, pRectScr );
		return;
	}

	// Do we render the shadow?
	ClsRect rc;
	if ( m_LastParent.IsEmpty() == FALSE && m_bDrawShadow )
	{
		// Get the desktop DC.
		ClsWindowDC wdc( NULL );
		if ( wdc.IsValid())
		{
			// Copy the parent rectangle and adjust the width
			// for the shadow.
			rc = m_LastParent;
			rc.Right() += 4;
			rc.Bottom() += 4;

			// Render the shadow at the parent rectangle.
			ClsDrawTools::DrawShadow( wdc, rc );
		}
	}
}

// Render the menu frame.
void ClsXPMenu::OnDrawFrame( HDC hDC, LPCRECT pRect, LPCRECT pRectScr ) 
{
	// Old style, let the baseclass decide.
	if ( m_bDrawOldStyle )
	{
		ClsBitmapMenu::OnDrawFrame( hDC, pRect, pRectScr ); 
		return;
	}

	// Wrap the input DC.
	ClsDC *pDC = ClsDC::FromHandle( hDC );
	if ( pDC == NULL ) return;

	// Copy the input rectangle.
	ClsRect rc( *pRect );

	// Are shadows enabled? If so we only render the frame.
	// If not we have room to render the shadow ourselves.
	if ( ! ClsGetApp()->IsShadowEnabled() && m_bDrawShadow )
	{
		// Adjust for the shadow pixels.
		rc.Right()  -= 4;
		rc.Bottom() -= 4;
	}

	// Create brush and render the outer frame.
	ClsBrush brush( XPColors.GetXPColor( ClsXPColors::XPC_MENU_FRAME )); 
	pDC->FrameRect( rc, &brush ); 

	// Do the same for the inner frame.
	brush.Delete();
	rc.Deflate( 1, 1 );
	brush.CreateSolidBrush( XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND )); 
	pDC->FrameRect( rc, &brush ); 

	// Do we render the shadow ourselves?
	if ( ! ClsGetApp()->IsShadowEnabled() && m_bDrawShadow )
	{
		// Do we have a parent rectangle?
		if ( m_LastParent.IsEmpty() == FALSE )
		{
			// If the right-sides match we adjust the shadow rectangle to
			// be a union of both rectangle.
			if ( m_LastParent.Right() == pRectScr->right - 4 )
			{
				ClsRect rc;
				rc.Union( m_LastParent, *pRectScr );
				rc.Offset( -pRectScr->left, -pRectScr->top );
				ClsDrawTools::DrawShadow( hDC, rc );
			}
			else
				ClsDrawTools::DrawShadow( hDC, pRect );
		}
		else
			ClsDrawTools::DrawShadow( hDC, pRect );
	}

	// Valid parent?
	if ( m_LastParent.IsEmpty() == FALSE )
	{
		// Create an intersection of the menu rectangle and the
		// parent-item rectangle.
		ClsRect RealRect( *pRectScr );

		// Do we have a shadow?
		if ( ! ClsGetApp()->IsShadowEnabled() && m_bDrawShadow )
			RealRect.Right() -= 4;

		// Clear the intersection.
		if ( rc.Intersect( RealRect, m_LastParent ))
		{
			// Create a rectangle relative to the menu top-left.
			rc.Offset( -pRectScr->left, -pRectScr->top );

			// Adjust to fit.
			if ( rc.Width() > rc.Height()) rc.Deflate( 1, 0 ); else rc.Deflate( 0, 1 );

			// Create a brush to erase the rectangle, which actually
			// is a line instead of a true rectangle.
			brush.Delete();
			brush.CreateSolidBrush( XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_BACKGROUND ));

			// Render.
			pDC->FillRect( rc, brush );
		}
	}
}