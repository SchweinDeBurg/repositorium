#ifndef _SIMPLEARRAY_H_
#define _SIMPLEARRAY_H_
//
// simplearray.h
//
// (C) Copyright 2002 Jan van den Baard
//     All Rights Reserved.
//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

// Dynamic array class for simple data types.
template< typename TYPE >
class ClsSimpleArray
{
public:
	ClsSimpleArray( int nGrowBy = 0 )
	{
		// Setup default values.
		m_nGrowBy   = nGrowBy;
		m_nMax      = m_nSize = 0;
		m_pElements = NULL;
	}

	virtual ~ClsSimpleArray()
	{
		// Remove all elements.
		SetSize( 0, -1 );
	}

	BOOL SetSize( int nSize, int nGrowBy )
	{
		_ASSERT( nSize >= 0 );

		// Do we have a new "grow-by" value?
		if ( nGrowBy != -1 )
			m_nGrowBy = nGrowBy;

		// Destroy all elements?
		if ( nSize == 0 )
		{
			// Do we have an array?
			if ( m_pElements )
			{
				// Free the memory and clear the pointer.
				delete [] ( LPBYTE )m_pElements;
				m_pElements = NULL;
			}

			// Reset values.
			m_nSize = m_nMax = 0;
		}
		else if ( nSize <= m_nMax )
		{
			// Zero new elements.
			if ( nSize > m_nSize )
				memset( &m_pElements[ m_nSize ], 0, nSize - m_nSize );

			// Set new upper bound.
			m_nSize = nSize;
		}
		else
		{
			TYPE   *pNewElem;
			int	nMaximum;

			// Get the growby value.
			nGrowBy = m_nGrowBy;

			// Valid?
			if ( nGrowBy == 0 )
			{
				// Grow the array. If no grow-by value is
				// present we use our own value.
				nGrowBy = m_nSize / 8;
				nGrowBy = ( nGrowBy < 4 ) ? 4 : ( nGrowBy > 1024 ? 1024 : nGrowBy );
			}

			// Will the requested size fit if we grow?
			if ( nSize > ( m_nMax + nGrowBy ))
				// Make sure we grow enough.
				nMaximum = nSize;
			else
				// This is enough.
				nMaximum = m_nMax + nGrowBy;

			// Allocate new elements.
			pNewElem = ( TYPE * )new BYTE[ nMaximum * sizeof( TYPE ) ];
			if ( pNewElem == NULL )
				return FALSE;

			// Copy old contents and free it.
			if ( m_pElements )
			{
				memcpy( pNewElem, m_pElements, m_nSize * sizeof( TYPE ));
				delete [] ( LPBYTE )m_pElements;
			}

			// Initialize new elements.
			memset( &pNewElem[ m_nSize ], 0, ( nSize - m_nSize ) * sizeof( TYPE ));

			// Setup new array.
			m_pElements = pNewElem;
			m_nSize	    = nSize;
			m_nMax	    = nMaximum;
		}
		return TRUE;
	}

	// Add one or more elements.
	int Add( TYPE *pElements, int nCount )
	{
		_ASSERT_VALID( pElements );
		_ASSERT( nCount > 0 );

		// Save current size.
		int	nIndex = m_nSize;

		// Grow the array to include the new elements
		if ( SetSize( nIndex + nCount, -1 ) == FALSE )
			return -1;

		// Copy the elements.
		memcpy( &m_pElements[ nIndex ], pElements, nCount * sizeof( TYPE ));

		// Return the index of the first element added.
		return nIndex;
	}

	// Add a single element.
	int Add( TYPE Element )
	{
		// Add the element.
		return Add( &Element, 1 );
	}

	// Insert element(s).
	int InsertAt( TYPE *pElements, int nIndex, int nCount )
	{
		_ASSERT_VALID( pElements );
		_ASSERT( nCount > 0 );
		_ASSERT( nIndex >= 0 );

		// Are we inserting at the end or
		// beyond the end of the array?
		if ( nIndex >= m_nSize )
		{
			// Adding after the end of the array.
			// Grow the array.
			if ( SetSize( nIndex + nCount, -1 ) == FALSE )
				return -1;
		}
		else
		{
			// Insert in the array.
			int nSize = m_nSize;

			// Grow the array.
			if ( SetSize( nSize + nCount, -1 ) == FALSE )
				return -1;

			// Move the data.
			memmove( &m_pElements[ nIndex + nCount ], &m_pElements[ nIndex ], ( nSize - nIndex ) * sizeof( TYPE ));
		}

		// Copy elements.
		memcpy( &m_pElements[ nIndex ], pElements, nCount * sizeof( TYPE ));
		return nIndex;
	}

