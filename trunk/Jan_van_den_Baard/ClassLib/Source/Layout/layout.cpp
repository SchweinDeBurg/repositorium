//
// layout.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "layout.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../exceptions/memoryexception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Scale a size.
#define ScaleWeight( s, f, w ) (( s * w ) / f )

// Special messages.
#define WM_ISLAYOUTENGINE	WM_APP+1 // Is it a layout engine?
#define WM_RELAYOUT		WM_APP+2 // Re-layout members.
#define WM_ORGANIZE		WM_APP+3 // Organize members.
#define WM_GETCONTROLS		WM_APP+4 // Append members.
#define WM_SHOWMEMBERS		WM_APP+5 // Show or hide members.
#define WM_ERRORS		WM_APP+6 // Any errors?
#define WM_REFRESH		WM_APP+7 // Refresh the group.

// Constructor. Set the defaults.
ClsLayoutEngine::ClsLayoutEngine()
{
	// Setup defaults.
	m_nSpacing		= 4;
	m_rcOffsets.Left()	= m_rcOffsets.Top() = m_rcOffsets.Right() = m_rcOffsets.Bottom() = 4;
	m_bIsHorizontal		= TRUE;
	m_bFrameGroup		= FALSE;
	m_dwFlags		= 0;
	m_nFontHeight		= 0;
}

// Destructor.
ClsLayoutEngine::~ClsLayoutEngine()
{
	// Destroy all members.
	ClsMember *pMember;
	while (( pMember = m_Members.RemoveHead()) != NULL )
		delete pMember;
}

// Scale the group.
void ClsLayoutEngine::ScaleGroup( UINT nEffSize )
{
	// Are we horizontal?
	DWORD dwBit = m_bIsHorizontal ? ClsMember::MF_FIXEDWIDTH : ClsMember::MF_FIXEDHEIGHT;

	// First loop to get all scaled sizes.
	ClsMember *pMember;
	UINT nTotalSize = 0, nSize;

	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Get minimum size.
		nSize = m_bIsHorizontal ? pMember->m_nMinWidth : pMember->m_nMinHeight;

		// Skip fixed sized objects.
		if (( pMember->m_dwFlags & dwBit ) != dwBit ) 
		{
			// Find out scaled size. This may
			// not be smaller than the object
			// it's minimum size.
			if (( pMember->m_nScaledSize = ScaleWeight( nEffSize, m_nFullWeight, pMember->m_nWeight )) < nSize )
				pMember->m_nScaledSize = nSize;

			// Keep track of the total size.
			nTotalSize += pMember->m_nScaledSize;
		}
	}

	// Constantly adjust the size of
	// objects which where scaled larger
	// than their minimum size when the
	// total scaled size was larger than
	// the area the objects must fit in.
	BOOL bAdjust = TRUE;
	while (( nTotalSize > nEffSize ) && bAdjust ) 
	{
		// Set adjust flag to false.
		bAdjust = FALSE;

		// Iterate members.
		for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
		{
			// Get minimum size.
			nSize = m_bIsHorizontal ? pMember->m_nMinWidth : pMember->m_nMinHeight;

			// Skip fixed size objects.
			if (( pMember->m_dwFlags & dwBit ) != dwBit ) 
			{
				// Was the size bigger than the
				// minimum size?
                                if ( pMember->m_nScaledSize > nSize ) 
				{
					// Yes. adjust.
					pMember->m_nScaledSize--;
					nTotalSize--;
					bAdjust = TRUE;

					// Are we done yet?
					if ( nTotalSize <= nEffSize )
						return;
				}
			}
		}
	}
}

// Resize and reposition the group frame.
void ClsLayoutEngine::ResizeGroupFrame()
{
	// Get window bounds.
	ClsRect	rc = GetWindowRect();

	// Adjust for the offsets.
	rc.Left()	+= m_rcOffsets.Left();
	rc.Top()	+= m_rcOffsets.Top();
	rc.Right()	-= m_rcOffsets.Right();
	rc.Bottom()	-= m_rcOffsets.Bottom();

	// Map the points to the client.
	GetParent()->ScreenToClient( rc );

	// Resize and Reposition the frame.
	m_Frame.MoveWindow( rc, FALSE );
}

