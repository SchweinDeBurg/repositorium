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

#ifndef _HASHMGR_HXX_
#define _HASHMGR_HXX_

#include "htypes.hxx"

class HashMgr
{
	int             tablesize;
	struct hentry * tableptr;

public:
	HashMgr(const char * tpath);
	~HashMgr();

	struct hentry * lookup(const char *) const;
	int hash(const char *) const;
	struct hentry * walk_hashtable(int & col, struct hentry * hp) const;
	int get_tablesize() { return tablesize; }

private:
	HashMgr( const HashMgr & ); // not implemented
	HashMgr &operator=( const HashMgr & ); // not implemented
	int load_tables(const char * tpath);
	int add_word(const char * word, int wl, const char * ap, int al);

};

#endif
