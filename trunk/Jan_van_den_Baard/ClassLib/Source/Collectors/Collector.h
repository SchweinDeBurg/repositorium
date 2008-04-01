#ifndef _COLLECTOR_H_
#define _COLLECTOR_H_
//
// Collector.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"

#if _MSC_VER == 1200
	#ifdef new
		#undef new
		#define _REDEF_NEW
	#endif

	#ifndef _INC_NEW
		#include <new.h>
	#endif
#endif

// Base class for the collection template classes.
template <typename TYPE>
class ClsCollector
{
public:
	// Do-nothing constructor and destructor.
	ClsCollector() {;}
	virtual ~ClsCollector() {;}

protected:
	// Calls the constructor of the passed elements.
	inline void ConstructElements( TYPE *pElem, int nCount )
	{
		_ASSERT_VALID( pElem );
		_ASSERT( nCount >= 0 );

		// By default all fields are set to 0.
		memset(( void * )pElem, 0, nCount * sizeof( TYPE ));

#if _MSC_VER >= 1300
	#pragma push_macro("new")
	#undef new
#endif
		// Call the constructor for each element.
		while ( nCount-- )
			::new (( void * )pElem++ ) TYPE;
#if _MSC_VER >= 1300
	#pragma pop_macro("new")
#endif
	}

	// Calls the destructor of the passed elements.
	inline void DestructElements( TYPE *pElem, int nCount )
	{
		_ASSERT_VALID( pElem );
		_ASSERT( nCount >= 0 );

		// Call the destructor for each element.
		while ( nCount-- )
		{
			pElem->~TYPE();
			pElem++;
		}
	}

	// Copy the elements.
	inline void CopyElements( TYPE *pDest, const TYPE *pSrc, int nCount )
	{
		_ASSERT_VALID( pDest ); 
		_ASSERT_VALID( pSrc );
		_ASSERT( nCount >= 0 );

		// Copy each element. Note that classes should
		// overload the = operator when appropiate.
		while ( nCount-- )
			*pDest++ = *pSrc++;
	}
};

#if _MSC_VER == 1200
	#undef new
	#ifdef _REDEF_NEW
		#define new DEBUG_NEW
		#undef _REDEF_NEW
	#endif
#endif

#endif // _CONLLECTOR_H_