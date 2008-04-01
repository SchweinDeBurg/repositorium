#ifndef _STRINGARRAY_H_
#define _STRINGARRAY_H_
//
// stringarray.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "array.h"

// A ClsArray derived class for ClsString objects
// and a sort function.
class ClsStringArray : public ClsArray< ClsString >
{
public:
	// A no-op constructor and destructor.
	ClsStringArray() {;}
	virtual ~ClsStringArray() {;}

	// A Sort() function which uses qsort() to
	// sort the array.
	void Sort()
	{
		typedef int ( __cdecl *COMPFUNC)( const void *, const void * );
		qsort(( void * )m_pElements, m_nSize, sizeof( ClsString ), ( COMPFUNC )CompareStrings );
	}
protected:
	// String comparisson routine.
	static int __cdecl CompareStrings( const ClsString *pStr1, const ClsString *pStr2 )
	{
		// Compare the strings.
		return pStr1->CompareNoCase( *pStr2 );
	}
};

#endif // _STRINGARRAY_H_