//
//	toolbar.cpp
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//

#include "stdafx.h"

#include "bcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
EditToolbar::EditToolbar() 
{
}

// Destructor.
EditToolbar::~EditToolbar()
{
}

// Number of buttons.
#define NUMBUT	8

// Setup the toolbar.
void EditToolbar::SetupToolbar( BOOL bNewDel, BOOL bUpDown, BOOL bKey, BOOL bColor, BOOL bBgColor /*= FALSE*/ )
{
	_ASSERT_VALID( GetSafeHWND()); // Must be valid.
	TBBUTTON	Buttons[ NUMBUT ] =
	{
		{ 0, ETID_NEW,    TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON, { 0, 0 }, IDS_TOOL_NEW,     0 },
		{ 1, ETID_DELETE, TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON, { 0, 0 }, IDS_TOOL_DELETE,  0 },
		{ 2, ETID_UP,     TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON, { 0, 0 }, IDS_TOOL_UP,      0 },
		{ 3, ETID_DOWN,   TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON, { 0, 0 }, IDS_TOOL_DOWN,    0 },
		{ 4, ETID_EDIT,   TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON, { 0, 0 }, IDS_TOOL_EDIT,    0 },
		{ 5, ETID_INSERT, TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON, { 0, 0 }, IDS_TOOL_INSERT,  0 },
		{ 6, ETID_COLOR,  TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON, { 0, 0 }, IDS_TOOL_COLOR,   0 },
		{ 7, ETID_BGCOLOR,TBSTATE_ENABLED | TBSTATE_HIDDEN, TBSTYLE_BUTTON, { 0, 0 }, IDS_TOOL_BGCOLOR, 0 },
	};

	// Setup bitmaps.
	TBADDBITMAP	tab;
	tab.hInst	= ClsGetResourceHandle();
	tab.nID		= IDB_TOOLS;
	AddBitmap( NUMBUT, &tab );

	// Show buttons where necessary.
	if ( bNewDel )
	{
		Buttons[ 0 ].fsState &= ~TBSTATE_HIDDEN;
		Buttons[ 1 ].fsState &= ~TBSTATE_HIDDEN;
	}
	if ( bUpDown )
	{
		Buttons[ 2 ].fsState &= ~TBSTATE_HIDDEN;
		Buttons[ 3 ].fsState &= ~TBSTATE_HIDDEN;
	}
	if ( bKey )
	{
		Buttons[ 4 ].fsState &= ~TBSTATE_HIDDEN;
		Buttons[ 5 ].fsState &= ~TBSTATE_HIDDEN;
	}
	if ( bColor )
		Buttons[ 6 ].fsState &= ~TBSTATE_HIDDEN;
	if ( bBgColor )
		Buttons[ 7 ].fsState &= ~TBSTATE_HIDDEN;

	// Setup the toolbar control.
	ButtonStructSize( sizeof( TBBUTTON ));
	AddButtons( NUMBUT, Buttons );
}

// Obtain toolbar tip.
LRESULT EditToolbar::OnGetInfoTip( LPNMTBGETINFOTIP pTip, BOOL& bNotifyParent )
{
	// Find the right tip.
	for ( int i = 0; i < NUMBUT; i++ )
	{
		// Is this the one?
		TBBUTTONINFO tbi;
		tbi.cbSize = sizeof( tbi );
		tbi.dwMask = TBIF_LPARAM;
		GetButtonInfo( pTip->iItem, tbi );

		// Yes. Load the string and return a
		// pointer to it.
		::LoadString( ClsGetInstanceHandle(), tbi.lParam, m_chTip, 256 );
		pTip->pszText = m_chTip;
	}
	return 0;
}
