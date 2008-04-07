/*
 *	nofrag.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Memory allocation routines which prevent fragmentation.
 *
 */

#include "stdafx.h"

#include "nofrag.h"
#include "list.h"

/*
 *	Internal structures. Most are simply type-casted
 *	LIST and NODE structures.
 */
typedef struct memBlock		*LPMEMBLOCK;
typedef struct memBlock
{
	LPMEMBLOCK		lpbNext;
	LPMEMBLOCK		lpbPrev;
	DWORD			dwBytesUsed;
} MEMBLOCK;

typedef struct memItem		*LPMEMITEM;
typedef struct memItem
{
	LPMEMITEM		lpiNext;
	LPMEMITEM		lpiPrev;
	LPMEMBLOCK		lpbBlock;
	DWORD			dwSize;
} MEMITEM;

typedef struct sysItem		*LPSYSITEM;
typedef struct sysItem
{
	LPSYSITEM		lpsNext;
	LPSYSITEM		lpsPrev;
	LPVOID			lpvMemPtr;
	DWORD			dwSize;
} SYSITEM;

typedef struct blockList	*LPBLOCKLIST;
typedef struct blockList
{
	LPMEMBLOCK		lpbFirst;
	LPMEMBLOCK		lpbEnd;
	LPMEMBLOCK		lpbLast;
} BLOCKLIST;

typedef struct itemList		*LPITEMLIST;
typedef struct itemList
{
	LPMEMITEM		lpiFirst;
	LPMEMITEM		lpiEnd;
	LPMEMITEM		lpiLast;
} ITEMLIST;

typedef struct sysList		*LPSYSLIST;
typedef struct sysList
{
	LPSYSITEM		lpsFirst;
	LPSYSITEM		lpsEnd;
	LPSYSITEM		lpsLast;
} SYSLIST;

typedef struct memPool		*LPMEMPOOL;
typedef struct memPool
{
	BLOCKLIST		blBlocks;
	ITEMLIST		ilItems;
	SYSLIST			slSysMem;
	DWORD			dwBlockSize;
} MEMPOOL;

/*
 *	Macro to round up all allocation
 *	sizes to the nearest possible
 *	multiple of 8.
 */
#define ROUND_ALLOCATION_SIZE( s )	( DWORD )((( int )s + 7 ) & -8 )

/*
 *	Easy type-cast.
 */
#define MP( p )				(( LPMEMPOOL )p )

/*
 *	Free a block of memory. It removes all
 *	free items from the pools it's free
 *	item list. When do the block is freed.
 */
static void FreeBlock( LPMEMPOOL lpMemPool, LPMEMBLOCK lpMemBlock )
{
	LPMEMITEM	lpMemItem, lpNextItem;

	/*
	 *	Iterate the free items list and remove all
	 *	items belonging to the block.
	 */
	lpMemItem = lpMemPool->ilItems.lpiFirst;

	while ( lpMemItem->lpiNext )
	{
		/*
		 *	Is this item located in the block?
		 */
		if ( lpMemBlock == lpMemItem->lpbBlock )
		{
			/*
			 *	Pick up a pointer to the next item.
			 */
			lpNextItem = lpMemItem->lpiNext;

			/*
			 *	Remove the item from the list.
			 */
			Remove(( LPNODE )lpMemItem );

			/*
			 *	Continue at the next item.
			 */
			lpMemItem = lpNextItem;
		}
		else
			/*
			 *	Next...
			 */
			lpMemItem = lpMemItem->lpiNext;
	}

	/*
	 *	Remove the block from the list
	 *	and free the memory it used.
	 */
	Remove(( LPNODE )lpMemBlock );
	free( lpMemBlock );
}

/*
 *	Allocate a block of memory and add it to the
 *	list.
 */
