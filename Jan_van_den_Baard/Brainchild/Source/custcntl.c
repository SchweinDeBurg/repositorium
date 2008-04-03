/*
 *	custcntl.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	See "custcntl.h"
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	Data.
 */
LPCCSTYLE	gpccs;
DWORD		dwOldStyle;

/*
 *	We are requested to put up a dialog
 *	to edit the control styles.
 */
static BOOL CALLBACK ControlStyle( HWND hParent, LPCCSTYLE pccs )
{
	/*
	 *	No styles defined as of yet.
	 */
	return TRUE;
}

/*
 *	No supported.
 */
static INT CALLBACK ControlSizeToText( DWORD flStyle, DWORD flExtStyle, HFONT hFont, LPTSTR pszText )
{
	return -1;
}

/*
 *	Entry point for the dislog editor to find out more
 *	about our class.
 */
#ifdef UNICODE
UINT CALLBACK CustomControlInfoW( LPCCINFOW acci )
#else
UINT CALLBACK CustomControlInfoA( LPCCINFOA acci )
#endif
{
	/*
	 *	They want to know the number of
	 *	supported controls.
	 */
	if ( ! acci )
		return 1;

	/*
	 *	Fill the structure(s)
	 */
	acci[ 0 ].flOptions		= 0;
	acci[ 0 ].cxDefault		= 40;
	acci[ 0 ].cyDefault		= 15;
	acci[ 0 ].flStyleDefault	= WS_TABSTOP | WS_VSCROLL | WS_HSCROLL;
	acci[ 0 ].flExtStyleDefault	= 0;
	acci[ 0 ].cStyleFlags		= 0;
	acci[ 0 ].aStyleFlags		= NULL;
	acci[ 0 ].lpfnStyle		= ControlStyle;
	acci[ 0 ].lpfnSizeToText	= ControlSizeToText;
	acci[ 0 ].dwReserved1		= 0;
	acci[ 0 ].dwReserved2		= 0;
	acci[ 0 ].szTextDefault[ 0 ]	= 0;

	/*
	 *	Copy strings...
	 */
	_tcscpy( acci[ 0 ].szClass, BRAINCHILD_CLASS );
	_tcscpy( acci[ 0 ].szDesc, _T("Text Editor Class"));
	
	return 1;
}

#if defined(UNICODE)
#pragma comment(linker, "/export:CustomControlInfoW=_CustomControlInfoW@4")
#else
#pragma comment(linker, "/export:CustomControlInfoA=_CustomControlInfoA@4")
#endif   // UNICODE
