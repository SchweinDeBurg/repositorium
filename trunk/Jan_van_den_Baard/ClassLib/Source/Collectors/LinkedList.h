#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_
//
// LinkedList.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "collector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

// Base class for all classes that wish to
// be added to a ClsLinkedList.
class ClsNode
{
public:
	ClsNode() { m_pNext = NULL; m_pPrev = NULL; }
	virtual ~ClsNode() {}

	// Link pointers to the next and
	// previous node.
	ClsNode		*m_pNext;
	ClsNode		*m_pPrev;
};

// Linked list class. This class is used to manage a list
// of ClsNode and ClsNode derived classes. The list is doubly
// linked and can be iterated front to back and vice-versa.
template< typename TYPE >
class ClsLinkedList : public ClsCollector< TYPE >
{
public:
	ClsLinkedList() { Reset(); }
	virtual ~ClsLinkedList()
	{;}

	// Add a new node to the head of the list.
	void AddHead( TYPE *pNode )
	{
		_ASSERT_VALID( pNode );

		// Are there already nodes in the list?
		if ( m_pFirst != NULL )
		{
			// Link in the new node.
			pNode->m_pPrev = NULL;
			pNode->m_pNext = m_pFirst;

			// Update the link pointers
			m_pFirst->m_pPrev = pNode;
			m_pFirst = pNode;
		}
		else
		{
			// Clear node pointers.
			pNode->m_pNext = pNode->m_pPrev = NULL;

			// Make the node the current first and
			// last node.
			m_pFirst = m_pLast = pNode;
		}

		// We have one more node.
		m_nNumNodes++;
	}

	// Add a new node to the tail of the list.
	void AddTail( TYPE *pNode )
	{
		_ASSERT_VALID( pNode );

		// Are there already nodes in the list?
		if ( m_pLast )
		{
			// Link in the new node.
			pNode->m_pNext = NULL;
			pNode->m_pPrev = m_pLast;

			// Update link pointers.
			m_pLast->m_pNext = pNode;
			m_pLast = pNode;
		}
		else
		{
			// Clear node pointers.
			pNode->m_pNext = pNode->m_pPrev = NULL;

			// Make the node the current first and
			// last node.
			m_pFirst = m_pLast = pNode;
		}

		// We have one more node.
		m_nNumNodes++;
	}

	// Insert a node into the list.
	void Insert( TYPE *pNode, TYPE *pPred )
	{
		_ASSERT_VALID( pNode ); 
		_ASSERT_VALID( pPred );

		// If the predecessor of the node to insert
		// is the current last node we simply AddTail()
		// the new node.
		if ( pPred == m_pLast )
			AddTail( pNode );
		else
		{
			// Setup the node it's link
			// pointers.
			pNode->m_pNext = pPred->m_pNext;
			pNode->m_pPrev = pPred;

			// Link in the node.
			pPred->m_pNext->m_pPrev = pNode;
			pPred->m_pNext = pNode;

			// We have one more node.
			m_nNumNodes++;
		}
	}

	// Remove the first node in the list
	// and return a pointer to it.
	TYPE *RemoveHead()
	{
		TYPE	*pRCode;

		// Is there a first node?
		if (( pRCode = m_pFirst ) != NULL )
		{
			// If the first node is the same as
			// the last node we reset the list.
			if ( m_pFirst == m_pLast )
			{
				// Reset link pointers.
				Reset();

				// Will be corrected below.
				m_nNumNodes++;
			}
			else
			{
				// Remove the first node and
				// update the link pointers.
				m_pFirst = ( TYPE * )m_pFirst->m_pNext;
				m_pFirst->m_pPrev = NULL;
			}

			// Clear node link pointers.
			pRCode->m_pNext = pRCode->m_pPrev = NULL;

			// One less node.
			m_nNumNodes--;
		}

		// Return node pointer or NULL.
		return pRCode;
	}

	// Remove the last node from in the list
	// and return a pointer to it.
	TYPE *RemoveTail()
	{
		TYPE *pRCode;

		// Is there a last node?
		if (( pRCode = m_pLast ) != NULL )
		{
			// If the first node is the same as
			// the last node we reset the list.
			if ( m_pFirst == m_pLast )
			{
				// Reset link pointers.
				Reset();

				// Will be corrected below.
				m_nNumNodes++;
			}
			else
			{
				// Remove the first node and
				// update the link pointers.
				m_pLast = ( TYPE * )m_pLast->m_pPrev;
				m_pLast->m_pNext = NULL;
			}

			// Clear node link pointers.
			pRCode->m_pNext = pRCode->m_pPrev = NULL;

			// One less node.
			m_nNumNodes--;
		}

		// Return node pointer or NULL.
		return pRCode;
	}

	// Remove a specific node from the list.
	void Remove( TYPE *pNode )
	{
		_ASSERT_VALID( pNode );

		// Do a RemoveHead() if the node to remove
		// is the first one in the list.
		if ( pNode == m_pFirst )
			RemoveHead();
		// Do a RemoveTail() if the node to remove
		// is the last one in the list.
		else if ( pNode == m_pLast )
			RemoveTail();
		else
		{
			// When this point is reached it means the node should
			// have predecessors and successors. Both the m_pPred and
			// the m_pNext members should be non-zero.
			_ASSERT_VALID( pNode->m_pNext ); 
			_ASSERT_VALID( pNode->m_pPrev );

			// Update the link pointers.
			pNode->m_pPrev->m_pNext = pNode->m_pNext;
			pNode->m_pNext->m_pPrev = pNode->m_pPrev;

			// Clear the node it's link pointers.
			pNode->m_pNext = pNode->m_pPrev = NULL;

			// One less node.
			m_nNumNodes--;
		}
	}


	// Free the nodes from the list.
	void Free()
	{
		TYPE	*pNode;

		// Remove and destroy all nodes.
		while (( pNode = RemoveHead()) != NULL )
			delete pNode;
	}

	// Reset the list contents.
	inline void Reset()
	{
		m_pFirst = m_pLast = NULL;
		m_nNumNodes = 0;
	}

	// Return the number of nodes currently
	// in the list.
	inline int GetSize() const
	{
		return m_nNumNodes;
	}

	// Same as AddTail().
	inline void operator += ( TYPE *pNode )
	{
		AddTail( pNode );
	}

	// Same as Remove().
	inline void operator -= ( TYPE *pNode )
	{
		Remove( pNode );
	}

	// Return a pointer to the first node.
	inline TYPE *GetFirst() const
	{
		return m_pFirst;
	}

	// Return a pointer to the last node.
	inline TYPE *GetLast() const
	{
		return m_pLast;
	}

	// Return a pointer to the next node.
	inline TYPE *GetNext( TYPE *pNode ) const
	{
		_ASSERT_VALID( pNode );
		return ( TYPE * )pNode->m_pNext;
	}

	// Return a pointer to the previous node.
	inline TYPE *GetPrevious( TYPE *pNode ) const
	{
		_ASSERT_VALID( pNode );
		return ( TYPE * )pNode->m_pPrev;
	}

protected:
	// Link pointer to the first and last node.
	TYPE	*m_pFirst;
	TYPE	*m_pLast;

	// Number of nodes in the list.
	int	 m_nNumNodes;
};
#endif // _LINKEDLIST_H_