static LPMEMBLOCK AllocBlock( LPMEMPOOL lpMemPool )
{
	LPMEMBLOCK	lpMemBlock;
	LPMEMITEM	lpMemItem;

	/*
	 *	Allocate the necessary memory.
	 */
	if (( lpMemBlock = malloc( sizeof( MEMBLOCK ) + lpMemPool->dwBlockSize )) != NULL )
	{
		/*
		 *	Initialize block structure.
		 */
		lpMemBlock->dwBytesUsed = 0L;

		/*
		 *	Add the memory block to the block list.
		 */
		AddHead(( LPLIST )&lpMemPool->blBlocks, ( LPNODE )lpMemBlock );

		/*
		 *	Get a pointer to the block it's
		 *	memory and hang it in the free
		 *	items list.
		 */
		lpMemItem = ( LPMEMITEM )( lpMemBlock + 1 );

		/*
		 *	Initialize the item.
		 */
		lpMemItem->lpbBlock = lpMemBlock;
		lpMemItem->dwSize   = lpMemPool->dwBlockSize;

		/*
		 *	Add it to the free items list.
		 */
		AddHead(( LPLIST )&lpMemPool->ilItems, ( LPNODE )lpMemItem );
	}

	/*
	 *	Return a pointer to the block
	 *	or NULL in case of an error.
	 */
	return lpMemBlock;
}

/*
 *	Optimizes a memory block. It will add all free items from the
 *	specific block and to a seperate list. The it will lookup all items
 *	which are located directly after eachother. All these items will
 *	be joined into one larger item.
 *
 *	This will prevent fragmentizing of the memory pool itself.
 */
static void OptimizeBlock( LPMEMPOOL lpMemPool, LPMEMBLOCK lpMemBlock )
{
	ITEMLIST	itemList;
	LPMEMITEM	lpMemItem1, lpMemItem2, lpMemItem3, lpNextItem;

	/*
	 *	Initialize buffer list.
	 */
	NewList(( LPLIST )&itemList );

	/*
	 *	Iterate the items.
	 */
	lpMemItem1 = lpMemPool->ilItems.lpiFirst;

	while ( lpMemItem1->lpiNext )
	{
		/*
		 *	Is this one from the block?
		 */
		if ( lpMemItem1->lpbBlock == lpMemBlock )
		{
			/*
			 *	Get a pointer to the next item.
			 */
			lpNextItem = lpMemItem1->lpiNext;

			/*
			 *	Remove it from the free list and
			 *	add it to the buffer list.
			 */
			Remove(( LPNODE )lpMemItem1 );
			AddHead(( LPLIST )&itemList, ( LPNODE )lpMemItem1 );

			/*
			 *	Continue...
			 */
			lpMemItem1 = lpNextItem;
		}
		else
			/*
			 *	Next...
			 */
			lpMemItem1 = lpMemItem1->lpiNext;
	}

	/*
	 *	Iterate the buffer list.
	 */
	for ( lpMemItem1 = itemList.lpiFirst; lpMemItem1->lpiNext; lpMemItem1 = lpMemItem1->lpiNext )
	{
		/*
		 *	Get a pointer to the memory item directly
		 *	following the current one.
		 */
		lpMemItem2 = ( LPMEMITEM )((( char * )lpMemItem1 ) + lpMemItem1->dwSize );

		/*
		 *	Iterate the buffer list.
		 */
		lpMemItem3 = itemList.lpiFirst;

		while ( lpMemItem3->lpiNext )
		{
			/*
			 *	When these two are the same it means that
			 *	these are two items located directly after
			 *	eachother. They can be merged into one,
			 *	larger item.
			 */
			if ( lpMemItem2 == lpMemItem3 )
			{
				/*
				 *	Get a pointer to the next item.
				 */
				lpNextItem = lpMemItem3->lpiNext;

				/*
				 *	Merge the two items.
				 */
				lpMemItem1->dwSize += lpMemItem3->dwSize;

				/*
				 *	Remove the merged item.
				 */
				Remove(( LPNODE )lpMemItem3 );

				/*
				 *	Get a pointer to the next memory item
				 */
				lpMemItem2 = ( LPMEMITEM )((( char * )lpMemItem1 ) + lpMemItem1->dwSize );

				/*
				 *	Continue...
				 */
				lpMemItem3 = lpNextItem;
			}
			else
				/*
				 *	Next...
				 */
				lpMemItem3 = lpMemItem3->lpiNext;
		}
	}

	/*
	 *	Put the optimized free list back.
	 */
	while (( lpMemItem1 = ( LPMEMITEM )RemHead(( LPLIST )&itemList )) != NULL )
		AddHead(( LPLIST )&lpMemPool->ilItems, ( LPNODE )lpMemItem1 );
}

