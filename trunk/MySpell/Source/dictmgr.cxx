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

#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cstdio>

#include "dictmgr.hxx"

#pragma warning(disable:4706)

// some utility functions
extern void mychomp(char* s);
extern char* mystrdup(const char* s);
extern char* mystrsep(char** stringp, const char delim);

DictMgr::DictMgr(const char* dictpath, const char* etype)
{
	// load list of etype entries
	numdict = 0;
	pdentry = (dictentry*)malloc(MAXDICTIONARIES * sizeof(struct dictentry));
	if(pdentry)
	{
		if(parse_file(dictpath, etype))
		{
			numdict = 0;
			// no dictionary.lst found is okay
		}
	}
	else
	{
		numdict = 0;
	}
}


DictMgr::~DictMgr()
{
	dictentry* pdict = NULL;
	if(pdentry)
	{
		pdict = pdentry;
		for(int i = 0; i < numdict; i++)
		{
			if(pdict->lang)
			{
				free(pdict->lang);
				pdict->lang = NULL;
			}
			if(pdict->region)
			{
				free(pdict->region);
				pdict->region = NULL;
			}
			if(pdict->filename)
			{
				free(pdict->filename);
				pdict->filename = NULL;
			}
			pdict++;
		}
		free(pdentry);
		pdentry = NULL;
		pdict = NULL;
	}
	numdict = 0;
}


// read in list of etype entries and build up structure to describe them
int  DictMgr::parse_file(const char* dictpath, const char* etype)
{

	int i;
	char line[MAXDICTENTRYLEN+1];
	dictentry* pdict = pdentry;

	// open the dictionary list file
	FILE* dictlst;
	dictlst = fopen(dictpath, "r");
	if(!dictlst)
	{
		return 1;
	}

	// step one is to parse the dictionary list building up the
	// descriptive structures

	// read in each line ignoring any that dont start with etype
	while(fgets(line, MAXDICTENTRYLEN, dictlst))
	{
		mychomp(line);

		/* parse in a dictionary entry */
		if(strncmp(line, etype, 4) == 0)
		{
			if(numdict < MAXDICTIONARIES)
			{
				char* tp = line;
				char* piece;
				i = 0;
				while((piece = mystrsep(&tp, ' ')))
				{
					if(*piece != '\0')
					{
						switch(i)
						{
							case 0:
								break;
							case 1:
								pdict->lang = mystrdup(piece);
								break;
							case 2:
								if(strcmp(piece, "ANY") == 0)
								{
									pdict->region = mystrdup("");
								}
								else
								{
									pdict->region = mystrdup(piece);
								}
								break;
							case 3:
								pdict->filename = mystrdup(piece);
								break;
							default:
								break;
						}
						i++;
					}
					free(piece);
				}
				if(i == 4)
				{
					numdict++;
					pdict++;
				}
				else
				{
					fprintf(stderr, "dictionary list corruption in line \"%s\"\n", line);
					fflush(stderr);
				}
			}
		}
	}
	fclose(dictlst);
	return 0;
}

// return text encoding of dictionary
int DictMgr::get_list(dictentry** ppentry)
{
	*ppentry = pdentry;
	return numdict;
}
