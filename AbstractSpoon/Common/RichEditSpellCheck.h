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
// - merged with ToDoList version 6.1.6 sources
//*****************************************************************************

// RichEditSpellCheck.h: interface for the CRichEditSpellCheck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHEDITSPELLCHECK_H__9A6FC513_71CB_4207_9FED_7B1429010FE5__INCLUDED_)
#define AFX_RICHEDITSPELLCHECK_H__9A6FC513_71CB_4207_9FED_7B1429010FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISpellCheck.h"

class CRichEditBaseCtrl;

class CRichEditSpellCheck : public ISpellCheck
{
public:
	CRichEditSpellCheck(CRichEditBaseCtrl& re);
	virtual ~CRichEditSpellCheck();

	const char* GetFirstWord() const;
	const char* GetNextWord() const;
	const char* GetCurrentWord() const;

	void SelectCurrentWord();
	void ReplaceCurrentWord(const char* szWord);

	void ClearSelection();

protected:
	CRichEditBaseCtrl& m_re;
	mutable CHARRANGE m_crCurrentWord;

protected:
	void GetWord(const CHARRANGE& cr, CStringA& sWord) const;
};

#endif // !defined(AFX_RICHEDITSPELLCHECK_H__9A6FC513_71CB_4207_9FED_7B1429010FE5__INCLUDED_)