/*
 *	Find a suitable free item.
 *
 *	The free items list should be sorted so that the smallest
 *	free item is located at the start of the list and the
 *	largest at the end to make more efficient use of the
 *	items.
 */
static LPMEMITEM FindSpace( LPMEMPOOL lpMemPool, DWORD dwSize )
{
	LPMEMITEM	lpMemItem;

	/*
	 *	Iterate the free items list.
	 */
	for ( lpMemItem = lpMemPool->ilItems.lpiFirst; lpMemItem->lpiNext; lpMemItem = lpMemItem->lpiNext )
	{
		/*
		 *	Enough room in the item?
		 */
		if ( lpMemItem->dwSize >= dwSize )
			/*
			 *	Yes. Return the item.
			 */
			 return lpMemItem;
	}

	/*
	 *	No item found.
	 */
	return NULL;
}

/*
 *	Split up a free item.
 */
static DWORD SplitItem( LPMEMPOOL lpMemPool, LPMEMITEM lpMemItem, DWORD dwSize )
{
	DWORD		dwSizeLeft = 0;
	LPMEMITEM	lpSplitItem;

	/*
	 *	If the size matches exactly no
	 *	splitting is necessary.
	 */
	if ( lpMemItem->dwSize == dwSize )
		return( 0L );

	/*
	 *	When, after splitting, there is not enough
	 *	room left to create a new free item we return
	 *	the size left without splitting.
	 */
	dwSizeLeft = lpMemItem->dwSize - dwSize;
	if ( dwSizeLeft < sizeof( MEMITEM ))
		return dwSizeLeft;

	/*
	 *	Get a pointer to the memory behind
	 *	the allocation.
	 */
	lpSplitItem = ( LPMEMITEM )((( char * )lpMemItem ) + dwSize );

	/*
	 *	Set it up as a memory item.
	 */
	lpSplitItem->dwSize   = dwSizeLeft;
	lpSplitItem->lpbBlock = lpMemItem->lpbBlock;

	/*
	 *	Add it to the free item list.
	 */
	AddHead(( LPLIST )&lpMemPool->ilItems, ( LPNODE )lpSplitItem );

	return 0;
}

/*
 *	Allocate memory from the pool.
 */
