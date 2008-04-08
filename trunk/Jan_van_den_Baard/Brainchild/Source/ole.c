/*
 *	ole.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	These typcasts are merely to prevent
 *	compilation warnings.
 */
typedef ULONG ( STDMETHODCALLTYPE *IDataObject_AddRef )( LPDATAOBJECT );
typedef ULONG ( STDMETHODCALLTYPE *IDropSource_AddRef )( LPDROPSOURCE );
typedef ULONG ( STDMETHODCALLTYPE *IDropTarget_AddRef )( LPDROPTARGET );
typedef ULONG ( STDMETHODCALLTYPE *IDataObject_Release )( LPDATAOBJECT );
typedef ULONG ( STDMETHODCALLTYPE *IDropSource_Release )( LPDROPSOURCE );
typedef ULONG ( STDMETHODCALLTYPE *IDropTarget_Release )( LPDROPTARGET );
typedef HRESULT ( STDMETHODCALLTYPE *IDataObject_QueryInterface )( LPDATAOBJECT, REFIID, LPVOID * );
typedef HRESULT ( STDMETHODCALLTYPE *IDropSource_QueryInterface )( LPDROPSOURCE, REFIID, LPVOID * );
typedef HRESULT ( STDMETHODCALLTYPE *IDropTarget_QueryInterface )( LPDROPTARGET, REFIID, LPVOID * );

/*
 *	WM_SHOWWINDOW handler code.
 */
LRESULT OnShowWindow( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Is drag and drop enabled?
	 */
	if ( Parser->bDnDEnabled )
	{
		/*
		 *	Window visible?
		 */
		if ( wParam )
			/*
			 *	Register us as a drop-target.
			 */
			lpcd->bTargetRegistered = ( BOOL )( SUCCEEDED( RegisterDragDrop( hWnd, ( LPDROPTARGET )&lpcd->ifDropTarget )));
		else
		{
			/*
			 *	Where we registered?
			 */
			if ( lpcd->bTargetRegistered )
			{
				/*
				 *	Remove us as a drop target.
				 */
				RevokeDragDrop( hWnd );
	
				/*
				 *	No longer registered.
				 */
				lpcd->bTargetRegistered = FALSE;
			}
		}
	}
	return 0;
}

/*
 *	IUnknown interface.
 */
static ULONG STDMETHODCALLTYPE IUnknown_AddRef( LPUNKNOWN lpu )
{
	LPUINTERFACE	lpi = ( LPUINTERFACE )lpu;

	/*
	 *	Increase reference counter.
	 */
	lpi->nRefCount++;

	/*
	 *	Return reference count.
	 */
	return lpi->nRefCount;
}

static ULONG STDMETHODCALLTYPE IUnknown_Release( LPUNKNOWN lpu )
{
	LPUINTERFACE	lpi = ( LPUINTERFACE )lpu;
	
	/*
	 *	Decrease reference count.
	 */
	lpi->nRefCount--;

	/*
	 *	Return reference count.
	 */
	return lpi->nRefCount;
}

static HRESULT STDMETHODCALLTYPE IUnknown_QueryInterface( LPUNKNOWN lpu, REFIID riid, LPVOID *lplpvObject )
{
	LPUINTERFACE	lpi = ( LPUINTERFACE )lpu;
	
	/*
	 *	Clear storage pointer.
	 */
	*lplpvObject = NULL;

	/*
	 *	Interface queried supported?
	 */
	if ( IsEqualIID( riid, &IID_IUnknown    ) || 
	     IsEqualIID( riid, &IID_IDataObject ) ||
	     IsEqualIID( riid, &IID_IDropSource ) ||
	     IsEqualIID( riid, &IID_IDropTarget ))
	{
		/*
		 *	Yes. Add reference.
		 */
		IUnknown_AddRef( lpu );
		
		/*
		 *	Store object.
		 */
		*lplpvObject = ( LPVOID )lpi;
		return S_OK;
	}
	return E_NOINTERFACE;
}