// Scale a horizontal group.
void ClsLayoutEngine::ScaleHGroup()
{
	// Find out the group domain.
	ClsRect	rc = GetWindowRect();

        // Calculate offsets.
	UINT nLeftOff = rc.Left() + m_rcOffsets.Left();
        UINT nTopOff  = rc.Top()  + m_rcOffsets.Top();

	// Calculate effective width and height
	// in which the objects are scaled.
	UINT nEffWidth  = ( rc.Width()  - ((( m_Members.GetSize() - 1 ) * m_nSpacing ) + m_rcOffsets.Left() + m_rcOffsets.Right()));
	UINT nEffHeight = ( rc.Height() - ( m_rcOffsets.Top() + m_rcOffsets.Bottom()));

	// Decrease the effective width
	// with the total fixed width.
	nEffWidth -= m_nFixedSize;

	// Any framing?
	if ( m_bFrameGroup && m_Frame.GetSafeHWND())
	{
		// Reposition and resize the
		// frame object.
		ResizeGroupFrame();

		// Adjust offsets.
		nLeftOff += ::GetSystemMetrics( SM_CXEDGE ) + m_nSpacing;
		nTopOff  += m_nFontHeight + m_nSpacing;

		// And sizes.
		nEffWidth  -= ( ::GetSystemMetrics( SM_CXEDGE ) * 2 ) + ( m_nSpacing * 2 );
		nEffHeight -= ::GetSystemMetrics( SM_CYEDGE ) + m_nFontHeight + ( m_nSpacing * 2 );
	}

	// Scale objects.
	ScaleGroup( nEffWidth );

	// Iterate members.
	ClsMember *pMember;
	UINT nMemberWidth, nMemberHeight, nTotal = 0;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Calculate object size.
		if ( pMember->m_dwFlags & ClsMember::MF_FIXEDWIDTH ) nMemberWidth = pMember->m_nFixedWidth;
		else 
		{
			nMemberWidth = pMember->m_nScaledSize;
			nTotal += nMemberWidth;
		}
		nMemberHeight = ( pMember->m_dwFlags & ClsMember::MF_FIXEDHEIGHT ) ? pMember->m_nFixedHeight : nEffHeight;

		// Adjust when necessary. This may get a somewhat ugly
		// result in some cases.
		if ( pMember == m_Members.GetLast()) 
		{
			// Was the member scaled to large to fit into the area?
			if ( nTotal > nEffWidth ) nMemberWidth -= ( nTotal - nEffWidth );
			else
			{
				// If the member width is not fixed we stretch it to
				// fill the area.
				if ( ! ( pMember->m_dwFlags & ClsMember::MF_FIXEDWIDTH )) 
				{
					if ( nTotal < nEffWidth )
						nMemberWidth += ( nEffWidth - nTotal );
				}
			}
		}

		// Spacing member?
		if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING ) 
		{
			// Setup object position/dimensions.
			ClsRect points( nLeftOff, nTopOff, nLeftOff + nMemberWidth, nTopOff + nMemberHeight );

			// Extra lines?
			if ( pMember->m_nExtraLines ) 
			{
				if ( pMember->m_nLineSize == 0 )
				{
					// Compute the line size.
					ClsGetDC dc( pMember->m_pMember );
					ClsSelector sel( &dc, ( HFONT )pMember->m_pMember->SendMessage( WM_GETFONT )); 
					ClsSize sz = dc.GetTextExtent( _T( "W" ), 1 );
					pMember->m_nLineSize = sz.CY();
				}

				// OK?
				if ( pMember->m_nLineSize )
					// Add extra height.
					points.Bottom() += ( pMember->m_nExtraLines * pMember->m_nLineSize ) + 4;
			}

			// Convert the screen coordinates
			// to the target client.
			GetParent()->ScreenToClient( points );

			// Layout the member.
			pMember->m_pMember->MoveWindow( points, FALSE );

			// If the member is a layout engine
			// tell it to relayout it's members.
			if ( pMember->m_pMember->SendMessage( WM_ISLAYOUTENGINE ))
				pMember->m_pMember->SendMessage( WM_RELAYOUT );
		}

		// Adjust left offset.
		nLeftOff += ( nMemberWidth + m_nSpacing );
	}
}

