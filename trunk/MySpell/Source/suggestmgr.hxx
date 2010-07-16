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

#ifndef _SUGGESTMGR_HXX_
#define _SUGGESTMGR_HXX_

#define MAXSWL 100
#define MAX_ROOTS 10
#define MAX_WORDS 500
#define MAX_GUESS 10

#define NGRAM_IGNORE_LENGTH 0
#define NGRAM_LONGER_WORSE  1
#define NGRAM_ANY_MISMATCH  2


#include "atypes.hxx"
#include "affixmgr.hxx"
#include "hashmgr.hxx"

class SuggestMgr
{
	char *          ctry;
	int             ctryl;
	AffixMgr*       pAMgr;
	int             maxSug;
	bool            nosplitsugs;

public:
	SuggestMgr(const char * tryme, int maxn, AffixMgr *aptr);
	~SuggestMgr();

	int suggest(char** wlst, int ns, const char * word);
	int check(const char *, int);
	int ngsuggest(char ** wlst, char * word, HashMgr* pHMgr);

private:
	int replchars(char**, const char *, int);
	int mapchars(char**, const char *, int);
	int map_related(const char *, int, char ** wlst, int, const mapentry*, int);
	int forgotchar(char **, const char *, int);
	int swapchar(char **, const char *, int);
	int extrachar(char **, const char *, int);
	int badchar(char **, const char *, int);
	int twowords(char **, const char *, int);
	int ngram(int n, char * s1, const char * s2, int uselen);
	void bubblesort( char ** rwd, int * rsc, int n);
};

#endif