/*
 *	IDataObject interface.
 */
static HRESULT STDMETHODCALLTYPE IDataObject_GetData( LPDATAOBJECT lpdo, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium )
{
	LPDOINTERFACE	lpi = ( LPDOINTERFACE )lpdo;
	LPCLASSDATA	lpcd = lpi->lpcd;
	SCODE		sc = S_OK;

	/*
	 *	Add reference.
	 */
	IUnknown_AddRef(( LPUNKNOWN )lpdo );

	/*
	 *	Clear STGMEDIUM structure.
	 */
	lpMedium->tymed		 = TYMED_NULL;
	lpMedium->pUnkForRelease = NULL;
	lpMedium->hGlobal	 = NULL;

	/*
	 *	Any markers?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Valid format?
		 */
		if ( lpFormatetc->cfFormat == CF_TEXT && ( lpFormatetc->tymed & TYMED_HGLOBAL ))
		{
			/*
			 *	Get the selection in a
			 *	global memory block.
			 */
			if (( lpMedium->hGlobal = GetTextGlobal( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd )) != NULL )
				/*
				 *	Setup the tymed.
				 */
				lpMedium->tymed = TYMED_HGLOBAL;
			else
				/*
				 *	Out of memory.
				 */
				sc = E_OUTOFMEMORY;
		}
		else
			/*
			 *	Unsupported format.
			 */
			sc = DATA_E_FORMATETC;
	}
	else
		/*
		 *	Nothing to get.
		 */
		sc = S_OK;

	/*
	 *	Release the object.
	 */
	IUnknown_Release(( LPUNKNOWN )lpdo );

	return sc;
}

static HRESULT STDMETHODCALLTYPE IDataObject_GetDataHere( LPDATAOBJECT lpdo, LPFORMATETC lpFormatetc, LPSTGMEDIUM lpMedium )
{
	/*
	 *	Not supported.
	 */
	return DATA_E_FORMATETC;
}

static LRESULT STDMETHODCALLTYPE IDataObject_QueryGetData( LPDATAOBJECT lpdo, LPFORMATETC lpFormatetc )
{
	SCODE	sc = DATA_E_FORMATETC;

	/*
	 *	Add object reference.
	 */
	IUnknown_AddRef(( LPUNKNOWN )lpdo );

	/*
	 *	Supported format queried?
	 */
	if ( lpFormatetc->cfFormat == CF_TEXT && ( lpFormatetc->tymed & TYMED_HGLOBAL ))
		/*
		 *	Set result.
		 */
		sc = S_OK;

	/*
	 *	Remove object reference.
	 */
	IUnknown_Release(( LPUNKNOWN )lpdo );

	return sc;
}

static LRESULT STDMETHODCALLTYPE IDataObject_EnumFormatEtc( LPDATAOBJECT lpdo, DWORD dwDirection, LPENUMFORMATETC *lplpEnumFormatetc )
{
	/*
	 *	We only support getting data
	 *	in this IDataObject interface.
	 */
	if (      dwDirection == DATADIR_GET ) return E_NOTIMPL;
	else if ( dwDirection == DATADIR_SET ) return E_NOTIMPL;
	else				       return E_INVALIDARG;
}

/*
 *	IDropSource interface.
 */
static LRESULT STDMETHODCALLTYPE IDropSource_QueryContinueDrag( LPDROPSOURCE lpds, BOOL bEsc, DWORD dwKeyState )
{
	/*
	 *	Escape key pressed?
	 */
	if ( bEsc )
		/*
		 *	Cancel dragging.
		 */
		return DRAGDROP_S_CANCEL;
	else if (( dwKeyState & MK_LBUTTON ) != MK_LBUTTON )
		/*
		 *	Left mouse button released means
		 *	drop the data.
		 */
		return DRAGDROP_S_DROP;

	return S_OK;
}