	// Insert a single element.
	int InsertAt( TYPE Element, int nIndex )
	{
		// Insert the element.
		return InsertAt( &Element, nIndex, 1 );
	}

	// Insert array into this array.
	int InsertAt( const ClsSimpleArray< TYPE >& arSrc, int nIndex )
	{
		_ASSERT( &arSrc != this );

		// Insert elements into this array.
		if ( arSrc.m_nSize )
			return InsertAt( arSrc.m_pElements, nIndex, arSrc.m_nSize );
		return -1;
	}

	// Remove element(s).
	void RemoveAt( int nIndex, int nCount )
	{
		// Valid index?
		_ASSERT( nIndex >= 0 );
		_ASSERT( nCount > 0 );
		_ASSERT( nIndex + nCount <= m_nSize );

		// How many elements are to be
		// moved?
		int nMove = m_nSize - ( nIndex + nCount );

		// Move any elements?
		if ( nMove )
			memmove( &m_pElements[ nIndex ], &m_pElements[ nIndex + nCount ], nMove * sizeof( TYPE ));

		// nCount less elements.
		m_nSize -= nCount;
	}

	// Append array to this array.
	int Append( const ClsSimpleArray< TYPE >& arSrc )
	{
		_ASSERT( &arSrc != this );

		// Get current size as index.
		int	nIndex = m_nSize;

		// Grow the array.
		if ( SetSize( m_nSize + arSrc.m_nSize, -1 ) == FALSE )
			return -1;

		// Copy the elements.
		memcpy( &m_pElements[ nIndex ], arSrc.m_pElements, arSrc.m_nSize * sizeof( TYPE ));
		return nIndex;
	}

	// Copy array into this array.
	void Copy( const ClsSimpleArray< TYPE >& arSrc )
	{
		_ASSERT( &arSrc != this );

		// Setup the size to match.
		SetSize( arSrc.m_nSize, -1 );

		// Copy the elements.
		if ( arSrc.m_nSize )
			memcpy( m_pElements, arSrc.m_pElements, arSrc.m_nSize * sizeof( TYPE ));
	}

	// Remove all array elements.
	inline void RemoveAll()
	{
		SetSize( 0, -1 );
	}

	// Get the current array size.
	inline int GetSize() const
	{
		return m_nSize;
	}

	// Return a reference to the element at the given index.
	inline TYPE& GetAt( int nIndex ) const
	{
		_ASSERT( nIndex >= 0 && nIndex < m_nSize );
		return m_pElements[ nIndex ];
	}

	// Overloaded [] operator to the item at the given index.
	inline TYPE& operator[]( int nIndex ) const
	{
		return GetAt( nIndex );
	}

	// Overloaded += operator to add an array element.
	inline int operator+=( TYPE Element )
	{
		return Add( Element );
	}

	// Overloaded += operator to add an array.
	inline int operator+=( const ClsSimpleArray< TYPE >& arSrc )
	{
		return Append( arSrc );
	}

	// Overloaded -= operator to remove an element at the
	// given index.
	inline void operator-=( int nIndex )
	{
		RemoveAt( nIndex, 1 );
	}

	// Overloaded = operator to copy an array into this one.
	inline void operator=( const ClsSimpleArray< TYPE >& arSrc )
	{
		Copy( arSrc );
	}

protected:
	int		m_nSize;
	int		m_nMax;
	int		m_nGrowBy;
	TYPE	      *m_pElements;
};

// Some easy type-defs.
typedef ClsSimpleArray<UINT>	ClsUIntArray;
typedef ClsSimpleArray<int>	ClsIntArray;
typedef ClsSimpleArray<TCHAR>	ClsTCharArray;
typedef ClsSimpleArray<WORD>	ClsWordArray;
typedef ClsSimpleArray<DWORD>	ClsDWordArray;
typedef ClsSimpleArray<HANDLE>	ClsHandleArray;

#endif // _SIMPLEARRAY_H_