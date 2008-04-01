#ifndef _HASH_H_
#define _HASH_H_
//
// hash.h
//
// (C) Copyright 2000-2001 Jan van den Baard.
//     All Rights Reserved.
//
#include "collector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

// Forward declaration.
class ClsHashEntry;

// Hash table class. The data this template class can handle
// _must_ be derived from the ClsHashEntry class defined
// above.
template< typename TYPE >
class ClsHashTable : public ClsCollector< TYPE >
{
public:
	// Construction and destruction.
	ClsHashTable() { m_nSize = 0; m_pTable = NULL; }
	virtual ~ClsHashTable()	{ DeconstructTable(); }

	// Free the current table.
	void DeconstructTable()
	{
		// Do we have a table?
		if ( m_nSize && m_pTable )
		{
			// Iterate the table.
			TYPE *pNext;
			for ( int i = 0; i < m_nSize; i++ )
			{
				// Iterate the current chain.
				for ( TYPE *pHash = m_pTable[ i ]; pHash; pHash = pNext )
				{
					// Get a pointer to the next hash in
					// the chain.
					pNext = ( TYPE * )pHash->m_pNext;

					// Delete the current hash.
					delete pHash;
				}
			}

			// Free the table memory.
			delete [] m_pTable;

			// Reset values.
			m_pTable = NULL;
			m_nSize  = 0;
		}
	}

	// Construct the table.
	void ConstructTable( int nSize )
	{
		_ASSERT( nSize >=4 && nSize <= 65535 );
		_ASSERT( m_nSize == 0 );
		
		// Allocate the table and clear it.
		m_pTable = ( TYPE ** )new BYTE[ nSize * sizeof( TYPE * ) ];
		memset( m_pTable, 0, nSize * sizeof( TYPE * ));

		// Setup the size.
		m_nSize = nSize;
	}

	// Add a hash entry to the table.
	BOOL AddEntry( TYPE& HashEntry )
	{ return AddEntry( &HashEntry ); }
	BOOL AddEntry( TYPE *pHashEntry )
	{
		_ASSERT_VALID( pHashEntry );
		_ASSERT( m_nSize != 0 );

		// Get the hash index.
		int nPos = OnGetHashIndex( pHashEntry );
		if ( nPos != -1 )
		{
			// The hash index must stay in range.
			_ASSERT( nPos >= 0 && nPos < m_nSize );

			// Get the first hash entry at this index.
			TYPE **pos;
			TYPE  *pEntry = *( pos = &m_pTable[ nPos & ( m_nSize - 1 ) ] );

			// Iterate the chain.
			while ( pEntry )
			{
				// Does this hash already exist?
				if ( OnCompareHashes( pEntry, pHashEntry ))
				{
					// return success.
					return TRUE;
				}

				// Next...
				pEntry = *( pos = ( TYPE ** )&pEntry->m_pNext );
			}

			// Copy the hash entry.
			TYPE *pNew = new TYPE;
			*pNew = *pHashEntry;

			// Link in the new hash.
			*pos = pNew;
			return TRUE;
		}
		return FALSE;
	}

	// Remove a hash from the hash table.
	BOOL RemoveEntry( TYPE& HashEntry )
	{ return RemoveEntry( &HashEntry ); }
	BOOL RemoveEntry( TYPE *pHashEntry )
	{
		_ASSERT_VALID( pHashEntry );
		_ASSERT( m_nSize != 0 );

		// Get hash index.
		int nPos = OnGetHashIndex( pHashEntry );
		if ( nPos != -1 )
		{
			// Get first entry in the chain.
			TYPE **pos;
			TYPE  *pEntry = *( pos = &m_pTable[ nPos & ( m_nSize - 1 ) ] );

			// Iterate the chain.
			while ( pEntry )
			{
				// Is this the one?
				if ( OnCompareHashes( pEntry, pHashEntry ))
				{
					// Save of next hash pointer.
					*pos = ( TYPE * )pEntry->m_pNext;

					// Delete the hash.
					delete pEntry;
					return TRUE;
				}

				// Get the next hash.
				pEntry = *( pos = ( TYPE ** )&pEntry->m_pNext );
			}
		}
		return FALSE;
	}

	// Find a specific entry. The input hash entry
	// only needs to contain the data on which the
	// hash index is computed and the data which is
	// used to compare two hashes.
	TYPE *FindEntry( TYPE& HashEntry ) const
	{ return FindEntry( &HashEntry ); }
	TYPE *FindEntry( TYPE *pHashEntry ) const
	{
		_ASSERT_VALID( pHashEntry );
		_ASSERT( m_nSize != 0 );

		// Get hash index.
		int nPos = OnGetHashIndex( pHashEntry );
		
		// Iterate this chain to find the command
		// we are looking for.
		if ( nPos != -1 )
		{
			// Get first entry in the chain.
			TYPE *p = m_pTable[ nIndex ];
			if ( p )
			{
				// Loop.
				do
				{
					// Is this the one?
					if ( OnCompareHashes( p, pHashEntry ))
						return p;

					// Get the next hash.
					p = ( TYPE * )p->m_pNext;
				}
				while ( p );
			}
		}
		return NULL;
	}

	// Obtain first entry.
	TYPE *GetFirstHash( int& nPos ) const
	{ 
		_ASSERT( m_nSize != 0 );

		// Return the first hash.
		for ( int i = 0; i < m_nSize; i++ )
		{
			// Is this a valid entry?
			if ( m_pTable[ i ] )
			{
				// Save index and return the
				// hash.
				nPos = i;
				return m_pTable[ i ];
			}
		}
		return NULL;
	}

	// Obtain the next hash entry.
	TYPE *GetNextHash( TYPE *pHash, int& nPos ) const
	{
		_ASSERT_VALID( pHash );
		_ASSERT( m_nSize != 0 );
		_ASSERT( nPos >= 0 && nPos < m_nSize );

		// Is there a next in the chain?
		if ( pHash->m_pNext )
			return ( TYPE * )pHash->m_pNext;

		// Continue iteration.
		nPos++;
		for ( int i = nPos; i < m_nSize; i++ )
		{
			// Is this a valid entry?
			if ( m_pTable[ i ] )
			{
				// Save index and return the
				// hash.
				nPos = i;
				return m_pTable[ i ];
			}
		}
		return NULL;
	}

	// Obtain table size.
	inline int GetTableSize() { return m_nSize; }

protected:
	// Overidables.
	virtual int OnGetHashIndex( TYPE *pHashEntry )
	{ return -1; }
	virtual BOOL OnCompareHashes( TYPE *pHash1, TYPE *pHash2 )
	{ return FALSE; }

	TYPE   **m_pTable;
	int	 m_nSize;
};

// Base class for all classes that wish to
// be added to a ClsHashTable.
class ClsHashEntry
{
public:
	ClsHashEntry() { m_pNext = NULL; }
	virtual ~ClsHashEntry() {;}

	// Link pointer to the next entry.
	ClsHashEntry	*m_pNext;
};

#endif // _HASH_H_