// Scale a vertical group.
void ClsLayoutEngine::ScaleVGroup()
{
	// Find out the group domain.
	ClsRect rc = GetWindowRect();

	// Calculate offsets.
	UINT nLeftOff = rc.Left() + m_rcOffsets.Left();
	UINT nTopOff  = rc.Top()  + m_rcOffsets.Top();

	// Calculate the effective height
	// in which the objects are scaled.
	UINT nEffHeight = ( rc.Height() - ((( m_Members.GetSize() - 1 ) * m_nSpacing ) + m_rcOffsets.Top() + m_rcOffsets.Bottom()));
	UINT nEffWidth  = ( rc.Width()  - ( m_rcOffsets.Left() + m_rcOffsets.Right()));

	// Decrease the effective height with
	// the total fixed height.
	nEffHeight -= m_nFixedSize;

	// Any framing?
	if ( m_bFrameGroup && m_Frame.GetSafeHWND()) 
	{
		// Reposition and resize the
		// frame object.
		ResizeGroupFrame();

		// Adjust offsets.
		nLeftOff += ::GetSystemMetrics( SM_CXEDGE ) + m_nSpacing;
		nTopOff  += m_nFontHeight + m_nSpacing;

		// And sizes.
		nEffWidth  -= ( ::GetSystemMetrics( SM_CXEDGE ) * 2 ) + ( m_nSpacing * 2 );
		nEffHeight -= ::GetSystemMetrics( SM_CYEDGE ) + m_nFontHeight + ( m_nSpacing * 2 );
	}

        // Scale the objects.
        ScaleGroup( nEffHeight );

	// Iterate members.
	ClsMember *pMember;
	UINT nMemberWidth, nMemberHeight, nTotal = 0;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Calculate object dimensions.
                nMemberWidth = ( pMember->m_dwFlags & ClsMember::MF_FIXEDWIDTH  ) ? pMember->m_nFixedWidth  : nEffWidth;
		if ( pMember->m_dwFlags & ClsMember::MF_FIXEDHEIGHT ) nMemberHeight = pMember->m_nFixedHeight;
		else 
		{
                        nMemberHeight = pMember->m_nScaledSize;
                        nTotal += nMemberHeight;
                }

		// Adjust when necessary. This may get a somewhat ugly
		// result in some cases.
		if ( pMember == m_Members.GetLast()) 
		{
			// Was the member scaled to large?
			if ( nTotal > nEffHeight ) nMemberHeight -= ( nTotal - nEffHeight );
			else
			{
				// If the member does not have a fixed height we stretch
				// it to fill the area.
				if ( ! ( pMember->m_dwFlags & ClsMember::MF_FIXEDHEIGHT )) 
				{
					if ( nTotal < nEffHeight )
						nMemberHeight += ( nEffHeight - nTotal );
				}
			}
		}

		// Align (if necessary).
		UINT nLeftAdjust;
		if ( pMember->m_dwFlags & ClsMember::MF_ALIGN ) nLeftAdjust = max( nEffWidth - nMemberWidth, 0 );
		else						nLeftAdjust = 0;
		
		// Spacing member?
		if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING ) 
		{
			// Setup object.
			ClsRect points( nLeftOff + nLeftAdjust, nTopOff, nLeftOff + nLeftAdjust + nMemberWidth, nTopOff + nMemberHeight );

			// Extra lines?
			if ( pMember->m_nExtraLines ) 
			{
				if ( pMember->m_nLineSize == 0 )
				{
					// Compute the line size.
					ClsGetDC dc( pMember->m_pMember );
					ClsSelector sel( &dc, ( HFONT )pMember->m_pMember->SendMessage( WM_GETFONT )); 
					ClsSize sz = dc.GetTextExtent( _T( "W" ), 1 );
					pMember->m_nLineSize = sz.CY();
				}

				// OK?
				if ( pMember->m_nLineSize ) 
					// Add extra height.
					points.Bottom() += ( pMember->m_nExtraLines * pMember->m_nLineSize ) + 4;
			}

			// Convert the screen coordinates 
			// to the target client.
			GetParent()->ScreenToClient( points );

			// Layout the member.
			pMember->m_pMember->MoveWindow( points, FALSE );

			// If this member is a layout engine force
			// it to re-layout it's members.
			if ( pMember->m_pMember->SendMessage( WM_ISLAYOUTENGINE ))
				pMember->m_pMember->SendMessage( WM_RELAYOUT );
		}
	        
		// Increase top offset.
		nTopOff += ( nMemberHeight + m_nSpacing );
	}
}

// Pickup the largest minimum width and height.
BOOL ClsLayoutEngine::MaxMinSize()
{
	// Clear the values.
	m_nMaxMinWidth  = 0;
	m_nMaxMinHeight = 0;

	// Iterate the members.
	ClsMember *pMember;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Ask minimum dimensions.
		ClsSize szMinSize( 0, 0 );

		// Only on non-spacing members.
		if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING ) 
		{
			// Use the current control size?
			if ( pMember->m_dwFlags & ClsMember::MF_CONTROLSIZE )
			{
				// Do we already know the design size?
				if ( pMember->m_dwFlags & ClsMember::MF_GOTSIZE )
				{
					// Yes. Simple copy these values.
					szMinSize.CX() = pMember->m_nDesignWidth;
					szMinSize.CY() = pMember->m_nDesignHeight;
				}
				else
				{
					// Get the control size.
					ClsRect rc = pMember->m_pMember->GetWindowRect();
					szMinSize = rc.Size();

					// Save these values for later use.
					pMember->m_nDesignWidth  = szMinSize.CX();
					pMember->m_nDesignHeight = szMinSize.CY();

					// We got the values...
					pMember->m_dwFlags |= ClsMember::MF_GOTSIZE;
				}
			}
			// Call the OnGetMinSize() overidable.
			else if ( pMember->m_pMember->OnGetMinSize( szMinSize ) == FALSE )
				return FALSE;
		}

		// Setup values.
		pMember->m_nMinWidth  = szMinSize.CX();
		pMember->m_nMinHeight = szMinSize.CY();

		// Clear fixed sizes for non fixed size
		// members.
		if ( pMember->m_dwFlags & ClsMember::MF_ASKMIN )
			pMember->m_nFixedWidth = pMember->m_nFixedHeight = 0;

		// Skip spacing objects in this count.
		if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING ) 
		{
			// Make sure the fixed sizes are not
			// smaller than allowed.
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDWIDTH  ) 
			{
				if ( pMember->m_nMinWidth > pMember->m_nFixedWidth ) pMember->m_nFixedWidth = pMember->m_nMinWidth;
				else						     pMember->m_nMinWidth   = pMember->m_nFixedWidth;
			}
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDHEIGHT ) 
			{
				if ( pMember->m_nMinHeight > pMember->m_nFixedHeight ) pMember->m_nFixedHeight = pMember->m_nMinHeight;
				else						       pMember->m_nMinHeight   = pMember->m_nFixedHeight;
			}

			// Keep track of the largest minimum member
			// width and height.
			if ( pMember->m_nMinWidth  > m_nMaxMinWidth  ) m_nMaxMinWidth  = pMember->m_nMinWidth;
			if ( pMember->m_nMinHeight > m_nMaxMinHeight ) m_nMaxMinHeight = pMember->m_nMinHeight;
		}
	}
	// Success.
	return TRUE;
}