static LRESULT STDMETHODCALLTYPE IDropSource_GiveFeedback( LPDROPSOURCE lpds, DWORD dwEffect )
{
	/*
	 *	Let OLE setup the cursors.
	 */
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

/*
 *	IDropTarget interface.
 */
static MoveCaret( LPCLASSDATA lpcd )
{
	/*
	 *	Change caret position.
	 */
	SetCaretPos( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ) + (( GetCaretOffset( lpcd, lpcd->ptCaretPos.x ) - lpcd->ptViewPos.x ) * Parser->szCharSize.cx ), ( lpcd->ptCaretPos.y - lpcd->ptViewPos.y ) * Parser->szCharSize.cy );
}

static void ChangeCaret( LPCLASSDATA lpcd, BOOL bGray )
{
	int	cx, cy;

	/*
	 *	Setup normal caret size.
	 */
	cx = GetSystemMetrics( SM_CXBORDER );
	cy = Parser->szCharSize.cy;

	/*
	 *	What type of caret?
	 */
	if (      Parser->nCaretType == CARET_WIDE  ) cx *= 2;
	else if ( Parser->nCaretType == CARET_BLOCK ) cx = Parser->szCharSize.cx;

	/*
	 *	Create it solid or gray.
	 */
	CreateCaret( lpcd->hWnd, bGray ? ( HBITMAP )1 : NULL, cx, cy );

	/*
	 *	Move the caret.
	 */
	MoveCaret( lpcd );
}

static DWORD GetDropEffect( LPCLASSDATA lpcd, DWORD dwKeyState, DWORD dwSupported )
{
	DWORD	dwEffect = DROPEFFECT_MOVE;

	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return DROPEFFECT_NONE;

	/*
	 *	Ctrl pressed?
	 */
	if (( dwKeyState & MK_CONTROL ) == MK_CONTROL ) {
		/*
		 *	Link or copy.
		 */
		dwEffect = (( dwKeyState & MK_SHIFT ) == MK_SHIFT ) ? DROPEFFECT_LINK : DROPEFFECT_COPY;
	}

	/*
	 *	Is the effect supported.
	 */
	if (( dwSupported & dwEffect ) != dwEffect )
		dwEffect = dwSupported;

	return dwEffect;
}

static BOOL ValidData( LPDATAOBJECT lpdo )
{
	LPDOINTERFACE	lpo = ( LPDOINTERFACE )lpdo;
	FORMATETC	fmetc;

	/*
	 *	Setup FORMATETC structure.
	 */
	fmetc.cfFormat  = CF_TEXT;
	fmetc.ptd	= NULL;
	fmetc.dwAspect	= DVASPECT_CONTENT;
	fmetc.lindex	= -1;
	fmetc.tymed	= TYMED_HGLOBAL;
	
	/*
	 *	Requested format available?
	 */
	if ( lpo->lpVtbl->QueryGetData( lpdo, &fmetc ) == S_OK )
		return TRUE;

	return FALSE;
}

