//
// gdiobject.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "gdiobject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Temporary and global GDI object map. Please note
// that objects in the temporary list will be destroyed
// after each processed message.
ClsLinkedList<ClsGdiObject>	global_gdi_objects;
ClsLinkedList<ClsGdiObject>	temporary_gdi_objects;

// A counter which is used to keep track of the
// GDI object handles created and destroyed.
#ifdef _DEBUG
int	nGDIObjects = 0;
#endif

// Finds a gdi object in the list by it's handle
// value.
static ClsGdiObject *ClsFindObjectByHandle( ClsLinkedList<ClsGdiObject>& list, HGDIOBJ hGdiObject )
{
	_ASSERT_VALID( hGdiObject ); // This must be valid.

	// Iterate the nodes.
	for ( ClsGdiObject *pGdiObject = list.GetFirst(); pGdiObject; pGdiObject = list.GetNext( pGdiObject ))
	{
		// Is the handle wrapped by this object
		// the one we are looking for?
		if ( *pGdiObject == hGdiObject )
			// Yes. Return a pointer to the object.
			return pGdiObject;
	}
	// Object not in the list.
	return NULL;
}

// Constructor. Initializes to the passed
// handle or NULL.
ClsGdiObject::ClsGdiObject( HGDIOBJ hGdiObject /* = NULL */, BOOL bDestroy /* = TRUE */ )
{
	// Setup the data.
	m_hGdiObject = hGdiObject;
	m_bDestroy   = bDestroy;

#ifdef _DEBUG
	// Increase object counter.
	if ( hGdiObject && bDestroy )
		nGDIObjects++;
#endif

	// Attach us to the global list.
	global_gdi_objects.AddHead( this );
}

// Destructor. Destroys the wrapped object
// if desired.
ClsGdiObject::~ClsGdiObject()
{
	// Remove us from the global list if we where
	// still linked into it.
	ClsGdiObject *pGdiObject;
	for ( pGdiObject = global_gdi_objects.GetFirst(); pGdiObject; pGdiObject = global_gdi_objects.GetNext( pGdiObject ))
	{
		// Is this us?
		if ( pGdiObject == this )
		{
			// Yes. remove us from the list.
			global_gdi_objects.Remove( this );
			break;
		}
	}

	// Destroy the object.
	Delete();
}

// Destroys the object if it is marked to be destroyed.
void ClsGdiObject::Delete()
{
	// Valid handle and may we
	// destroy it?
	if ( m_hGdiObject && m_bDestroy )
		DeleteObject();
	else
		// Remove the handle.
		m_hGdiObject = NULL;
}

// Destroy the object.
BOOL ClsGdiObject::DeleteObject()
{
	BOOL	bRC = FALSE;

	// Only destroy valid objects.
	if ( m_hGdiObject ) 
	{
		// Destroy the object.
		bRC = ::DeleteObject( m_hGdiObject );
#ifdef _DEBUG
		if ( bRC )
			// Decrease object counter.
			nGDIObjects--;
#endif
	}

	// Clear handle.
	m_hGdiObject = NULL;

	// Return result.
	return bRC;
}

// Attach a handle to the current object.
void ClsGdiObject::Attach( HGDIOBJ hGdiObject, BOOL bDestroy /* = TRUE */ )
{
	_ASSERT_VALID( hGdiObject ); // Passed handle must be valid.
	_ASSERT( hGdiObject != m_hGdiObject ); // Can't attach to ourselves.

	// Delete the current handle.
	Delete();

#ifdef _DEBUG
	// Increase object counter.
	if ( bDestroy )
		nGDIObjects++;
#endif

	// Setup the new handle.
	m_hGdiObject = hGdiObject;
	m_bDestroy   = bDestroy;
}

// Detach the wrapped handle.
HGDIOBJ ClsGdiObject::Detach()
{
	// Get result.
	HGDIOBJ hObject = m_hGdiObject;

	// Clear handle.
	m_hGdiObject = NULL;

#ifdef _DEBUG
	// Decrease object counter.
	if ( m_bDestroy && hObject )
		nGDIObjects--;
#endif

	// Return result.
	return hObject;
}

// Is the object valid?
BOOL ClsGdiObject::IsValid()
{
	return ( BOOL )( m_hGdiObject != NULL );
}

// Get object data.
int ClsGdiObject::GetObject( int cbSize, LPVOID pStorage ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Object must be valid.
	_ASSERT_VALID( pStorage ); // Storage pointer must be valid.

	return ::GetObject( m_hGdiObject, cbSize, pStorage );
}

// Create a temporary object.
ClsGdiObject *ClsGdiObject::FromHandle( HGDIOBJ hGdiObject )
{
	_ASSERT_VALID( hGdiObject ); // Must be valid.
	ClsGdiObject	*pGdiObject;

	// Do we have it in the global list?
	if (( pGdiObject = ClsFindObjectByHandle( global_gdi_objects, hGdiObject )) == NULL )
	{
		// No, see if we can find it in the temporary list.
		if (( pGdiObject = ClsFindObjectByHandle( temporary_gdi_objects, hGdiObject )) == NULL )
		{
			// No. Create a temporary new object
			// and add it to the temporary list.
			//
			// Objects in the temporary list will be deleted after
			// each processed message but the handle they wrap
			// will not be destroyed.
			pGdiObject = new ClsGdiObject( hGdiObject, FALSE );
			global_gdi_objects.Remove( pGdiObject );
			temporary_gdi_objects.AddHead( pGdiObject );
		}
	}
	return pGdiObject;
}

// Delete the temporary gdi object list.
void ClsGdiObject::DeleteTempObjects()
{
	ClsGdiObject *pGdiObject;

	// Remove all objects and delete them.
	while (( pGdiObject = temporary_gdi_objects.RemoveHead()) != NULL )
		delete pGdiObject;
}

// Operator overload. Returns
// the wrapped handle.
ClsGdiObject::operator HGDIOBJ() const
{
	// Return the handle.
	return m_hGdiObject;
}