// Calculate the minumum size of a
// horizontal group.
BOOL ClsLayoutEngine::GroupHMinSize( ClsSize& szMinSize )
{
	// Reset values.
	m_nFullWeight = m_nFixedSize = 0;

	// Query minimum sizes.
	if ( MaxMinSize() == FALSE )
		return FALSE;

	// Scan through the members.
	ClsMember *pMember;
	UINT nWidth = 0, nHeight = 0;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Is this a spacing member?
		if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING ) 
		{
			// No. If this group has equal widths we set the
			// witdth of the member to the group width.
			if ( m_dwFlags & LF_EQUALWIDTH )
				pMember->m_nMinWidth = m_nMaxMinWidth;
		}

		// Does the member have fixed minimum dimensions?
		if (( pMember->m_dwFlags & ClsMember::MF_ASKMIN ) != ClsMember::MF_ASKMIN ) 
		{
			// No. Is it a fixed width object?
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDWIDTH )
				// Increase the group it's total fixed width.
				m_nFixedSize += pMember->m_nFixedWidth;
			else 
			{
				// Increase the groups width and the total
				// weight.
				nWidth	      += pMember->m_nMinWidth;
				m_nFullWeight += pMember->m_nWeight;
			}
		} 
		else 
		{
			// Is it a fixed width member?
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDWIDTH ) 
			{
				// Increase the total fixed width by the member
				// it's minimum width.
				m_nFixedSize	      += pMember->m_nMinWidth;
				pMember->m_nFixedWidth = pMember->m_nMinWidth;
			}
			else
			{
				// Increase the group width and the total weight.
				nWidth	      += pMember->m_nMinWidth;
				m_nFullWeight += pMember->m_nWeight;
			}
		}

		// Does the member have fixed minimum dimensions?
		if (( pMember->m_dwFlags & ClsMember::MF_ASKMIN ) != ClsMember::MF_ASKMIN ) 
		{
			// Is it a fixed height object?
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDHEIGHT ) 
			{
				// Store the member height when it is larger than
				// the height sofar.
				if ( pMember->m_nFixedHeight > nHeight )
					nHeight = pMember->m_nFixedHeight;
			} 
			else 
			{
				// Store the member minimum height when it is larger
				// than the height sofar.
				if ( pMember->m_nMinHeight > nHeight )
					nHeight = pMember->m_nMinHeight;
			}
		} 
		else 
		{
			// Is it a fixed height object?
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDHEIGHT )
				// Store the minimum height as the fixed height.
				pMember->m_nFixedHeight = pMember->m_nMinHeight;

			// Store the height when it is larger than
			// the height sofar.
			if ( pMember->m_nMinHeight > nHeight )
				nHeight = pMember->m_nMinHeight;
		}
	}

	// Add the offsets etc. to the sizes. The result will be the total
	// group minimum dimensions.
        nWidth  += m_nFixedSize + ( m_rcOffsets.Left() + m_rcOffsets.Right() + (( m_Members.GetSize() - 1 ) * m_nSpacing ));
        nHeight += ( m_rcOffsets.Top() + m_rcOffsets.Bottom());

	//	Adjust for the frame.
	if ( m_bFrameGroup && m_Frame.GetSafeHWND())
	{
		nWidth  += ( ::GetSystemMetrics( SM_CXEDGE ) * 2 ) + ( m_nSpacing * 2 );
		nHeight += ::GetSystemMetrics( SM_CYEDGE ) + ( m_nFontHeight + ( m_nSpacing * 2 ));
	}

	// Store the values.
	szMinSize.CX() = max( 1, nWidth );
        szMinSize.CY() = max( 1, nHeight );

	// Success...
	return TRUE;
}

