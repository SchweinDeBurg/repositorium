//
// bitmapmenu.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "bitmapmenu.h"
#include "../strings/string.h"
#include "../gdi/brush.h"
#include "../gdi/pen.h"
#include "../gdi/font.h"
#include "../gdi/bitmap.h"
#include "../gdi/selector.h"
#include "../gdi/dc.h"
#include "../gdi/getdc.h"
#include "../collectors/linkedlist.h"
#include "../tools/color.h"
#include "../tools/drawstate.h"
#include "../windows/common controls/imagelist.h"
#include "../exceptions/memoryexception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// List of created temporary objects.
ClsLinkedList<ClsBitmapMenu>	temporary_bm_list;
extern ClsLinkedList<ClsMenu>	global_menu_list;

// Constructor. Clears data fields.
ClsBitmapMenu::ClsBitmapMenu()
{
	// Clear fields.
	m_hImageList	= NULL;
	m_bIsSubMenu	= FALSE;
	m_bIconFrame	= TRUE;
	m_bConverted	= FALSE;
	m_bIsPopup	= FALSE;
	m_bDrawShadow	= TRUE;
	m_cxBitmap	= m_cyBitmap = 16;

	// Initialize separator data.
	m_SeparatorData.m_pOwner     = this;
	m_SeparatorData.m_bSeparator = TRUE;
	m_SeparatorData.m_bToplevel  = FALSE;
	m_SeparatorData.m_iBitmap    = -1;
	m_SeparatorData.m_wID	     = SC_SEPARATOR;
}

// Destructor. Deletes the image list
// and the item strings.
ClsBitmapMenu::~ClsBitmapMenu()
{
	// Delete it all.
	Destroy();

	// Delete left over contents.
	for ( int i = 0; i < m_ItemArray.GetSize(); i++ )
		delete m_ItemArray[ i ];
}

// Destroy the item data of all the menu items.
void ClsBitmapMenu::DestroyItemData( HMENU hMenu /* = NULL */ )
{
	// Use menu which is passed or the wrapped handle.
	HMENU hUseMenu = hMenu ? hMenu : m_hMenu;

	// How many items?
	int nItems = ::GetMenuItemCount( hUseMenu );

	// Iterate items.
	for ( int i = 0; i < nItems; i++ )
	{
		// Does this item have a submenu? If so
		// destroy it.
		HMENU hSubMenu = ::GetSubMenu( hUseMenu, i );
		if ( hSubMenu )	DestroyItemData( hSubMenu );

		// Obtain item data pointer.
		ClsMenuItemInfo mi;
		mi.fMask = MIIM_DATA;
		if ( ::GetMenuItemInfo( hUseMenu, i, TRUE, &mi ))
		{
			// Cast pointer and delete it.
			ItemData *pData = ( ItemData * )mi.dwItemData;
			if ( pData )
				RemoveFromArray( pData );
		}
	}
}

// Delete a menu item including it's sub-items.
BOOL ClsBitmapMenu::DeleteMenu( UINT uPosition, BOOL bByPos /* = FALSE */ )
{
	// Get item data.
	ClsMenuItemInfo	 mi;
	mi.fMask = MIIM_DATA | MIIM_SUBMENU | MIIM_TYPE;
	mi.dwTypeData = NULL;
	if ( GetItemInfo( uPosition, &mi, bByPos ))
	{
		// Only ownerdraw items.
		if ( mi.fType == MFT_OWNERDRAW )
		{
			// Get the pointer to the item
			// data structure.
			ItemData *pData = ( ItemData * )mi.dwItemData;
			if ( pData )
				RemoveFromArray( pData );

			// Popup?
			if ( mi.hSubMenu )
				// Destroy the data of that aswell.
				DestroyItemData( mi.hSubMenu );
		}
		// And the menu itself.
		return ClsMenu::DeleteMenu( uPosition, bByPos );
	}
	return FALSE;
}

// Destroys the image list and item
// strings.
BOOL ClsBitmapMenu::Destroy()
{
	// Only when the handle is valid.
	if ( m_hMenu )
	{
		// Free item data if this is not a
		// menu created by GetSubMenu().
		if ( ! m_bIsSubMenu )
		{
			// Destroy the data from the menus.
			DestroyItemData();

			// Destroy the handle.
			return ClsMenu::Destroy();
		}
	}
	return TRUE;
}