static LPVOID InternalAllocPooled( LPMEMPOOL lpMemPool, LPDWORD lpdwSize )
{
	LPMEMITEM	lpMemItem;
	LPSYSITEM	lpSysItem;
	DWORD		dwSize = *lpdwSize;

	/*
	 *	Allocation size must be at least
	 *	sizeof( MEMITEM ) bytes.
	 */
	if ( dwSize < sizeof( MEMITEM )) dwSize = sizeof( MEMITEM );

	/*
	 *	Round up the allocation size.
	 */
	dwSize = ROUND_ALLOCATION_SIZE( dwSize );

	/*
	 *	Will the allocation fit in a block?
	 */
	if ( dwSize <= lpMemPool->dwBlockSize )
	{
		/*
		 *	Yes. Look up a suitable item.
		 */
		if (( lpMemItem = FindSpace( lpMemPool, dwSize )) != NULL )
		{
			/*
			 *	Split up the item if possible
			 *	or necessary.
			 */
			dwSize += SplitItem( lpMemPool, lpMemItem, dwSize );
			
			/*
			 *	"dwSize" more bytes in use.
			 */
			lpMemItem->lpbBlock->dwBytesUsed += dwSize;

			/*
			 *	Remove the item from the list.
			 */
			Remove(( LPNODE )lpMemItem );

			/*
			 *	Clear memory.
			 */
			memset( lpMemItem, 0, dwSize );

			/*
			 *	Store "real" allocation size.
			 */
			*lpdwSize = dwSize;

			/*
			 *	Return a pointer to the memory.
			 */
			return ( LPVOID )lpMemItem;
		}
		else
		{
			/*
			 *	No space was found so we try to
			 *	allocate more memory.
			 */
			if ( AllocBlock( lpMemPool ))
				/*
				 *	Try again...
				 */
				 return InternalAllocPooled( lpMemPool, lpdwSize );

			/*
			 *	Failure...
			 */
			return NULL;
		}
	}
	else
	{
		/*
		 *	Allocation size to large. Allocate
		 *	directly from the system.
		 */
		if (( lpSysItem = malloc( sizeof( SYSITEM ) + dwSize )) != NULL )
		{
			/*
			 *	Initialize the item.
			 */
			lpSysItem->lpvMemPtr = ( LPVOID )( lpSysItem + 1 );
			lpSysItem->dwSize    = dwSize;

			/*
			 *	Add it to the list.
			 */
			AddTail(( LPLIST )&lpMemPool->slSysMem, ( LPNODE )lpSysItem );

			/*
			 *	Clear the memory.
			 */
			memset( lpSysItem->lpvMemPtr, 0, dwSize );

			/*
			 *	Store "real" allocation size.
			 */
			*lpdwSize = dwSize;

			/*
			 *	Return a pointer to the memory.
			 */
			return lpSysItem->lpvMemPtr;
		}
	}

	/*
	 *	Failure...
	 */
	return NULL;
}

/*
 *	Find the block in which the
 *	memory was allocated.
 */
static LPMEMBLOCK FindBlock( LPMEMPOOL lpMemPool, LPVOID lpMemPtr )
{
	LPMEMBLOCK	lpMemBlock;
	DWORD		dwStart, dwEnd, dwPtr = ( DWORD )lpMemPtr;

	/*
	 *	Iterate the blocks.
	 */
	for ( lpMemBlock = lpMemPool->blBlocks.lpbFirst; lpMemBlock->lpbNext; lpMemBlock = lpMemBlock->lpbNext )
	{
		/*
		 *	Get the start and end
		 *	of this block.
		 */
		dwStart = ( DWORD )( lpMemBlock + 1 );
		dwEnd   = dwStart + lpMemPool->dwBlockSize;

		/*
		 *	Is the pointer located
		 *	in this block?
		 */
		if (( dwPtr >= dwStart ) && ( dwPtr <= dwEnd ))
			return lpMemBlock;
	}

	/*
	 *	Block not found. This means that either
	 *	it was located directly from system
	 *	memory or a false address was passed.
	 */
	return NULL;
}

/*
 *	Free some memory from the pool.
 */