// Calculate the minimum size of a
// vertical group.
BOOL ClsLayoutEngine::GroupVMinSize( ClsSize& szMinSize )
{
	// Reset values.
	m_nFixedSize = m_nFullWeight = 0;

	// Query minimum sizes.
	if ( MaxMinSize() == FALSE )
		return FALSE;

	// Iterate members.
	ClsMember *pMember;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Is it a spacing member?
		if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING ) 
		{
			// Is it a equal height group?
			if ( m_dwFlags & LF_EQUALHEIGHT )
				// Make the group it's minimum height the
				// member it's minimum height.
				pMember->m_nMinHeight = m_nMaxMinHeight;
		}
	}

	// Iterate members.
	UINT nWidth = 0, nHeight = 0;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Does the member have fixed minimum dimensions?
		if (( pMember->m_dwFlags & ClsMember::MF_ASKMIN ) != ClsMember::MF_ASKMIN) 
		{
			// Is it a fixed height member?
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDHEIGHT )
				// Increase the group it's fixed height.
				m_nFixedSize += pMember->m_nFixedHeight;
			else 
			{
				// Increase the group height and total weight.
				nHeight	      += pMember->m_nMinHeight;
				m_nFullWeight += pMember->m_nWeight;
			}
		} 
		else 
		{
			// Is it a fixed height member?
                        if ( pMember->m_dwFlags & ClsMember::MF_FIXEDHEIGHT ) 
			{
				// Make the fixed height the minimum height.
				m_nFixedSize	       += pMember->m_nMinHeight;
				pMember->m_nFixedHeight = pMember->m_nMinHeight;
			} 
			else 
			{
				// Increase the group height and total weight.
				nHeight	      += pMember->m_nMinHeight;
				m_nFullWeight += pMember->m_nWeight;
			}
		}

		// Does the member have fixed minimum dimensions?
                if (( pMember->m_dwFlags & ClsMember::MF_ASKMIN ) != ClsMember::MF_ASKMIN ) 
		{
			// Does it have a fixed width?
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDWIDTH ) 
			{
				// Store the width when it is larger than
				// the width sofar.
				if ( pMember->m_nFixedWidth > nWidth )
					nWidth = pMember->m_nFixedWidth;
			}
			else
			{
				// Store the width when it is larger than
				// the width sofar.
				if ( pMember->m_nMinWidth > nWidth )
					nWidth = pMember->m_nMinWidth;
			}
		}
		else
		{
			// Does it have a fixed width?
			if ( pMember->m_dwFlags & ClsMember::MF_FIXEDWIDTH )
				// Make the fixed width the minimum width.
				pMember->m_nFixedWidth = pMember->m_nMinWidth;

			// Store the width when it is larger than the width
			// sofar.
			if ( pMember->m_nMinWidth > nWidth )
				nWidth = pMember->m_nMinWidth;
		}
	}

	// Add the offsets etc. to the sizes. The result will be the total
	// group minimum dimensions.
	nHeight += m_nFixedSize + ( m_rcOffsets.Top() + m_rcOffsets.Bottom() + (( m_Members.GetSize() - 1 ) * m_nSpacing ));
	nWidth  += ( m_rcOffsets.Left() + m_rcOffsets.Right());

	// Adjust for the frame.
	if ( m_bFrameGroup && m_Frame.GetSafeHWND())
	{
		nWidth  += ( ::GetSystemMetrics( SM_CXEDGE )) * 2 + ( m_nSpacing * 2 );
		nHeight += ::GetSystemMetrics( SM_CYEDGE ) + ( m_nFontHeight + ( m_nSpacing * 2 ));
	}

	// Store the values.
	szMinSize.CX() = max( 1, nWidth  );
	szMinSize.CY() = max( 1, nHeight );

	// Success...
	return TRUE;
}

// Compute the group it's minimum dimensions.
BOOL ClsLayoutEngine::OnGetMinSize( ClsSize& szMinSize )
{
	BOOL	bRC;

	// Get the minimum width and height
	// of this group.
	if ( m_bIsHorizontal ) bRC = GroupHMinSize( szMinSize );
	else		       bRC = GroupVMinSize( szMinSize );

	// We have been dimensioned.
	if ( bRC )
		m_dwFlags |= LF_DIMENSIONED;

	// Return error flag.
	return bRC;
}

// Show or hide members.
void ClsLayoutEngine::ShowMembers( BOOL bShow /* = TRUE */ )
{
	_ASSERT_VALID( GetSafeHWND());

	// Call the show window message.
	SendMessage( WM_SHOWMEMBERS, bShow );
}

// Create the control.
BOOL ClsLayoutEngine::Create( ClsWindow& parent, DWORD dwTag, ... )
{
	// Tag/Data pairs...
	struct TagItem { DWORD dwTag; DWORD dwData; } *pArgs = ( TagItem * )&dwTag;
	LPCTSTR	pszName = NULL;
	BOOL bFrame = FALSE;

	// Evaluate arguments.
	while ( pArgs->dwTag != TAG_END )
	{
		switch ( pArgs->dwTag )
		{
			case	LAYOUT_Title:
				pszName = ( LPCTSTR )pArgs->dwData;
				break;

			case	LAYOUT_Horizontal:
				m_bIsHorizontal = ( BOOL )pArgs->dwData;
				break;

			case	LAYOUT_FrameGroup:
				bFrame = ( BOOL )pArgs->dwData;
				break;

			case	LAYOUT_Spacing:
				m_nSpacing = pArgs->dwData;
				break;

			case	LAYOUT_LeftOffset:
				m_rcOffsets.Left() = pArgs->dwData;
				break;

			case	LAYOUT_TopOffset:
				m_rcOffsets.Top() = pArgs->dwData;
				break;

			case	LAYOUT_RightOffset:
				m_rcOffsets.Right() = pArgs->dwData;
				break;

			case	LAYOUT_BottomOffset:
				m_rcOffsets.Bottom() = pArgs->dwData;
				break;

			case	LAYOUT_EqualMinWidth:
				if ( pArgs->dwData ) m_dwFlags |= LF_EQUALWIDTH;
				else		     m_dwFlags &= ~LF_EQUALWIDTH;
				break;

			case	LAYOUT_EqualMinHeight:
				if ( pArgs->dwData ) m_dwFlags |= LF_EQUALHEIGHT;
				else		     m_dwFlags &= ~LF_EQUALHEIGHT;
				break;

			case	LAYOUT_Master:
				if ( pArgs->dwData ) m_dwFlags |= LF_MASTER;
				else		     m_dwFlags &= ~LF_MASTER;
				break;
		}
		// Next...
		pArgs++;
	}
			
	// Create the control.
	if ( ClsWindow::Create( 0, NULL, pszName, WS_CHILD, ClsRect( 0, 0, 0, 0 ), parent, ( HMENU )0 ))
	{
		// Create frame?
		if ( bFrame )
			FrameGroup( bFrame );
		return TRUE;
	}
	return FALSE;
}