// Remove all objects from the temporary list. All call's
// to GetSubMenu() will create a temporary object.
void ClsBitmapMenu::DeleteTempObjects()
{
	// Iterate temporary objects deleting
	// them.
	ClsBitmapMenu *pBMenu;
	while (( pBMenu = temporary_bm_list.RemoveHead()) != NULL )
	{
		// Detach the handle before destroying it.
		pBMenu->Detach();
		delete pBMenu;
	}
}

// Setup the imagelist to use.
HIMAGELIST ClsBitmapMenu::SetImageList( HIMAGELIST hImages )
{
	// Get the old images.
	HIMAGELIST hOldImages = m_hImageList;

	// Get the new image-list and retrieve it's image size.
	if (( m_hImageList = hImages ) != NULL )
		::ImageList_GetIconSize( hImages, &m_cxBitmap, &m_cyBitmap );

	// Return old list.
	return hOldImages;
}

// Convert the menus.
BOOL ClsBitmapMenu::ConvertMenus()
{
	// Do the conversion.
	GarbageCollect();
	m_bConverted = ConvertMenusHelper();
	return m_bConverted;
}

// Convert the menus to ownerdraw menus.
BOOL ClsBitmapMenu::ConvertMenusHelper( HMENU hMenu /* = NULL */ )
{
	_ASSERT_VALID( m_hMenu ); // Must be valid.

	// Get the menu handle to use.
	HMENU hUseMenu = hMenu ? hMenu : m_hMenu;

	// Get the item count.
	int nItems = ::GetMenuItemCount( hUseMenu );

	// Iterate items.
	ClsMenuItemInfo	mi;
	TCHAR szBuffer[ 4096 ]; // Win32 strings are 4Kchars maximum.
	for ( int i = 0; i < nItems; i++ )
	{
		// Any submenus?
		HMENU hSubMenu = ::GetSubMenu( hUseMenu, i );
		if ( hSubMenu )
		{
			// Convert the submenus also.
			if ( ! ConvertMenusHelper( hSubMenu ))
				return FALSE;
		}

		// Get the item type data.
		mi.fMask	= MIIM_TYPE | MIIM_ID;
		mi.dwTypeData	= szBuffer;
		mi.cch		= 4096;

		// No conversion of system menus
		// allowed...
		if ( ! IsSystemMenu( hUseMenu ))
		{
			if ( ::GetMenuItemInfo( hUseMenu, i, TRUE, &mi ))
			{
				// String?
				if ( mi.fType == MFT_STRING )
				{
					try
					{
						// Create data structure.
						ItemData *pData = new ItemData;

						// Setup data structure.
						pData->m_pOwner	     = this;
						pData->m_bSeparator  = FALSE;
						pData->m_bToplevel   = ( hMenu == NULL && m_bIsPopup == FALSE ) ? TRUE : FALSE;
						pData->m_iBitmap     = -1;
						pData->m_strItemName = szBuffer;
						pData->m_wID	     = mi.wID;

						// Force it to be an onwerdraw
						// item.
						mi.fMask      = MIIM_TYPE | MIIM_DATA;
						mi.fType      = MFT_OWNERDRAW;
						mi.dwItemData = ( ULONG_PTR )pData;

						// Change the item.
						if ( m_ItemArray.Add( pData ) >= 0 )
						{
							if ( ! ::SetMenuItemInfo( hUseMenu, i, TRUE, &mi ))
							{
								delete pData;
								return FALSE;
							}
						}
						else
						{
							// Failed.
							delete pData;
							return FALSE;
						}
					}
					catch( ClsMemoryException& )
					{
						return FALSE;
					}
				}
				else if ( mi.fType == MFT_SEPARATOR )
				{
					// Change the type of the
					mi.fMask	= MIIM_TYPE | MIIM_DATA | MIIM_STATE;
					mi.fType	= MFT_OWNERDRAW | MFT_SEPARATOR;
					mi.fState	= MFS_DISABLED;
					mi.dwItemData   = ( ULONG_PTR )&m_SeparatorData;

					// Change the item.
					if ( ! ::SetMenuItemInfo( hUseMenu, i, TRUE, &mi ))
						return FALSE;
				}
			}
		}
		else
			return FALSE;
	}
	return TRUE;
}

