#ifndef __NOFRAG_H__
#define __NOFRAG_H__
/*
 *	nofrag.h
 *
 *	(C) Copyright 1993-1999 Jan van den Baard
 *	    All Rights Reserved.
 *
 *	Special memory allocation routines which
 *	prevent memory fragmentation.
 */

#ifndef _WINDOWS_
#include <windows.h>
#endif /* _WINDOWS_ */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 
 *	The memory pool is private.
 */
typedef void		*POOL;

/*
 *	Prototypes.
 */
POOL Brainchild_GetMemoryPool( DWORD );
void Brainchild_FreeMemoryPool( POOL, BOOL );
LPVOID Brainchild_AllocPooled( POOL, DWORD );
void Brainchild_FreePooled( POOL, LPVOID );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NOFRAG_H__ */
