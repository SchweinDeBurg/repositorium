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

#ifndef _AFFIXMGR_HXX_
#define _AFFIXMGR_HXX_

#include "atypes.hxx"
#include "baseaffix.hxx"
#include "hashmgr.hxx"
#include <cstdio>

class AffixMgr
{

	AffEntry *          pStart[SETSIZE];
	AffEntry *          sStart[SETSIZE];
	AffEntry *          pFlag[SETSIZE];
	AffEntry *          sFlag[SETSIZE];
	HashMgr *           pHMgr;
	char *              trystring;
	char *              encoding;
	char *              compound;
	int                 cpdmin;
	int                 numrep;
	replentry *         reptable;
	int                 nummap;
	mapentry *          maptable;
	bool                nosplitsugs;


public:

	AffixMgr(const char * affpath, HashMgr * ptr);
	~AffixMgr();
	struct hentry *     affix_check(const char * word, int len);
	struct hentry *     prefix_check(const char * word, int len);
	struct hentry *     suffix_check(const char * word, int len, int sfxopts, AffEntry* ppfx);
	int                 expand_rootword(struct guessword * wlst, int maxn,
		const char * ts, int wl, const char * ap, int al);
	struct hentry *     compound_check(const char * word, int len, char compound_flag);
	struct hentry *     lookup(const char * word);
	int                 get_numrep();
	struct replentry *  get_reptable();
	int                 get_nummap();
	struct mapentry *   get_maptable();
	char *              get_encoding();
	char *              get_try_string();
	char *              get_compound();
	bool                get_nosplitsugs();

private:
	int  parse_file(const char * affpath);
	int  parse_try(char * line);
	int  parse_set(char * line);
	int  parse_cpdflag(char * line);
	int  parse_cpdmin(char * line);
	int  parse_reptable(char * line, FILE * af);
	int  parse_maptable(char * line, FILE * af);
	int  parse_affix(char * line, const char at, FILE * af);

	void encodeit(struct affentry * ptr, char * cs);
	int build_pfxtree(AffEntry* pfxptr);
	int build_sfxtree(AffEntry* sfxptr);
	AffEntry* process_sfx_in_order(AffEntry* ptr, AffEntry* nptr);
	AffEntry* process_pfx_in_order(AffEntry* ptr, AffEntry* nptr);
	int process_pfx_tree_to_list();
	int process_sfx_tree_to_list();
	int process_pfx_order();
	int process_sfx_order();
};

#endif