// Set an item it's bitmap.
BOOL ClsBitmapMenu::SetItemBitmap( UINT nItemID, int iBitmap, BOOL bByPos /* = FALSE */ )
{
	// Is the conversion done yet?
	if ( ! m_bConverted )
		m_bConverted = ConvertMenusHelper();

	// Get item data.
	ClsMenuItemInfo	 mi;
	mi.fMask	= MIIM_DATA | MIIM_TYPE;
	mi.dwTypeData	= NULL;
	mi.cch		= 0;
	if ( GetItemInfo( nItemID, &mi, bByPos ))
	{
		// Only ownerdraw items.
		if ( mi.fType == MFT_OWNERDRAW )
		{
			// Get the pointer to the item
			// data structure.
			ItemData *pData = ( ItemData * )mi.dwItemData;
			if ( pData )
			{
				// Setup the bitmap index.
				pData->m_iBitmap = iBitmap;
				return TRUE;
			}
		}
		else
			return TRUE;
	}

	return FALSE;
}

// Set item bitmaps.
BOOL ClsBitmapMenu::SetItemBitmap( StrItemBitmap *pItemBitmaps, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( pItemBitmaps ); // Must be valid.

	// Iterate array.
	while ( pItemBitmaps->m_wID != SBM_END )
	{
		// Set the bitmap.
		if ( SetItemBitmap( pItemBitmaps->m_wID, pItemBitmaps->m_iBitmap, bByPos ) == FALSE )
			return FALSE;

		// Next...
		pItemBitmaps++;
	}
	return TRUE;
}

// Get a menu text.
BOOL ClsBitmapMenu::GetItemText( UINT nItemID, ClsString& str, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // No items in a NULL handle.

	// Preset string.
	str = _T( "" );

	// Obtain item information.
	ClsMenuItemInfo mi;
	TCHAR sz[ 4096 ];
	mi.dwTypeData = sz;
	mi.cch        = 4096;
	mi.fMask      = MIIM_DATA | MIIM_TYPE;
	if ( GetItemInfo( nItemID, &mi, bByPos ))
	{
		// Is it a converted item?
		if ( mi.fType & MFT_OWNERDRAW && mi.dwItemData )
		{
			// Obtain data and copy string.
			ItemData *pData = ( ItemData * )mi.dwItemData;
			str = pData->m_strItemName;
		}
		else
			// Set string.
			str = sz;
		return TRUE;
	}
	return FALSE;
}

// Set a menu text.
BOOL ClsBitmapMenu::SetItemText( UINT nItemID, LPCTSTR pszName, BOOL bByPos /* = FALSE */ )
{
	_ASSERT_VALID( m_hMenu ); // No items in a NULL handle.
	_ASSERT_VALID( pszName ); // Must be valid.

	// Obtain item information.
	ClsMenuItemInfo mi;
	mi.dwTypeData = NULL;
	mi.fMask      = MIIM_DATA | MIIM_TYPE;
	if ( GetItemInfo( nItemID, &mi, bByPos ))
	{
		// Is it a converted item?
		if ( mi.fType & MFT_OWNERDRAW && mi.dwItemData )
		{
			// Obtain data an replace the old string.
			ItemData *pData = ( ItemData * )mi.dwItemData;
			pData->m_strItemName = pszName;
			return TRUE;
		}
		else
		{
			// Set string.
			mi.cch = _tcslen( pszName );
			mi.dwTypeData = ( LPTSTR )pszName;
                        BOOL rc = SetItemInfo( nItemID, &mi, bByPos );
			return rc;
		}
	}
	return FALSE;
}

// Get a sub-menu.
ClsBitmapMenu *ClsBitmapMenu::GetSubMenu( int nPos )
{
	_ASSERT_VALID( m_hMenu ); // No items in a NULL handle.

	// Get the sub menu.
	HMENU hMenu = ::GetSubMenu( m_hMenu, nPos );

	// OK?
	if ( hMenu )
	{
		// Does it already exist?
		ClsBitmapMenu *pMenu;
		for ( pMenu = temporary_bm_list.GetFirst(); pMenu; pMenu = temporary_bm_list.GetNext( pMenu ))
		{
			// Is this it?
			if ( *pMenu == hMenu )
				return pMenu;
		}

		// Create menu.
		pMenu = new ClsBitmapMenu;
		pMenu->Attach( hMenu );

		// Copy resource data.
		//
		// The Free() routine will not
		// free the allocated data from an
		// object which is created with the
		// GetSubMenu() call.
		pMenu->m_cxBitmap   = m_cxBitmap;
		pMenu->m_cyBitmap   = m_cyBitmap;
		pMenu->m_hImageList = m_hImageList;

		// Make sure the object knows it is
		// a submenu. This way it will not
		// free the parent resources.
		pMenu->m_bIsSubMenu = TRUE;
		pMenu->m_bIsPopup   = TRUE;

		// Remove it from the global menu list.
		global_menu_list.Remove( pMenu );

		// Add it to the list.
		temporary_bm_list.AddHead( pMenu );
		return pMenu;
	}
	return NULL;
}