// Set or clear the group frame.
void ClsLayoutEngine::FrameGroup( BOOL bFrame /* = TRUE */ )
{
	// Did it change?
	if ( bFrame != m_bFrameGroup )
	{
		// Set the flag.
		m_bFrameGroup = bFrame;

		// Turn it on?
		if ( bFrame )
		{
			// Get the frame title.
			ClsString str( *this );

			// Create the frame control.
			if ( m_Frame.Create( GetParent(), ClsRect( 0, 0, 0, 0 ), 0, str, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | BS_GROUPBOX ))
			{
				// Change extended style.
				m_Frame.ModifyExStyle( 0, WS_EX_TRANSPARENT );

				// Setup it's font.
				if ( m_Font.IsValid())
					m_Frame.SetFont( &m_Font, FALSE );
			}
		}
		else
			m_Frame.Destroy();
	}
}

// Re-layout the group.
void ClsLayoutEngine::Relayout()
{
	// Are we the master?
	if ( m_dwFlags & LF_MASTER )
		SendMessage( WM_RELAYOUT );
}

// Refresh the group.
void ClsLayoutEngine::Refresh()
{
	// Are we the master?
	if ( m_dwFlags & LF_MASTER )
		SendMessage( WM_REFRESH );
}

// Add a member to the group.
BOOL ClsLayoutEngine::AddMember( ClsWindow *pLMember, ClsWindow *pPred, DWORD dwTag, ... )
{
	_ASSERT( pLMember != this ); // We can't be added to ourselves.
	_ASSERT( pPred != this ); // We can't be the predecessor.

	// Tag/Data pairs...
	struct TagItem { DWORD dwTag; DWORD dwData; } *pArgs = ( TagItem * )&dwTag;

	// Valid input?
	if ( pLMember->GetSafeHWND() == NULL )
	{
		// Set error flag.
		m_dwFlags |= LF_ADDERROR;
		return FALSE;
	}

	try
	{
		// Allocate a member structure.
		ClsMember *pMember = new ClsMember;

		// Setup fields.
		pMember->m_pMember = pLMember;
		pMember->m_nWeight = DEFAULT_WEIGHT;

		// Did we add a layout enigine with en error?
		if ( pLMember->SendMessage( WM_ISLAYOUTENGINE ))
		{
			// Set error bit if necessary...
			if ( pLMember->SendMessage( WM_ERRORS ))
				m_dwFlags |= LF_ADDERROR;
		}

		// Evaluate attributes.
		while ( pArgs->dwTag != TAG_END )
		{
			switch ( pArgs->dwTag )
			{
				case	ATTR_UseControlSize:
					if ( pArgs->dwData )
						pMember->m_dwFlags |= ClsMember::MF_CONTROLSIZE;
					break;

				case	ATTR_FixMinSize:
					if ( pArgs->dwData )
						pMember->m_dwFlags |= ClsMember::MF_ASKMIN | ClsMember::MF_FIXEDWIDTH | ClsMember::MF_FIXEDHEIGHT;
					break;

				case	ATTR_FixWidth:
					if ( pArgs->dwData )
					{
						pMember->m_dwFlags    &= ~ClsMember::MF_ASKMIN;
						pMember->m_dwFlags    |= ClsMember::MF_FIXEDWIDTH;
						pMember->m_nFixedWidth = pArgs->dwData;
					}
					break;

				case	ATTR_FixHeight:
					if ( pArgs->dwData ) 
					{
						pMember->m_dwFlags     &= ~ClsMember::MF_ASKMIN;
						pMember->m_dwFlags     |= ClsMember::MF_FIXEDHEIGHT;
						pMember->m_nFixedHeight = pArgs->dwData;
					}
					break;

				case	ATTR_Weight:
					if (( pMember->m_nWeight = pArgs->dwData ) == 0 )
						pMember->m_nWeight = DEFAULT_WEIGHT;
					break;

				case	ATTR_FixMinWidth:
					if ( pArgs->dwData )
						pMember->m_dwFlags |= ClsMember::MF_ASKMIN | ClsMember::MF_FIXEDWIDTH;
					break;

				case	ATTR_FixMinHeight:
					if ( pArgs->dwData )
						pMember->m_dwFlags |= ClsMember::MF_ASKMIN | ClsMember::MF_FIXEDHEIGHT;
					break;

				case	ATTR_RightAlign:
					if ( pArgs->dwData )
						pMember->m_dwFlags |= ClsMember::MF_ALIGN;
					break;

				case	ATTR_ExtraLines:
					pMember->m_nExtraLines = pArgs->dwData;
					break;
			}
			// Next...
			pArgs++;
		}

		// Predecessor?
		if ( pPred == WND_HEAD )
			// Add it to the member list.
			m_Members.AddHead( pMember );
		else if ( pPred )
		{
			// Look up the predecessor entry.
			ClsMember *p;
			for ( p = m_Members.GetFirst(); p; p = m_Members.GetNext( p ))
			{
				// Is this the one?
				if ( p->m_pMember == pPred )
				{
					m_Members.Insert( pMember, p );
					break;
				}
			}

			// Was it added?
			if ( p == NULL )
				// AddTail() it...
				m_Members.AddTail( pMember );
		}
		else
			// Add it to the member list.
			m_Members.AddTail( pMember );

		// No longer dimensioned/organized.
		m_dwFlags &= ~( LF_ORGANIZED | LF_DIMENSIONED );

		return TRUE;
	}
	catch( ClsMemoryException& me )
	{
		// Setup error flag.
		m_dwFlags |= LF_ADDERROR;
		me;
	}
	return FALSE;
}