static void InternalFreePooled( LPMEMPOOL lpMemPool, LPVOID lpMemPtr, DWORD dwSize )
{
	LPMEMBLOCK	lpMemBlock;
	LPMEMITEM	lpMemItem;
	LPSYSITEM	lpSysItem;

	/*
	 *	Try to locate the memory block.
	 */
	if (( lpMemBlock = FindBlock( lpMemPool, lpMemPtr )) != NULL )
	{
		/*
		 *	Set it up as a free item.
		 */
		lpMemItem = ( LPMEMITEM )lpMemPtr;
		lpMemItem->lpbBlock = lpMemBlock;
		lpMemItem->dwSize   = dwSize;

		/*
		 *	"dwSize" bytes less used.
		 */
		lpMemBlock->dwBytesUsed -= dwSize;

		/*
		 *	Any bytes still in use?
		 */
		if ( lpMemBlock->dwBytesUsed == 0L )
			/*
			 *	No. Free the unused block.
			 */
			 FreeBlock( lpMemPool, lpMemBlock );
		else
		{
			/*
			 *	Add the item to the free items list.
			 */
			AddHead(( LPLIST )&lpMemPool->ilItems, ( LPNODE )lpMemItem );

			/*
			 *	Garbage collection.
			 */
			OptimizeBlock( lpMemPool, lpMemBlock );
		}
	}
	else
	{
		/*
		 *	Try to find the memory in the system list.
		 */
		for ( lpSysItem = lpMemPool->slSysMem.lpsFirst; lpSysItem->lpsNext; lpSysItem = lpSysItem->lpsNext )
		{
			/*
			 *	Is this it?
			 */
			if ( lpSysItem->lpvMemPtr == lpMemPtr )
			{
				/*
				 *	Yes. Remove it from the list
				 *	and free it.
				 */
				Remove(( LPNODE )lpSysItem );
				free( lpSysItem );
				break;
			}
		}
	}
}

/*
 *	Allocate a new memory pool.
 */
POOL Brainchild_GetMemoryPool( DWORD dwBlockSize )
{
	LPMEMPOOL	lpMemPool;

	/*
	 *	Allocate structure.
	 */
	if (( lpMemPool = malloc( sizeof( MEMPOOL ))) != NULL )
	{
		/*
		 *	Initialize lists.
		 */
		NewList(( LPLIST )&lpMemPool->blBlocks );
		NewList(( LPLIST )&lpMemPool->ilItems );
		NewList(( LPLIST )&lpMemPool->slSysMem );

		/*
		 *	Round up block size.
		 */
		dwBlockSize = ROUND_ALLOCATION_SIZE( dwBlockSize );

		/*
		 *	Setup the block size with a minimum of
		 *	1 KByte.
		 */
		lpMemPool->dwBlockSize = dwBlockSize < 1024L ? 1024L : dwBlockSize;
	}

	/*
	 *	Return a pointer or NULL.
	 */
	return ( POOL )lpMemPool;
}

/*
 *	Free a memory pool.
 */
void Brainchild_FreeMemoryPool( POOL memPool, BOOL bAll )
{
	LPMEMBLOCK	lpMemBlock;
	LPSYSITEM	lpSysItem;

	/*
	 *	De-allocate all blocks.
	 */
	while (( lpMemBlock = ( LPMEMBLOCK )RemHead(( LPLIST )&MP( memPool )->blBlocks )) != NULL )
		free( lpMemBlock );

	/*
	 *	And the system memory.
	 */
	while (( lpSysItem = ( LPSYSITEM )RemHead(( LPLIST )&MP( memPool )->slSysMem )) != NULL )
		free( lpSysItem );

	/*
	 *	Free the pool also?
	 */
	if ( bAll )
		free( memPool );
	else
		NewList(( LPLIST )&MP( memPool )->ilItems );
}

/*
 *	Allocate memory.
 */
LPVOID Brainchild_AllocPooled( POOL memPool, DWORD dwSize )
{
	LPDWORD		lpdwMem;

	/*
	 *	Need room to store the allocation size.
	 */
	dwSize += sizeof( DWORD );

	/*
	 *	Allocate...
	 */
	if (( lpdwMem = InternalAllocPooled( MP( memPool ), &dwSize )) != NULL )
		*lpdwMem++ = dwSize;

	return ( LPVOID )lpdwMem;
}

/*
 *	Free memory.
 */
void Brainchild_FreePooled( POOL memPool, LPVOID lpMemPtr )
{
	/*
	 *	Adjust pointer.
	 */
	(( LPDWORD )lpMemPtr )--;

	/*
	 *	Free the memory.
	 */
	InternalFreePooled( MP( memPool ), lpMemPtr, *(( LPDWORD )lpMemPtr ));
}