// Measure an item.
LRESULT ClsBitmapMenu::OnReflectedMeasureItem( LPMEASUREITEMSTRUCT pmis, BOOL& bNotifyParent )
{
	// Correct message?
	if ( pmis->CtlType != ODT_MENU )
		return FALSE;

	// Seperator?
	ItemData *pData = ( ItemData * )pmis->itemData;
	if ( pData == NULL ) return FALSE;
	if ( pData->m_bSeparator == TRUE )
	{
		pmis->itemWidth = ::GetSystemMetrics( SM_CXFRAME  ) * 3;
		pmis->itemHeight = ::GetSystemMetrics( SM_CYFRAME ) * 2;
		return TRUE;
	}

	// Preset result.
	BOOL	 bRC = FALSE;

	// No need to bother the parent.
	bNotifyParent = FALSE;

	// Valid data?
	if ( pData != NULL )
	{
		// Yes. We have at least a width and
		// height of the image width and height
		// increased by 2 pixels unless we are
		// top-level.
		if ( ! pData->m_bToplevel )
		{
			pmis->itemWidth  = m_cxBitmap + 6;
			pmis->itemHeight = m_cyBitmap + 4;
		}
		else
			pmis->itemWidth = pmis->itemHeight = 0;

		// Any text?
		if ( pData->m_strItemName.GetStringLength())
		{
			// Create a display device context.
			ClsDC displayDC;
			if ( displayDC.CreateDC( _T( "DISPLAY" ), NULL, NULL, NULL ))
			{
				// Get the menu font from the system
				NONCLIENTMETRICS	ncm = { sizeof( NONCLIENTMETRICS ), 0 };
				if ( ::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( ncm ), ( void * )&ncm, 0 ))
				{
					// Create the font.
					ClsFont	hFont( &ncm.lfMenuFont );
					if ( hFont.IsValid())
					{
						// Set the font.
						ClsSelector selFont( &displayDC, hFont );

						// Measure the text.
						ClsRect	cRect( 0, 0, 0, 0 );
						if ( displayDC.DrawText( pData->m_strItemName, pData->m_strItemName.GetStringLength(), cRect, DT_EXPANDTABS | DT_CALCRECT ))
						{
							// Adjust the size to also include the
							// item text.
							if ( ! pData->m_bToplevel )
							{
								pmis->itemWidth += cRect.Width() + 20;
								pmis->itemHeight = pmis->itemHeight < ( UINT )cRect.Height() ? cRect.Height() : pmis->itemHeight;
							}
							else
							{
								// The system seems to add the extra room needed...
								pmis->itemWidth += cRect.Width();
								pmis->itemHeight += cRect.Height();
							}

							// Make sure the system defined size is not
							// larger than what we computed.
							int cSystemSize = ::GetSystemMetrics( SM_CYMENU );
							if ( pmis->itemHeight < ( UINT )cSystemSize )
								pmis->itemHeight = cSystemSize;

							// Success.
							bRC = TRUE;
						}
					}
				}
				// Delete the DC.
				displayDC.DeleteDC();
			}
		}
	}
	return bRC;
}

// Split-up the menu caption string in a left and a right part.
void ClsBitmapMenu::SplitCaption( LPCTSTR pszCaption, ClsString& sLeft, ClsString& sRight )
{
	// Assign left strings.
	sLeft = pszCaption;
	sRight.SetStringLength( 0 );

	// If we have a tab character in the item text
	// we split the string in two seperate parts.
	// The item name and the accelerator key string.
	if ( _tcschr( pszCaption, _T( '\t' )))
	{
		// Create a left and right part.
		int nTabPos = sLeft.Find( _T( '\t' )), nPos = nTabPos;

		// Skip other tab characters.
		while ( sLeft[ nPos ] == _T( '\t' )) nPos++;

		// Copy the right part of the item text.
		sRight = sLeft.Right( sLeft.GetStringLength() - nPos );

		// Truncate the left part.
		sLeft.SetStringLength( nPos );
	}
}