// Add a spacing member.
BOOL ClsLayoutEngine::AddSpacingMember( UINT nWeight /* = DEFAULT_WEIGHT */ )
{
	try
	{
		// Allocate member node.
		ClsMember *pMember = new ClsMember;

		// Setup member weight.
		pMember->m_nWeight = nWeight ? nWeight : DEFAULT_WEIGHT;

		// Make us known as a spacing member.
		pMember->m_dwFlags |= ClsMember::MF_SPACING;

		// Add the member to the list.
		m_Members.AddTail( pMember );

		// No longer dimensioned/organized.
		m_dwFlags &= ~( LF_DIMENSIONED | LF_ORGANIZED );
		return TRUE;
	}
	catch( ClsMemoryException& me )
	{
		// Set the error flag.
		m_dwFlags |= LF_ADDERROR;
		me;
	}
	// Return failure.
	return FALSE;
}

// Remove a member from the list.
BOOL ClsLayoutEngine::RemMember( ClsWindow *pRemove )
{
	_ASSERT( pRemove != this ); // Can't be us.

	// Look for the member.
	ClsMember *pMember;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Is this the one?
		if ( pMember->m_pMember == pRemove ) 
		{
			// Yes. Remove the node.
			m_Members.Remove( pMember );

			// Free it.
			delete pMember;

			// Hide the window.
			pRemove->ShowWindow( SW_HIDE );

			// No longer dimensioned/organized.
			m_dwFlags &= ~( LF_DIMENSIONED | LF_ORGANIZED );

			// Return success.
			return TRUE;
		}
	}
	return FALSE;
}

// Get group controls.
void ClsLayoutEngine::GetControls( ClsLinkedList<ClsCNode> *pList )
{
	// Get member controls.
	ClsMember *pMember;
	for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
	{
		// Spacing member?
		if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING ) 
		{
			// Layout control?
			if ( pMember->m_pMember->SendMessage( WM_ISLAYOUTENGINE ))
				// Yes. Tell it to add it's members.
				pMember->m_pMember->SendMessage( WM_GETCONTROLS, 0, ( LPARAM )pList );
			else
			{
				// No. Simply add control.
				ClsCNode *pNode = new ClsCNode;
				pNode->m_hWnd = pMember->m_pMember->GetSafeHWND();
				pList->AddTail( pNode );
			}
		}
	}
}

// Organize the members. This will re-organize the
// controls Z-order so that the tab-order goes from
// top-left to right bottom.
void ClsLayoutEngine::OrganizeMembers()
{
	// Storage for the members.
	ClsLinkedList<ClsCNode> list;

	// Are we already organized?
	if (( m_dwFlags & LF_ORGANIZED ) != LF_ORGANIZED )
	{
		// No. Get all member controls.
		GetControls( &list );

		// Organize the controls.
		ClsCNode *pNode;
		HWND hPrev = NULL;
		for ( pNode = list.GetFirst(); pNode; pNode = list.GetNext( pNode )) 
		{
			// Setup Z order.
			::SetWindowPos( pNode->m_hWnd, hPrev ? hPrev : HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );

			// Save predecessor.
			hPrev = pNode->m_hWnd;
		}

		// Free nodes.
		while (( pNode = list.RemoveHead()) != NULL )
			delete pNode;
			
		// We are organized.
		m_dwFlags |= LF_ORGANIZED;
	}
}

