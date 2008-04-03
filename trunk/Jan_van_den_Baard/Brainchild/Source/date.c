/*
 *	date.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	Insert system time.
 */
void InsertTime( LPCLASSDATA lpcd )
{
	TCHAR		szBuffer[ 1024 ];

	/*
	 *	Read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Format output.
	 */
	if ( GetTimeFormat( LOCALE_USER_DEFAULT, Parser->pszTimeFormat ? 0 : LOCALE_NOUSEROVERRIDE, 0, Parser->pszTimeFormat, szBuffer, 1024 ) > 0 )
		/*
		 *	Insert the text.
		 */
		InsertCharacters( lpcd, szBuffer );
}

/*
 *	Insert system date.
 */
void InsertDate( LPCLASSDATA lpcd )
{
	TCHAR		szBuffer[ 1024 ];

	/*
	 *	Read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Format output.
	 */
	if ( GetDateFormat( LOCALE_USER_DEFAULT, Parser->pszDateFormat ? 0 : LOCALE_NOUSEROVERRIDE, 0, Parser->pszDateFormat, szBuffer, 1024 ) > 0 )
		/*
		 *	Insert the text.
		 */
		InsertCharacters( lpcd, szBuffer );
}
