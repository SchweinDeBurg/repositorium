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

#ifndef _MYSPELLMGR_HXX_
#define _MYSPELLMGR_HXX_

#include "hashmgr.hxx"
#include "affixmgr.hxx"
#include "suggestmgr.hxx"
#include "csutil.hxx"

#define NOCAP   0
#define INITCAP 1
#define ALLCAP  2
#define HUHCAP  3

class MySpell
{
	AffixMgr*       pAMgr;
	HashMgr*        pHMgr;
	SuggestMgr*     pSMgr;
	char*           encoding;
	struct cs_info* csconv;
	int             maxSug;

public:
	MySpell(const char* affpath, const char* dpath);
	~MySpell();

	int suggest(char** * slst, const char* word);
	int spell(const char*);
	char* get_dic_encoding();
	bool isvalid();

private:
	int    cleanword(char*, const char*, int*, int*);
	char* check(const char*);
};

#endif
