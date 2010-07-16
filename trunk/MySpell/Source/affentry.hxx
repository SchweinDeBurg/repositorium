// Copyright 2002 Kevin B. Hendricks, Stratford, Ontario, Canada
// And Contributors (see CONTRIBUTORS file).  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. All modifications to the source code must be clearly marked as
//    such.  Binary redistributions based on modified source code
//    must be clearly marked as modified versions in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY KEVIN B. HENDRICKS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
// KEVIN B. HENDRICKS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - removed automatic CRT/MFC dependencies generation
// - replaced strdup() with _strdup() to use the ISO C++ conformant name
// - slighly reformatted source code
//*****************************************************************************

#ifndef _AFFIX_HXX_
#define _AFFIX_HXX_

#include "atypes.hxx"
#include "baseaffix.hxx"
#include "affixmgr.hxx"


/* A Prefix Entry  */

class PfxEntry : public AffEntry
{
	AffixMgr*    pmyMgr;

	PfxEntry* next;
	PfxEntry* nexteq;
	PfxEntry* nextne;
	PfxEntry* flgnxt;

public:

	PfxEntry(AffixMgr* pmgr, affentry* dp);
	~PfxEntry();

	struct hentry*       check(const char* word, int len);

	inline bool          allowCross()
	{
		return ((xpflg & XPRODUCT) != 0);
	}
	inline unsigned char getFlag()
	{
		return achar;
	}
	inline const char*   getKey()
	{
		return appnd;
	}
	char*                add(const char* word, int len);

	inline PfxEntry*     getNext()
	{
		return next;
	}
	inline PfxEntry*     getNextNE()
	{
		return nextne;
	}
	inline PfxEntry*     getNextEQ()
	{
		return nexteq;
	}
	inline PfxEntry*     getFlgNxt()
	{
		return flgnxt;
	}

	inline void   setNext(PfxEntry* ptr)
	{
		next = ptr;
	}
	inline void   setNextNE(PfxEntry* ptr)
	{
		nextne = ptr;
	}
	inline void   setNextEQ(PfxEntry* ptr)
	{
		nexteq = ptr;
	}
	inline void   setFlgNxt(PfxEntry* ptr)
	{
		flgnxt = ptr;
	}
};




/* A Suffix Entry */

class SfxEntry : public AffEntry
{
	AffixMgr*    pmyMgr;
	char*        rappnd;

	SfxEntry*    next;
	SfxEntry*    nexteq;
	SfxEntry*    nextne;
	SfxEntry*    flgnxt;

public:

	SfxEntry(AffixMgr* pmgr, affentry* dp);
	~SfxEntry();

	struct hentry*    check(const char* word, int len, int optflags,
	                        AffEntry* ppfx);

	inline bool          allowCross()
	{
		return ((xpflg & XPRODUCT) != 0);
	}
	inline unsigned char getFlag()
	{
		return achar;
	}
	inline const char*   getKey()
	{
		return rappnd;
	}
	char*                add(const char* word, int len);

	inline SfxEntry*     getNext()
	{
		return next;
	}
	inline SfxEntry*     getNextNE()
	{
		return nextne;
	}
	inline SfxEntry*     getNextEQ()
	{
		return nexteq;
	}
	inline SfxEntry*     getFlgNxt()
	{
		return flgnxt;
	}

	inline void   setNext(SfxEntry* ptr)
	{
		next = ptr;
	}
	inline void   setNextNE(SfxEntry* ptr)
	{
		nextne = ptr;
	}
	inline void   setNextEQ(SfxEntry* ptr)
	{
		nexteq = ptr;
	}
	inline void   setFlgNxt(SfxEntry* ptr)
	{
		flgnxt = ptr;
	}

};


#endif
