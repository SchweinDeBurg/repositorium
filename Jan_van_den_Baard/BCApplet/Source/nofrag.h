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
POOL GetMemoryPool( DWORD );
void FreeMemoryPool( POOL, BOOL );
LPVOID AllocPooled( POOL, DWORD );
void FreePooled( POOL, LPVOID );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NOFRAG_H__ */