// Render radioitem bullet.
void ClsBitmapMenu::RenderRadioBullet( ClsDC *pDC, ClsRect& rcRect, COLORREF crColor )
{
	// Get GDI objects.
	ClsBrush	hBrush( crColor );
	ClsPen		hPen( PS_SOLID, 0, crColor );

	// Objects OK?
	if ( hBrush.IsValid() && hPen.IsValid())
	{
		// Select them. When these objects go out
		// of scope the original objects will be
		// selected automatically.
		ClsSelector selBrush( pDC, hBrush );
		ClsSelector selPen( pDC, hPen );

		// Render the bullet.
		pDC->Ellipse( rcRect.Left()   + (( rcRect.Width()  / 2 ) - 4 ),
			      rcRect.Top()    + (( rcRect.Height() / 2 ) - 4 ),
			      rcRect.Right()  - (( rcRect.Width()  / 2 ) - 4 ),
			      rcRect.Bottom() - (( rcRect.Height() / 2 ) - 4 ));
	}
}

// Render checkmark.
void ClsBitmapMenu::RenderCheckmark( ClsDC *pDC, ClsRect& rcRect, COLORREF crFgColor, COLORREF crBgColor, DWORD dwItemState, LPMENUITEMINFO pmii )
{
	// Bitmap to render.
	ClsBitmap	hCheckmark;

	// Checked?
	if ( dwItemState & ODS_CHECKED )
	{
		// Do we have a bitmap?
		if ( pmii->hbmpChecked ) hCheckmark.Attach( pmii->hbmpChecked );
	}
	else
	{
		// Any object?
		if ( pmii->hbmpUnchecked )
			hCheckmark.Attach( pmii->hbmpUnchecked );
	}

	// Any bitmap?
	if ( hCheckmark.IsValid())
	{
		// Get information.
		BITMAP bitMap;
		if ( hCheckmark.GetBitmap( &bitMap ))
		{
			// Setup image colors.
			pDC->SetTextColor( crFgColor );
			pDC->SetBkColor(   crBgColor );

			// Draw the image.
			ClsBrush cmc( crFgColor );
			pDC->DrawState( ClsPoint( rcRect.Left() + (( rcRect.Width()  / 2 ) - ( bitMap.bmWidth  / 2 )),
							rcRect.Top()  + (( rcRect.Height() / 2 ) - ( bitMap.bmHeight / 2 )) - 1 ),
					ClsSize( rcRect.Width(), rcRect.Height()),
					&hCheckmark,
					DSS_MONO | ( dwItemState & ODS_DISABLED ? DSS_DISABLED : 0 ), &cmc );
		}
	}
	else
		// Draw our own checkmark...
		ClsDrawTools::RenderMonoBitmap( *pDC, ClsGetApp()->GetClsImages(), ClsApp::CII_CHECKMARK, rcRect, crFgColor );
}