// Window procedure override. Handles layout engine private messages.
LRESULT ClsLayoutEngine::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Interpred message.
	switch ( uMsg )
	{
		case	WM_CREATE:
		{
			// Get the default font height used
			// by the layout control. This information
			// is needed if the control font is not
			// set.
			ClsGetDC dc( this );
			ClsSize sz = dc.GetTextExtent( _T( "W" ), 1 );
			m_nFontHeight = sz.CY();
			break;
		}

		case	WM_GETFONT:
			// Valid font?
			return ( LRESULT )( m_Font.IsValid() ? ( HFONT )m_Font : NULL );

		case	WM_SETFONT:
		{
			// Detach old font.
			m_Font.Detach();

			// Valid?
			if ( wParam )
			{
				// Attach the new font.
				m_Font.Attach(( HFONT )wParam, FALSE );

				// Set the font to the frame
				// if it exists.
				if ( m_Frame.GetSafeHWND())
					m_Frame.SetFont( &m_Font, FALSE );

				// Pass this message to all our members
				// when they do not have a font set.
				ClsMember *pMember;
				for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember ))
				{
					// Skip spacing members.
					if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING )
					{
						// Does it have a font?
						if ( pMember->m_pMember->SendMessage( WM_GETFONT ) == NULL )
							// No. Set it.
							pMember->m_pMember->SendMessage( WM_SETFONT, wParam, MAKELPARAM( FALSE, 0 ));
					}
				}

				// Pickup the font height.
				ClsGetDC dc(this);
				ClsSelector sel( &dc, ( HFONT )wParam );
				ClsSize sz = dc.GetTextExtent( _T( "W" ), 1 );

				// Store the height.
				m_nFontHeight = sz.CY();

				m_dwFlags &= ~LF_DIMENSIONED;
			}
			break;
		}

		case	WM_ISLAYOUTENGINE:
			// Yes, we really are a layout engine...
			return TRUE;

		case	WM_ORGANIZE:
			// Organize the members.
			m_dwFlags &= ~LF_ORGANIZED;
			OrganizeMembers();
			return TRUE;

		case	WM_GETCONTROLS:
			// Add members to the passed list.
			GetControls(( ClsLinkedList<ClsCNode> * )lParam );
			return TRUE;

		case	WM_ERRORS:
			// Return error bool.
			return ( LRESULT )(( BOOL )( m_dwFlags & LF_ADDERROR ));

		//case	WM_SIZE:
			// Resize the frame.
		//	if ( m_bFrameGroup && m_Frame.GetSafeHWND())
		//	{
		//		ClsRect rc;
		//		GetClientRect( rc );
		//		m_Frame.MoveWindow( rc, FALSE );
		//	}
		//	return 0;

		case	WM_RELAYOUT:
			// Group dimensioned? When ever a group contents
			// has changed we need to call the OnGetMinSize()
			// method before we re-layout. This is to make sure
			// the layout engine always works with the latest
			// information.
			if (( m_dwFlags & LF_DIMENSIONED ) != LF_DIMENSIONED ) 
			{
				ClsSize szMinSize( 0, 0 );

				// OK?
				if ( OnGetMinSize( szMinSize ) == FALSE )
					return FALSE;
			}

			// Is this the master group?
			if ( m_dwFlags & LF_MASTER ) 
			{
				// Organized?
				if (( m_dwFlags & LF_ORGANIZED ) != LF_ORGANIZED )
					OrganizeMembers();

				// Get the parent it's client rectangle.
				ClsRect rc = GetParent()->GetClientRect();

				// Re-size the group.
				MoveWindow( rc, FALSE );
			}

			// Scale the group members.
			if ( m_bIsHorizontal ) ScaleHGroup();
			else		       ScaleVGroup();

			// Master group?
			if ((( m_dwFlags & LF_MASTER ) == LF_MASTER ) && GetParent()->IsWindowVisible())
				// Force the parent to redraw.
				GetParent()->RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN );
			return TRUE;

		case	WM_SHOWMEMBERS:
		{
			// Show/Hide members.
			ClsMember *pMember;
			for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
			{
				// A spacing member?
				if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING )
				{
					if ( pMember->m_pMember->SendMessage( WM_ISLAYOUTENGINE )) pMember->m_pMember->SendMessage( WM_SHOWMEMBERS, wParam );
					else
					{
						pMember->m_pMember->ShowWindow( wParam ? SW_SHOW : SW_HIDE );
						if ( wParam ) pMember->m_pMember->UpdateWindow();
					}
				}
			}

			// Show/Hide the frame.
			if ( m_bFrameGroup && m_Frame.GetSafeHWND())
			{
				m_Frame.ShowWindow( wParam ? SW_SHOW : SW_HIDE );
				if ( wParam ) m_Frame.UpdateWindow();
			}
			return TRUE;
		}

		case	WM_REFRESH:
		{
			// Do longer dimensioned or organized...
			m_dwFlags &= ~( LF_ORGANIZED | LF_DIMENSIONED );

			// Refresh layout engine members.
			ClsMember *pMember;
			for ( pMember = m_Members.GetFirst(); pMember; pMember = m_Members.GetNext( pMember )) 
			{
				// A spacing member?
				if (( pMember->m_dwFlags & ClsMember::MF_SPACING ) != ClsMember::MF_SPACING )
				{
					// Is it a layout engine?
					if ( pMember->m_pMember->SendMessage( WM_ISLAYOUTENGINE )) 
						pMember->m_pMember->SendMessage( WM_REFRESH );
				}
			}

			// Are we the master?
			if ( m_dwFlags & LF_MASTER )
				// Layout the group.
				SendMessage( WM_RELAYOUT );
			return TRUE;
		}
	}

	// Call the base class.
	return ClsWindow::WindowProc( uMsg, wParam, lParam );
}