#ifndef _GDIOBJECT_H_
#define _GDIOBJECT_H_
//
// gdiobject.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../collectors/linkedlist.h"

// A wrapper class for HGDIOBJECT handles. This class is
// ment to be a base class for other GDI object.
class ClsGdiObject : public ClsNode
{
	// No copy constructor.
	_NO_COPY( ClsGdiObject );
public:
	// Constructor.
	ClsGdiObject( HGDIOBJ hGdiObject = NULL, BOOL bDestroy = TRUE );

	// Destructor.
	virtual ~ClsGdiObject();

	// Interface.
	virtual void Delete();
	BOOL DeleteObject();
	void Attach( HGDIOBJ hGdiObject, BOOL bDestroy = TRUE );
	HGDIOBJ Detach();
	BOOL IsValid();
	static ClsGdiObject *FromHandle( HGDIOBJ hGdiObject );
	static void DeleteTempObjects();

	// Operator overload.
	operator HGDIOBJ() const;

protected:
	// Protected interface routines.
	int GetObject( int cbSize, LPVOID pStorage ) const;
	
	// Data
	HGDIOBJ	m_hGdiObject;
	BOOL	m_bDestroy;
};

#endif // _GDIOBJECT_H_