static LRESULT STDMETHODCALLTYPE IDropTarget_DragEnter( LPDROPTARGET lpdt, LPDATAOBJECT lpdo, DWORD dwKeyState, POINTL pt, DWORD *lpdwEffect )
{
	LPDTINTERFACE	lpi = ( LPDTINTERFACE )lpdt;
	LPCLASSDATA	lpcd = ( LPCLASSDATA )lpi->lpcd;
	POINT		point;
	DWORD		dwSupported = *lpdwEffect;
	SCODE		sc = E_INVALIDARG;

	/*
	 *	Add a reference to the object.
	 */
	IUnknown_AddRef(( LPUNKNOWN )lpdt );

	/*
	 *	Are we disabled or read-only?
	 */
	if ( ! IsWindowEnabled( lpcd->hWnd ))
	{
		/*
		 *	Yes. Release the object and
		 *	return an error.
		 */
		IUnknown_Release(( LPUNKNOWN )lpdt );
		return sc;
	}

	/*
	 *	Kill qualifiers.
	 */
	lpcd->cQualifier = 0;

	/*
	 *	Requested format available?
	 */
	if ( ValidData( lpdo ))
	{
		/*
		 *	Dragging...
		 */
		lpcd->bDragOver = TRUE;

		/*
		 *	Hide the caret if we have the
		 *	focus.
		 */
		if ( lpcd->bHasFocus )
			DisplayCaret( lpcd, FALSE );

		/*
		 *	Setup the gray caret.
		 */
		ChangeCaret( lpcd, TRUE );
	
		/*
		 *	Setup the caret location according
		 *	to the mouse position.
		 */
		point.x = pt.x - ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ));
		point.y = pt.y;
		ScreenToClient( lpcd->hWnd, &point );

		/*
		 *	Place the caret and update
		 *	it.
		 */
		MouseToCaret( lpcd, point.x, point.y, &lpcd->ptCaretPos );
		MoveCaret( lpcd );
		ShowCaret( lpcd->hWnd );

		/*
		 *	Setup effect.
		 */
		*lpdwEffect = GetDropEffect( lpcd, dwKeyState, dwSupported );

		sc = S_OK;
	}

	/*
	 *	Release reference.
	 */
	IUnknown_Release(( LPUNKNOWN )lpdt );

	return sc;
}

static void MoveView( LPCLASSDATA lpcd )
{
	POINT	ptView = lpcd->ptViewPos;
	POINT	ptMouse;
	RECT	rcClient;
	BOOL	bChange = FALSE;
	int	nLines = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;

	/*
	 *	Get client rectabgle.
	 */
	GetClientRect( lpcd->hWnd, &rcClient );

	/*
	 *	Get mouse position and convert
	 *	it to client coordinates.
	 */
	GetCursorPos( &ptMouse );
	ScreenToClient( lpcd->hWnd, &ptMouse );

	/*
	 *	Are we located Outside the client
	 *	area?
	 */
	if (      ptMouse.y < ( rcClient.top    + ( Parser->szCharSize.cy / 2 )))	ptView.y--;
	else if ( ptMouse.y > ( rcClient.bottom - ( Parser->szCharSize.cy / 2 ))) ptView.y++;

	/*
	 *	Validate.
	 */
	if ( ptView.y < 0 ) ptView.y = 0;
	else if (( ptView.y + lpcd->szViewSize.cy - 1 ) > nLines ) ptView.y = max( 0, nLines - lpcd->szViewSize.cy + 1 );

	/*
	 *	Are we located outside the
	 *	client area?
	 */
	if ( ptMouse.x < ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ) + ( Parser->szCharSize.cx / 2 ))) ptView.x--;
	else if ( lpcd->ptCaretPos.x < ptView.x ) ptView.x = lpcd->ptCaretPos.x;
	else if ( lpcd->ptCaretPos.x > ( ptView.x + lpcd->szViewSize.cx - 1 )) ptView.x++;

	/*
	 *	Validate.
	 */
	if ( ptView.x < 0 ) ptView.x = 0;
	else if ( ptView.x > ( lpcd->nLongestLine - lpcd->szViewSize.cx )) ptView.x = max( 0, lpcd->nLongestLine - lpcd->szViewSize.cx );
		
	/*
	 *	Did the column position change?
	 */
	if ( ptView.x != lpcd->ptViewPos.x )
	{
		/*
		 *	Re-render the view.
		 */
		InvalidateRect( lpcd->hWnd, NULL, FALSE );

		/*
		 *	Position change.
		 */
		bChange = TRUE;
	}
	else if ( ptView.y != lpcd->ptViewPos.y )
	{
		/*
		 *	Scroll.
		 */
		VScroll( lpcd, lpcd->ptViewPos.y - ptView.y );

		/*
		 *	Position change.
		 */
		bChange = TRUE;
	}

	/*
	 *	Update visuals.
	 */
	if ( bChange )
	{
		/*
		 *	Store changes.
		 */
		lpcd->ptViewPos = ptView;

		/*
		 *	Setup scrollers.
		 */
		SetupHScroller( lpcd );
		SetupVScroller( lpcd );
	}
}