// Render an item.
LRESULT ClsBitmapMenu::OnReflectedDrawItem( LPDRAWITEMSTRUCT pdis, BOOL& bNotifyParent )
{
	// Menu?
	if ( pdis->CtlType != ODT_MENU )
		return FALSE;

	// No need to bother the parent.
	bNotifyParent = FALSE;

	// Seperator?
	ItemData *pData = ( ItemData * )pdis->itemData;
	if ( pData == NULL ) return FALSE;
	if ( pData->m_bSeparator == TRUE )
	{
		// Render the seperator.
		ClsRect rc( pdis->rcItem );
		::FillRect( pdis->hDC, rc, ( HBRUSH )( COLOR_MENU + 1 ));

		// Adjust seperator position and
		// render it.
		rc.Top() += rc.Height() / 2;
		::DrawEdge( pdis->hDC, rc, EDGE_ETCHED, BF_TOP );
		return TRUE;
	}

	// Wrap the DC.
	ClsDC	cDC( pdis->hDC );

	// Get some flags settings.
	BOOL bDisabled = ( BOOL )( pdis->itemState & ( ODS_DISABLED | ODS_GRAYED ));
	BOOL bSelected = ( BOOL )( pdis->itemState & ODS_SELECTED );
	BOOL bChecked  = ( BOOL )( pdis->itemState & ODS_CHECKED );

	// Get background color.
	ClsRect		rcBitmap;

	// Snapshot the DC.
	int nDC = cDC.SaveDC();
	int nCol;

	// Are we top-level?
	if ( ! pData->m_bToplevel )
	{
		// Create icon rectangle.
		rcBitmap	 = pdis->rcItem;
		rcBitmap.Right() = rcBitmap.Left() + m_cxBitmap + 5;

		// Do we render a frame or are we checked?
		if ( m_bIconFrame || bChecked )
			nCol = COLOR_MENU;
		else
		{
			// Disabled?
			if ( bDisabled )
			{
				// Are we selected?
				if ( bSelected ) nCol = COLOR_HIGHLIGHT;
				else 		 nCol = COLOR_MENU;
			}
			else
			{
				// Selected or not?
				if ( bSelected ) nCol = COLOR_HIGHLIGHT;
				else		 nCol = COLOR_MENU;
			}
		}

		// Clear icon area.
		cDC.FillRect( rcBitmap, ( HBRUSH )( nCol + 1 ));
		rcBitmap.Right()--;

		// Render the checkmark rectangle.
		if ( bChecked )
		{
			// Create a rectangle 2 pixels smaller than the
			// bitmap rectangle.
			ClsRect		rc = rcBitmap;
			rc.Deflate( 1, 1 );

			// Render the background of the
			// checkmark rectangle.
			if ( bSelected == FALSE && bDisabled == FALSE )
			{
				// Convert the menu background color to a color
				// with it's lightness increased or decreased by 20
				// depending on the current lightness level.
				ClsColor cc( ::GetSysColor( COLOR_MENU ));
				if ( cc.Lightness() >= 220 ) cc.DarkenColor( 0.1, 0.1, 0.1 );
				else cc.LightenColor( 0.5, 0.5, 0.5 );
				ClsBrush bgc( cc );
				cDC.FillRect( rc, bgc );
			}

			// Render the rectangle.
			cDC.DrawEdge( rc, BDR_SUNKENOUTER, BF_RECT );
		}

		// Render bitmap?.
		if ( pData->m_iBitmap >= 0 && pData->m_pOwner->m_hImageList )
		{
			_ASSERT_VALID( pData->m_pOwner->m_hImageList ); // Should be valid.

			// Render.
			ClsDrawTools::RenderBitmap( pdis->hDC, pData->m_pOwner->m_hImageList, pData->m_iBitmap, rcBitmap, ClsDrawTools::CDSF_TRANSPARENT | ( pdis->itemState & ODS_DISABLED ? ClsDrawTools::CDSF_DISABLED : 0 ));

			// Draw icon rectangle when necessary
			if ( bSelected && bDisabled == FALSE && m_bIconFrame )
			{
				// Create icon rectangle.
				ClsRect	rcRect = pdis->rcItem;

				// Adjust...
				rcRect.Left()++;
				rcRect.Right() = rcRect.Left() + m_cxBitmap + 4;

				// Render rectangle.
				cDC.DrawEdge( rcRect, BDR_RAISEDINNER, BF_RECT );
			}
		}
	}

	// Render text...
	if ( pData->m_strItemName.GetStringLength())
		RenderItemText( pdis, pData );

	// Did we render a bitmap?
	if ( pData->m_iBitmap == -1 )
	{
		// Checkmark?
		ClsMenuItemInfo	mi;
		mi.fMask = MIIM_CHECKMARKS | MIIM_TYPE;

		// Get the information.
		if ( GetItemInfo( pdis->itemID, &mi ))
		{
			// Draw a bullet checkmark?
			if (( mi.fType & MFT_RADIOCHECK ) && bChecked )
				// Render the radio bullet.
				RenderRadioBullet( &cDC, rcBitmap, bDisabled ? ::GetSysColor( COLOR_GRAYTEXT ) : ::GetSysColor( COLOR_MENUTEXT ));
			else if ( bChecked )
				// Render the checkmark.
				RenderCheckmark( &cDC, rcBitmap, bDisabled ? ::GetSysColor( COLOR_GRAYTEXT ) : ::GetSysColor( COLOR_MENUTEXT ), ::GetSysColor( COLOR_MENU ), pdis->itemState, &mi );
		}
	}

	// Reset last snapshot.
	cDC.RestoreDC( nDC );
	return TRUE;
}

