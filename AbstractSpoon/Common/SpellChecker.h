// Copyright (C) 2003-2005 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

// SpellChecker.h: interface for the CSpellChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPELLCHECKER_H__74596014_29E0_4289_9BC7_28A7996FA37B__INCLUDED_)
#define AFX_SPELLCHECKER_H__74596014_29E0_4289_9BC7_28A7996FA37B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISpellCheck.h"
#include "../../MySpell/Source/myspell.hxx"

class CSpellChecker : public ISpellChecker, protected MySpell
{
public:
	CSpellChecker(const char* szAffPath, const char* szDicPath);
	virtual ~CSpellChecker();

	void Release(); // releases the interface
	bool CheckSpelling(const char* szWord);
	bool CheckSpelling(const char* szWord, char**& pSuggestions, int& nNumSuggestions);
	void FreeSuggestions(char**& pSuggestions);

	bool IsValid();

};

#endif // !defined(AFX_SPELLCHECKER_H__74596014_29E0_4289_9BC7_28A7996FA37B__INCLUDED_)