static LRESULT STDMETHODCALLTYPE IDropTarget_DragOver( LPDROPTARGET lpdt, DWORD dwKeyState, POINTL pt, DWORD *lpdwEffect )
{
	LPDTINTERFACE	lpi = ( LPDTINTERFACE )lpdt;
	LPCLASSDATA	lpcd = ( LPCLASSDATA )lpi->lpcd;
	DWORD		dwSupported = *lpdwEffect;
	POINT		point;

	/*
	 *	Add reference.
	 */
	IUnknown_AddRef(( LPUNKNOWN )lpdt );

	/*
	 *	Setup the mouse location
	 */
	point.x = pt.x - ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ));
	point.y = pt.y;
	ScreenToClient( lpcd->hWnd, &point );
	
	/*
	 *	Move the caret as close as
	 *	possible to the mouse position.
	 */
	MouseToCaret( lpcd, point.x, point.y, &lpcd->ptCaretPos );
	MoveCaret( lpcd );
	
	/*
	 *	Setup effect.
	 */
	*lpdwEffect = GetDropEffect( lpcd, dwKeyState, dwSupported );

	/*
	 *	Scroll is necessary.
	 */
	MoveView( lpcd );

	/*
	 *	Remove reference.
	 */
	IUnknown_Release(( LPUNKNOWN )lpdt );

	return S_OK;
}

static LRESULT STDMETHODCALLTYPE IDropTarget_DragLeave( LPDROPTARGET lpdt )
{
	LPDTINTERFACE	lpi = ( LPDTINTERFACE )lpdt;
	LPCLASSDATA	lpcd = ( LPCLASSDATA )lpi->lpcd;

	/*
	 *	If we are read-only and we have a block
	 *	marker we clear it now.
	 */
	if ( ISREADONLY && HasMark( lpcd ))
		ClearMark( 0, lpcd );

	/*
	 *	Add reference.
	 */
	IUnknown_AddRef(( LPUNKNOWN )lpdt );

	/*
	 *	No longer dragging...
	 */
	lpcd->bDragOver = FALSE; 

	/*
	 *	Make the caret solid again.
	 */
	ChangeCaret( lpcd, FALSE );

	/*
	 *	Show the caret if we have
	 *	the focus.
	 */
	if ( lpcd->bHasFocus )
		DisplayCaret( lpcd, TRUE );

	/*
	 *	Remove reference.
	 */
	IUnknown_Release(( LPUNKNOWN )lpdt );

	return S_OK;
}