// Helper to render the item text.
void ClsBitmapMenu::RenderItemText( DRAWITEMSTRUCT *pdis, ItemData *pData )
{
	int			nCol, nBrush;
	ClsBrush		hBrush;
	ClsSize			szSize;
	ClsRect			rcRect = pdis->rcItem;
	ClsDC			cDC( pdis->hDC );

	// Get some flags settings.
	BOOL bDisabled = ( BOOL )( pdis->itemState & ( ODS_DISABLED | ODS_GRAYED ));
	BOOL bSelected = ( BOOL )( pdis->itemState & ODS_SELECTED );
	BOOL bChecked  = ( BOOL )( pdis->itemState & ODS_CHECKED );

	// Toplevel item?
	if ( pData->m_bToplevel )
	{
		// Render background.
		cDC.FillRect( &pdis->rcItem, ( HBRUSH )( COLOR_BTNFACE + 1 ));

		// Render rectangle when necessary.
		if ( bSelected || pdis->itemState & ODS_HOTLIGHT )
			cDC.DrawEdge( &pdis->rcItem, ( bSelected && IsDropped()) ? BDR_SUNKENOUTER : BDR_RAISEDINNER, BF_RECT );

		// Select the color in which the text is rendered.
		nCol = pdis->itemState & ODS_INACTIVE ? COLOR_GRAYTEXT : COLOR_MENUTEXT;

		// Determine text rectangle.
		rcRect.Offset(( bSelected && IsDropped()) ? 7 :6, ( bSelected && IsDropped()) ? 1 : 0 );
	}
	else
	{
		// Disabled?
		if ( bDisabled )
		{
			// Are we selected?
			if ( bSelected )
			{
				// Render using highlight color.
				nBrush = COLOR_HIGHLIGHT;

				// Remove the disabled bit and render
				pdis->itemState &= ~ODS_DISABLED;
			}
			else
				// Menu color.
				nBrush = COLOR_MENU;

			// Gray text.
			nCol   = COLOR_GRAYTEXT;
		}
		else
		{
			// Selected or not?
			if ( bSelected )
			{
				// Highlight color.
				nBrush = COLOR_HIGHLIGHT;

				// Highlight text color.
				nCol   = COLOR_HIGHLIGHTTEXT;
			}
			else
			{
				// Menu
				nBrush = COLOR_MENU;
				nCol   = COLOR_MENUTEXT;
			}
		}

		// Create the brush.
		hBrush.CreateSysColorBrush( nBrush );

		// We fill the whole area if the item has
		// a bitmap to render or if it is checked.
		if ( pData->m_iBitmap >= 0 || bChecked ) rcRect.Left() += m_cxBitmap +  + ( m_bIconFrame ? 6 : 4 );

		// Render the background.
		if ( hBrush.IsValid())
			cDC.FillRect( rcRect, &hBrush );

		// Skip checkmark width.
		rcRect.Right() -= ::GetSystemMetrics( SM_CXMENUCHECK );

		// Move right.
		if ( pData->m_iBitmap >= 0 || bChecked ) rcRect.Left() += 2;
		else					 rcRect.Left() += m_cxBitmap + ( m_bIconFrame ? 6 : 4 );
	}

	// Any text?
	if ( pData->m_strItemName )
	{
		// Set draw mode and color.
		cDC.SetBkMode( TRANSPARENT );
		cDC.SetTextColor( GetSysColor( nCol ));

		// Split caption in a left an right part.
		ClsString Left, Right;
		SplitCaption( pData->m_strItemName, Left, Right );

		// Make some room.
		rcRect.Right() -= 6;

		// Render the parts.
		if ( Left.GetStringLength())  ClsDrawTools::RenderText( pdis->hDC, Left,  rcRect, ClsDrawTools::CDSF_LEFTALIGN  | ( pdis->itemState & ODS_DISABLED ? ClsDrawTools::CDSF_DISABLED : 0 ) | ( pdis->itemState & ODS_NOACCEL ? ClsDrawTools::CDSF_HIDEACCEL : 0 ));
		if ( Right.GetStringLength()) ClsDrawTools::RenderText( pdis->hDC, Right, rcRect, ClsDrawTools::CDSF_RIGHTALIGN | ( pdis->itemState & ODS_DISABLED ? ClsDrawTools::CDSF_DISABLED : 0 ));
	}
}

// Do we render frames?
BOOL ClsBitmapMenu::DoFrameRendering()
{
	// Yes, we do it ourselves.
	return TRUE;
}

// Adjust popup menu position size.
void ClsBitmapMenu::OnChangeWindowPos( LPWINDOWPOS pWindowPos )
{
	// Move allowed?
	if ( ! ( pWindowPos->flags & SWP_NOMOVE ))
	{
		// Windows 2000 has the anoying habbit to open the menu right
		// of the parent item the first time it has to open above
		// the parent when we use custom frames. We adjust it here.
		if ( pWindowPos->x == m_LastParent.Right()) pWindowPos->x = m_LastParent.Left();
	}
}

// Measure the frame.
void ClsBitmapMenu::OnMeasureFrame( LPRECT pRect )
{
	// Get frame sizes.
	int x = ::GetSystemMetrics( SM_CXEDGE ) + 1;
	int y = ::GetSystemMetrics( SM_CYEDGE ) + 1;

	// Setup frame size in pixels.
	pRect->left   = x;
	pRect->top    = y;
	pRect->right  = m_bDrawShadow ? ( ClsGetApp()->IsShadowEnabled() ? x : x + 4 ) : x;
	pRect->bottom = m_bDrawShadow ? ( ClsGetApp()->IsShadowEnabled() ? y : y + 4 ) : y;
}

