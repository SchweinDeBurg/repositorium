// ClipboardBackup.cpp: implementation of the CClipboardBackup class.
//
// Copyright 2006 (c) RegExLab.com
//
// Author: Ê·ÊÙÎ° (sswater shi)
//
// 2006/05/20 02:03:04
//

#include "stdafx.h"
#include "ClipboardBackup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CClipboardBackup::CClipboardBackup()
{
	m_bFilled = FALSE;
}

CClipboardBackup::~CClipboardBackup()
{
	if( m_bFilled )
	{
		POSITION pos = m_lstData.GetHeadPosition();
		while( pos != NULL )
		{
			ClipboardData & data = m_lstData.GetNext( pos );

			switch( data.m_nFormat )
			{
			case CF_ENHMETAFILE:
			case CF_DSPENHMETAFILE:
				::DeleteMetaFile((HMETAFILE)data.m_hData);
				break;

			default:
				::GlobalFree( data.m_hData );
			}
		}

		m_lstData.RemoveAll();
	}
}

BOOL CClipboardBackup::Backup()
{
	if( m_bFilled )
		return FALSE;

	if( ! ::OpenClipboard(NULL) )
		return FALSE;

	UINT format = 0;
	while( (format = ::EnumClipboardFormats(format)) != 0 )
	{
		ClipboardData data;
		data.m_nFormat = format;

		// skip some formats
		if( format == CF_BITMAP || format == CF_METAFILEPICT || format == CF_PALETTE || format == CF_OWNERDISPLAY ||
			format == CF_DSPMETAFILEPICT || format == CF_DSPBITMAP ||
			( format >= CF_PRIVATEFIRST && format <= CF_PRIVATELAST ) )
		{
			continue;
		}

		// get format name
		if( format <= 14 )
			data.m_szFormatName[0] = 0;
		else if( GetClipboardFormatName(format, data.m_szFormatName, 100) == 0 )
			data.m_szFormatName[0] = 0;

		// get handle
		HANDLE hMem = ::GetClipboardData( format );
		if( hMem == NULL )
			continue;

		// copy handle
		switch( format )
		{
		case CF_ENHMETAFILE:
		case CF_DSPENHMETAFILE:
			data.m_hData = ::CopyMetaFile((HMETAFILE)hMem, NULL);
			break;

		default:
			{
				int    size = ::GlobalSize(hMem);
				LPVOID pMem = ::GlobalLock( hMem );

				data.m_hData   = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, size );
				LPVOID pNewMem = ::GlobalLock( data.m_hData );

				memcpy(pNewMem, pMem, size);

				::GlobalUnlock(hMem);
				::GlobalUnlock(data.m_hData);
			}
		}

		m_lstData.AddTail(data);
	}

	::CloseClipboard();

	m_bFilled = TRUE;

	return TRUE;
}

BOOL CClipboardBackup::Restore()
{
	if( ! m_bFilled )
		return FALSE;

	if( ! ::OpenClipboard(NULL) )
		return FALSE;

	::EmptyClipboard();

	POSITION pos = m_lstData.GetHeadPosition();
	while( pos != NULL )
	{
		ClipboardData & data = m_lstData.GetNext( pos );

		UINT format = data.m_nFormat;

		if( data.m_szFormatName[0] != 0 )
		{
			UINT u = RegisterClipboardFormat( data.m_szFormatName );
			if( u > 0 ) format = u;
		}

		::SetClipboardData( format, data.m_hData );
	}

	::CloseClipboard();

	m_lstData.RemoveAll();
	m_bFilled = FALSE;

	return TRUE;
}