static HRESULT STDMETHODCALLTYPE IDropTarget_Drop( LPDROPTARGET lpdt, LPDATAOBJECT lpdo, DWORD dwKeyState, POINTL pt, DWORD *lpdwEffect )
{
	LPDTINTERFACE	lpi = ( LPDTINTERFACE )lpdt;
	LPDOINTERFACE	lpo = ( LPDOINTERFACE )lpdo;
	LPCLASSDATA	lpcd = lpi->lpcd;
	LPTSTR		lpString;
	FORMATETC	fmetc;
	STGMEDIUM	tgmed;
	POINT		ptStart, ptEnd;
	DWORD		dwSupported = *lpdwEffect, dwEffect;
	BOOL		bDeleted = FALSE;
	
	/*
	 *	Add object reference.
	 */
	IUnknown_AddRef(( LPUNKNOWN )lpdt );
	
	/*
	 *	Are we the source and we dropped
	 *	the selection on itself or are we
	 *	read-only.
	 */
	if (( MouseOnSelection( lpcd ) && lpcd->bDragSource ) || ISREADONLY )
		/*
		 *	Clear marker.
		 */
		ClearMark( 0, lpcd );		
	else
	{
		/*
		 *	Setup FORMATETC structure.
		 */
		fmetc.cfFormat  = CF_TEXT;
		fmetc.ptd	= NULL;
		fmetc.dwAspect	= DVASPECT_CONTENT;
		fmetc.lindex	= -1;
		fmetc.tymed	= TYMED_HGLOBAL;

		/*
		 *	Get effect.
		 */
		dwEffect = GetDropEffect( lpcd, dwKeyState, dwSupported );

		/*
		 *	Obtain the dragged data.
		 */
		if ( SUCCEEDED( lpo->lpVtbl->GetData( lpdo, &fmetc, &tgmed )))
		{
			/*
			 *	Lock down the global
			 *	memory.
			 */
			if (( lpString = GlobalLock( tgmed.hGlobal )) != NULL )
			{
				/*
				 *	Mark position.
				 */
				ptStart = lpcd->ptCaretPos;

				/*
				 *	Are we moving?
				 */
				if ( lpcd->bDragSource == TRUE && (( dwEffect & DROPEFFECT_MOVE ) == DROPEFFECT_MOVE ))
				{
					/*
					 *	Is the insert point located before
					 *	the moved block? 
					 *
					 *	If so we must delete
					 *	the moved selection before we insert
					 *	it at the new position. If we do not
					 *	do this the selection points will
					 *	become invalid.
					 */
					if ( lpcd->ptCaretPos.y < lpcd->ptSelStart.y || ( lpcd->ptCaretPos.y == lpcd->ptSelStart.y && lpcd->ptCaretPos.x < lpcd->ptSelStart.x ))
					{
						/*
						 *	Delete the current selection.
						 */
						DeleteText( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd, FALSE );

						/*
						 *	We have already deleted the
						 *	selection.
						 */
						bDeleted = TRUE;
					}
				}

				/*
				 *	Insert the text.
				 */
				if ( InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, lpString, &ptEnd, TRUE ))
				{
					/*
					 *	Move the caret.
					 */
					lpcd->ptCaretPos = ptEnd;

					/*
					 *	Are we the drag source?
					 */
					if ( lpcd->bDragSource == TRUE )
					{
						/*
						 *	Are we moving?
						 */
						if (( dwEffect & DROPEFFECT_MOVE ) == DROPEFFECT_MOVE )
						{
							/*
							 *	Invalidate the marks.
							 */
							ptStart.x = ptEnd.x = -1;
							ptStart.y = ptEnd.y = -1;

							/*
							 *	Did we delete already?
							 */
							if ( bDeleted == FALSE )
							{
								/*
								 *	Delete the current selection.
								 */
								DeleteText( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd, FALSE );
						
								/*
								 *	Move the caret.
								 */
								lpcd->ptCaretPos = lpcd->ptSelStart;
							}

						}
					}

					/*
					 *	Setup marks.
					 */
					lpcd->ptSelStart = ptStart;
					lpcd->ptSelEnd   = ptEnd;

					/*
					 *	Update column position.
					 */
					lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

					/*
					 *	Is the caret inside
					 *	the view?
					 */
					if ( CaretInView( lpcd ) == FALSE )
						/*
						 *	No. Move the view to
						 *	make it visible.
						 */
						MakeCaretVisibleNoRedraw( lpcd );

					/*
					 *	Re-render.
					 */
					InvalidateRect( lpcd->hWnd, NULL, FALSE );

					/*
					 *	Setup scrollers.
					 */
					SetupHScroller( lpcd );
					SetupVScroller( lpcd );

					/*
					 *	We are modified.
					 */
					SetModified( lpcd, TRUE );

					/*
					 *	Update the caret position.
					 */
					MoveCaret( lpcd );
				}
				/*
				 *	Unlock the memory.
				 */
				GlobalUnlock( tgmed.hGlobal );
			}
			/*
			 *	Release the storage medium.
			 */
			ReleaseStgMedium( &tgmed );
		}
	}

	/*
	 *	No longer dragging.
	 */
	lpcd->bDragOver = FALSE;

	/*
	 *	Make the caret solid again.
	 */
	ChangeCaret( lpcd, FALSE );

	/*
	 *	Show the caret if we have
	 *	the focus.
	 */
	if ( lpcd->bHasFocus )
		DisplayCaret( lpcd, TRUE );

	/*
	 *	Remove object reference.
	 */
	IUnknown_Release(( LPUNKNOWN )lpdt );

	return S_OK;
}

