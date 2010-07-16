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

#include "vcconfig.hxx"

#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "affentry.hxx"

#pragma warning(disable:4514)

extern char * mystrdup(const char * s);
extern char *  myrevstrdup(const char * s);

PfxEntry::PfxEntry(AffixMgr* pmgr, affentry* dp)
{
	// register affix manager
	pmyMgr = pmgr;

	// set up its intial values
	achar = dp->achar;         // char flag
	strip = dp->strip;         // string to strip
	appnd = dp->appnd;         // string to append
	stripl = dp->stripl;       // length of strip string
	appndl = dp->appndl;       // length of append string
	numconds = dp->numconds;   // number of conditions to match
	xpflg = dp->xpflg;         // cross product flag
	// then copy over all of the conditions
	memcpy(&conds[0],&dp->conds[0],SETSIZE*sizeof(conds[0]));
	next = NULL;
	nextne = NULL;
	nexteq = NULL;
}


PfxEntry::~PfxEntry()
{
	achar = '\0';
	if (appnd) free(appnd);
	if (strip)free(strip);
	pmyMgr = NULL;
	appnd = NULL;
	strip = NULL;
}



// add prefix to this word assuming conditions hold
char * PfxEntry::add(const char * word, int len)
{
	int			cond;
	char	        tword[MAXWORDLEN+1];

	/* make sure all conditions match */
	if ((len > stripl) && (len >= numconds)) {
		unsigned char * cp = (unsigned char *) word;
		for (cond = 0;  cond < numconds;  cond++) {
			if ((conds[*cp++] & (1 << cond)) == 0)
				break;
		}
		if (cond >= numconds) {
			/* we have a match so add prefix */
			int tlen = 0;
			if (appndl) {
				strcpy(tword,appnd);
				tlen += appndl;
			}
			char * pp = tword + tlen;
			strcpy(pp, (word + stripl));
			return mystrdup(tword);
		}
	}
	return NULL;
}




// check if this prefix entry matches
struct hentry * PfxEntry::check(const char * word, int len)
{
	int			cond;	// condition number being examined
	int	                tmpl;   // length of tmpword
	struct hentry *     he;     // hash entry of root word or NULL
	unsigned char *	cp;
	char	        tmpword[MAXWORDLEN+1];


	// on entry prefix is 0 length or already matches the beginning of the word.
	// So if the remaining root word has positive length
	// and if there are enough chars in root word and added back strip chars
	// to meet the number of characters conditions, then test it

	tmpl = len - appndl;

	if ((tmpl > 0) &&  (tmpl + stripl >= numconds)) {

		// generate new root word by removing prefix and adding
		// back any characters that would have been stripped

		if (stripl) strcpy (tmpword, strip);
		strcpy ((tmpword + stripl), (word + appndl));

		// now make sure all of the conditions on characters
		// are met.  Please see the appendix at the end of
		// this file for more info on exactly what is being
		// tested

		cp = (unsigned char *)tmpword;
		for (cond = 0;  cond < numconds;  cond++) {
			if ((conds[*cp++] & (1 << cond)) == 0) break;
		}

		// if all conditions are met then check if resulting
		// root word in the dictionary

		if (cond >= numconds) {
			tmpl += stripl;
			if ((he = pmyMgr->lookup(tmpword)) != NULL) {
				if (TESTAFF(he->astr, achar, he->alen)) return he;
			}

			// prefix matched but no root word was found
			// if XPRODUCT is allowed, try again but now
			// ross checked combined with a suffix

			if (xpflg & XPRODUCT) {
				he = pmyMgr->suffix_check(tmpword, tmpl, XPRODUCT, (AffEntry *)this);
				if (he) return he;
			}
		}
	}
	return NULL;
}



SfxEntry::SfxEntry(AffixMgr * pmgr, affentry* dp)
{
	// register affix manager
	pmyMgr = pmgr;

	// set up its intial values
	achar = dp->achar;         // char flag
	strip = dp->strip;         // string to strip
	appnd = dp->appnd;         // string to append
	stripl = dp->stripl;       // length of strip string
	appndl = dp->appndl;       // length of append string
	numconds = dp->numconds;   // number of conditions to match
	xpflg = dp->xpflg;         // cross product flag

	// then copy over all of the conditions
	memcpy(&conds[0],&dp->conds[0],SETSIZE*sizeof(conds[0]));

	rappnd = myrevstrdup(appnd);
}


SfxEntry::~SfxEntry()
{
	achar = '\0';
	if (appnd) free(appnd);
	if (rappnd) free(rappnd);
	if (strip) free(strip);
	pmyMgr = NULL;
	appnd = NULL;
	strip = NULL;
}



// add suffix to this word assuming conditions hold
char * SfxEntry::add(const char * word, int len)
{
	int			cond;
	char	        tword[MAXWORDLEN+1];

	/* make sure all conditions match */
	if ((len > stripl) && (len >= numconds)) {
		unsigned char * cp = (unsigned char *) (word + len);
		for (cond = numconds; --cond >=0; ) {
			if ((conds[*--cp] & (1 << cond)) == 0)
				break;
		}
		if (cond < 0) {
			/* we have a match so add suffix */
			strcpy(tword,word);
			int tlen = len;
			if (stripl) {
				tlen -= stripl;
			}
			char * pp = (tword + tlen);
			if (appndl) {
				strcpy(pp,appnd);
				tlen += appndl;
			} else *pp = '\0';
			return mystrdup(tword);
		}
	}
	return NULL;
}



// see if this suffix is present in the word
struct hentry * SfxEntry::check(const char * word, int len, int optflags, AffEntry* ppfx)
{
	int	                tmpl;		 // length of tmpword
	int			cond;		 // condition beng examined
	struct hentry *     he;              // hash entry pointer
	unsigned char *	cp;
	char	        tmpword[MAXWORDLEN+1];
	PfxEntry* ep = (PfxEntry *) ppfx;


	// if this suffix is being cross checked with a prefix
	// but it does not support cross products skip it

	if ((optflags & XPRODUCT) != 0 &&  (xpflg & XPRODUCT) == 0)
		return NULL;

	// upon entry suffix is 0 length or already matches the end of the word.
	// So if the remaining root word has positive length
	// and if there are enough chars in root word and added back strip chars
	// to meet the number of characters conditions, then test it

	tmpl = len - appndl;

	if ((tmpl > 0)  &&  (tmpl + stripl >= numconds)) {

		// generate new root word by removing suffix and adding
		// back any characters that would have been stripped or
		// or null terminating the shorter string

		strcpy (tmpword, word);
		cp = (unsigned char *)(tmpword + tmpl);
		if (stripl) {
			strcpy ((char *)cp, strip);
			tmpl += stripl;
			cp = (unsigned char *)(tmpword + tmpl);
		} else *cp = '\0';

		// now make sure all of the conditions on characters
		// are met.  Please see the appendix at the end of
		// this file for more info on exactly what is being
		// tested

		for (cond = numconds;  --cond >= 0; ) {
			if ((conds[*--cp] & (1 << cond)) == 0) break;
		}

		// if all conditions are met then check if resulting
		// root word in the dictionary

		if (cond < 0) {
			if ((he = pmyMgr->lookup(tmpword)) != NULL) {
				if (TESTAFF(he->astr, achar , he->alen) &&
					((optflags & XPRODUCT) == 0 ||
					TESTAFF(he->astr, ep->getFlag(), he->alen))) return he;
			}
		}
	}
	return NULL;
}
