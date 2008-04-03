#ifndef __LIST_H__
#define __LIST_H__
/*
 *	list.h
 *
 *	(C) Copyright 1997-2003 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Doubly-linked lists.
 */

#ifndef _WINDOWS_
#include <windows.h>
#endif /* _WINDOWS */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 
 *	Node structure. This is the base structure of
 *	any data you add to a list.
 */
typedef struct tagNode	*LPNODE;

typedef struct tagNode
{
	LPNODE			lpnNext;
	LPNODE			lpnPrev;
} NODE;

/*
 *	List structure. The nodes are all chained via
 *	this stucture.
 */
typedef struct tagList	*LPLIST;

typedef struct tagList
{
	LPNODE			lpnFirst;
	LPNODE			lpnEndMarker;
	LPNODE			lpnLast;
} LIST;

/*
 *	Disable inlining by defining _NO_INLINE
 *	before including this file.
 */
#ifndef _NO_INLINE
#define INLINE __inline
#else
#define INLINE
#endif /* _NO_INLINE */
/*
 *	Initialize a list.
 */
INLINE void NewList( LPLIST lpList )
{
	lpList->lpnFirst     = ( LPNODE )&lpList->lpnEndMarker;
	lpList->lpnEndMarker = ( LPNODE )NULL;
	lpList->lpnLast      = ( LPNODE )&lpList->lpnFirst;
}

/*
 *	Add a node to the head of the list.
 */
INLINE void AddHead( LPLIST lpList, LPNODE lpNode )
{
	/*
	 *	Pick up the first node which will
	 *	become the second one.
	 */
	LPNODE		lpFirst = lpList->lpnFirst;

	/*
	 *	Update the input node it's
	 *	link pointers.
	 */
	lpNode->lpnPrev = ( LPNODE )&lpList->lpnFirst;
	lpNode->lpnNext = lpFirst;
	
	/*
	 *	Link in the node at the head
	 *	of the list and update the next
	 *	node it's previous link to point
	 *	at the new node.
	 */
	lpList->lpnFirst = lpFirst->lpnPrev = lpNode;
}

/*
 *	Remove the first node and return a 
 *	pointer to it.
 */
INLINE LPNODE RemHead( LPLIST lpList )
{
	/*
	 *	Pick up a pointer to the first
	 *	node which is the one to be removed.
	 */
	LPNODE		lpFirst = lpList->lpnFirst;

	/*
	 *	Are there any nodes present 
	 *	in the list.
	 */
	if ( lpFirst->lpnNext != NULL )
	{
		/*
		 *	There are nodes present.
		 *
		 *	Make the first node it's successor
		 *	the first one in the list.
		 */
		lpList->lpnFirst = lpFirst->lpnNext;

		/*
		 *	Update that node it's previous
		 *	link pointer.
		 */
		lpList->lpnFirst->lpnPrev = ( LPNODE )&lpList->lpnFirst;
	}
	else
		/*
		 *	No nodes to remove.
		 */
		 lpFirst = NULL;

	/*
	 *	Return a pointer to the removed node
	 *	or NULL if no node was removed.
	 */
	return lpFirst;
}

/*
 *	Add a node to the end of the list.
 */
INLINE void AddTail( LPLIST lpList, LPNODE lpNode )
{
	/*
	 *	Pick up a pointer to the last 
	 *	which will become the node preceeding
	 *	the last node.
	 */
	LPNODE		lpLast = lpList->lpnLast;

	/*
	 *	Update the node it's link pointers.
	 */
	lpNode->lpnNext = ( LPNODE )&lpList->lpnEndMarker;
	lpNode->lpnPrev = lpLast;

	/*
	 *	Update the list and previous node
	 *	link pointers.
	 */
	lpList->lpnLast = lpLast->lpnNext = lpNode;
}

/*
 *	Remove a node from the end of the list
 *	and return a pointer to it.
 */
INLINE LPNODE RemTail( LPLIST lpList )
{
	/*
	 *	Pick up a pointer to the
	 *	node which is to be removed.
	 */
	LPNODE		lpLast = lpList->lpnLast;

	/*
	 *	Are there any nodes in the list?
	 */
	if ( lpLast != ( LPNODE )&lpList->lpnFirst )
	{
		/*
		 *	There are node present.
		 *
		 *	We set the node preceeding the
		 *	one to remove as the last one
		 *	in the list.
		 */
		lpList->lpnLast = lpLast->lpnPrev;

		/*
		 *	Update it's previous link pointer.
		 */
		lpList->lpnLast->lpnNext = ( LPNODE )&lpList->lpnEndMarker;
	}
	else
		/*
		 *	No nodes to remove.
		 */
		 lpLast = NULL;

	/*
	 *	Return a pointer to the removed
	 *	node or NULL if no node was removed.
	 */
	 return lpLast;
}

/*
 *	Remove a node from the list it is in.
 *
 *	NOTE: The node must be linked in a list
 *	      or serious problems may arise.
 */
INLINE void Remove( LPNODE lpNode )
{
	/*
	 *	Set the next link pointer of the previous
	 *	node to point at the next node and set the
	 *	previous link pointer of the next node to
	 *	point at the previous node.
	 */
        lpNode->lpnPrev->lpnNext = lpNode->lpnNext;
        lpNode->lpnNext->lpnPrev = lpNode->lpnPrev;
}

/*
 *	Insert a node after a specific other
 *	node.
 */
INLINE void Insert( LPLIST lpList, LPNODE lpNode, LPNODE lpPred )
{
	/*
	 *	If the node which we isert after is
	 *	the last one in the list we can
	 *	simply AddTail() it.
	 */
	if ( lpPred == lpList->lpnLast )
		AddTail( lpList, lpNode );
	else 
	{
		/*
		 *	Update the node it's
		 *	link pointers.
		 */
		lpNode->lpnNext = lpPred->lpnNext;
		lpNode->lpnPrev = lpPred;

		/*
		 *	Update the previous link pointer
		 *	of the next node and the next
		 *	link pointer of the previous node.
		 */
		lpNode->lpnNext->lpnPrev = lpPred->lpnNext = lpNode;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LIST_H__ */