/*
 *	Setup Drag-and-drop interfaces.
 */
void SetupInterfaces( LPCLASSDATA lpcd )
{
	/*
	 *	Setup Vtbl's.
	 */
	lpcd->ifDataObject.lpVtbl = &lpcd->doVtbl;
	lpcd->ifDropSource.lpVtbl = &lpcd->dsVtbl;
	lpcd->ifDropTarget.lpVtbl = &lpcd->dtVtbl;

	/*
	 *	Setup the IDataObject interface. Only the following
	 *	entries will be valid:
	 *
	 *	AddRef()
	 *	Release()
	 *	QueryInterface()
	 *	GetData()
	 *	GetDataHere()
	 *	QueryGetData()
	 *	EnumFormatEtc()
	 */
	lpcd->doVtbl.AddRef		= ( IDataObject_AddRef  )IUnknown_AddRef;
	lpcd->doVtbl.Release		= ( IDataObject_Release )IUnknown_Release;
	lpcd->doVtbl.QueryInterface	= ( IDataObject_QueryInterface )IUnknown_QueryInterface;
	lpcd->doVtbl.GetData		= IDataObject_GetData;
	lpcd->doVtbl.GetDataHere	= IDataObject_GetDataHere;
	lpcd->doVtbl.QueryGetData	= IDataObject_QueryGetData;
	lpcd->doVtbl.EnumFormatEtc	= IDataObject_EnumFormatEtc;

	lpcd->ifDataObject.lpcd		= ( LPVOID )lpcd;
	lpcd->ifDataObject.nRefCount	= 0;

	/*
	 *	Setup the IDropSource interface.
	 */
	lpcd->dsVtbl.AddRef		= ( IDropSource_AddRef  )IUnknown_AddRef;
	lpcd->dsVtbl.Release		= ( IDropSource_Release )IUnknown_Release;
	lpcd->dsVtbl.QueryInterface	= ( IDropSource_QueryInterface )IUnknown_QueryInterface;
	lpcd->dsVtbl.QueryContinueDrag	= IDropSource_QueryContinueDrag;
	lpcd->dsVtbl.GiveFeedback	= IDropSource_GiveFeedback;

	lpcd->ifDropSource.lpcd		= ( LPVOID )lpcd;
	lpcd->ifDropSource.nRefCount	= 0;

	/*
	 *	Setup the IDropTarget interface.
	 */
	lpcd->dtVtbl.AddRef		= ( IDropTarget_AddRef  )IUnknown_AddRef;
	lpcd->dtVtbl.Release		= ( IDropTarget_Release )IUnknown_Release;
	lpcd->dtVtbl.QueryInterface	= ( IDropTarget_QueryInterface )IUnknown_QueryInterface;
	lpcd->dtVtbl.DragEnter		= IDropTarget_DragEnter;
	lpcd->dtVtbl.DragOver		= IDropTarget_DragOver;
	lpcd->dtVtbl.DragLeave		= IDropTarget_DragLeave;
	lpcd->dtVtbl.Drop		= IDropTarget_Drop;

	lpcd->ifDropTarget.lpcd		= ( LPVOID )lpcd;
	lpcd->ifDropTarget.nRefCount	= 0;
}