// Render the menu frame.
void ClsBitmapMenu::OnDrawFrame( HDC hDC, LPCRECT pRect, LPCRECT pRectSrc )
{
	// Wrap the input DC.
	ClsDC *pDC = ClsDC::FromHandle( hDC );
	if ( pDC == NULL ) return;

	// Copy the input rectangle.
	ClsRect rc = *pRect;

	// Are shadows enabled? If so we only render the frame.
	// If not we have room to render the shadow ourselves.
	if ( ! ClsGetApp()->IsShadowEnabled() && m_bDrawShadow )
	{
		// Adjust for the shadow pixels.
		rc.Right()  -= 4;
		rc.Bottom() -= 4;
	}

	// Render the menu frame.
	pDC->DrawEdge( rc, EDGE_RAISED, BF_RECT );
	rc.Deflate( ::GetSystemMetrics( SM_CXEDGE ), ::GetSystemMetrics( SM_CYEDGE ));
	ClsBrush brush( ::GetSysColor( COLOR_MENU ));
	pDC->FrameRect( rc, &brush );

	// Do we render the shadow ourselves?
	if ( ! ClsGetApp()->IsShadowEnabled() && m_bDrawShadow )
		ClsDrawTools::DrawShadow( hDC, pRect );
}

// Remove ItemData pointer from the array.
void ClsBitmapMenu::RemoveFromArray( ItemData *pData )
{
	// Iterate the array.
	for ( int i = 0; i < m_ItemArray.GetSize(); i++ )
	{
		// Is this the one?
		if ( pData == m_ItemArray[ i ] )
		{
			// Remove from the array and delete
			// the data pointer.
			m_ItemArray.RemoveAt( i, 1 );
			delete pData;
			return;
		}
	}
}

// Check if the itemdata pointer is still in use.
BOOL ClsBitmapMenu::IsItemInUse( ItemData *pData, HMENU hMenu /* = NULL */ )
{
	// Determine handle to use.
	HMENU hUseMenu = hMenu ? hMenu : m_hMenu;

	// Iterate the items.
	for ( int i = 0; i < ::GetMenuItemCount( hUseMenu ); i++ )
	{
		// Does this item have a submenu?
		HMENU hSubMenu = ::GetSubMenu( hUseMenu, i );
		if ( hSubMenu )
		{
			// Use recursion to check them aswell.
			if ( IsItemInUse( pData, hSubMenu ))
				return TRUE;
		}

		// Get item data.
		ClsMenuItemInfo mi;
		mi.fMask = MIIM_DATA;
		if ( ::GetMenuItemInfo( hUseMenu, i, TRUE, &mi ))
		{
			// Is this the item?
			if ( pData == ( ItemData * )mi.dwItemData )
				return TRUE;
		}
		else
			// If this call fails we do not risk marking the
			// data as not in use.
			return TRUE;
	}
	// Item not in use.
	return FALSE;
}

// Cleanup unused data pointers.
void ClsBitmapMenu::GarbageCollect()
{
	// Iterate item data array.
	int i = 0;
	while ( i < m_ItemArray.GetSize())
	{
		// Is this item still in use?
		if ( IsItemInUse( m_ItemArray[ i ] ) == FALSE )
		{
			// No. We remove the item from the
			// array, delete it and continu.
			delete m_ItemArray[ i ];
			m_ItemArray -= i;
		}
		else
			i++;
	}
	return;
}

void ClsBitmapMenu::OnReflectedInitMenuPopup( ClsWindow *pWindow, HMENU hPopup, int nIndex, BOOL bWindowMenu )
{
	// Make sure the menus are converted.
	if ( ! bWindowMenu && ! IsSystemMenu( hPopup )) ConvertMenusHelper( hPopup );
	ClsMenu::OnReflectedInitMenuPopup( pWindow, hPopup, nIndex, bWindowMenu );
}

void ClsBitmapMenu::OnReflectedExitMenuLoop( ClsWindow *pWindow, BOOL bShortcut )
{
	// Garbage collection.
	GarbageCollect();
	ClsMenu::OnReflectedExitMenuLoop( pWindow, bShortcut );
}

void ClsBitmapMenu::OnReflectedUnInitMenuPopup( ClsWindow *pWindow, HMENU hPopup, LPARAM lParam )
{
	// Garbage collection.
	GarbageCollect();
	ClsMenu::OnReflectedUnInitMenuPopup( pWindow, hPopup, lParam );
}