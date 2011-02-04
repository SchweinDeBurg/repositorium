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
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.1.4 sources
// - merged with ToDoList version 6.1.6 sources
//*****************************************************************************

// RichEditSpellCheck.cpp: implementation of the CRichEditSpellCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RichEditSpellCheck.h"
#include "../../CodeProject/Source/RichEditBaseCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const CStringA DELIMS("\x20\t\r\n.,:;-/?<>|~!@#$%^&*()+=");

CRichEditSpellCheck::CRichEditSpellCheck(CRichEditBaseCtrl& re) : m_re(re)
{
	m_crCurrentWord.cpMin = m_crCurrentWord.cpMax = 0;
}

CRichEditSpellCheck::~CRichEditSpellCheck()
{
}

const char* CRichEditSpellCheck::GetFirstWord() const
{
	m_crCurrentWord.cpMin = m_crCurrentWord.cpMax = -1;

	return GetNextWord();
}

const char* CRichEditSpellCheck::GetNextWord() const
{
	CHARRANGE cr;

	cr.cpMin = m_re.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, m_crCurrentWord.cpMax);
	cr.cpMax = m_re.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, cr.cpMin + 1);

	// if nothing has changed then quit
	if (cr.cpMin == m_crCurrentWord.cpMin && cr.cpMax == m_crCurrentWord.cpMax)
	{
		return NULL;
	}

	static CStringA sWord;
	GetWord(cr, sWord);

	// trim at start
	int nLenOrg = sWord.GetLength();
	sWord.TrimLeft(DELIMS);
	cr.cpMin += (nLenOrg - sWord.GetLength());

	// trim at end
	nLenOrg = sWord.GetLength();
	sWord.TrimRight(DELIMS);
	cr.cpMax -= (nLenOrg - sWord.GetLength());

	m_crCurrentWord = cr;

	// if sWord is empty add a space else the iteration will end
	if (sWord.IsEmpty())
	{
		sWord = "\x20";
	}

	return sWord;
}

const char* CRichEditSpellCheck::GetCurrentWord() const
{
	static CStringA sWord;
	GetWord(m_crCurrentWord, sWord);

	return sWord;
}

void CRichEditSpellCheck::GetWord(const CHARRANGE& cr, CStringA& sWord) const
{
	sWord.Empty();

	if (cr.cpMax > cr.cpMin)
	{
		TEXTRANGE tr;
		tr.chrg = cr;

#if defined(UNICODE) || defined(_UNICODE)
		CString strTemp;
		tr.lpstrText = strTemp.GetBuffer(cr.cpMax - cr.cpMin);
		m_re.SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);
		strTemp.ReleaseBuffer();
		sWord = ATL::CT2A(strTemp);
#else
		tr.lpstrText = sWord.GetBuffer(cr.cpMax - cr.cpMin);
		m_re.SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);
		sWord.ReleaseBuffer();
#endif   // UNICODE || _UNICODE
	}
}

void CRichEditSpellCheck::SelectCurrentWord()
{
	m_re.SetSel(m_crCurrentWord);

	// need to make sure line is visible
	CPoint ptSel = m_re.GetCharPos(m_crCurrentWord.cpMax);

	CRect rClient;
	m_re.GetClientRect(rClient);

	if (ptSel.y >= rClient.bottom)
	{
		while (ptSel.y >= rClient.bottom)
		{
			m_re.LineScroll(1);
			ptSel = m_re.GetCharPos(m_crCurrentWord.cpMax);
		}

		// one more for good measure
		m_re.LineScroll(1);
	}
	else if (ptSel.y < rClient.top)
	{

		while (ptSel.y < rClient.top)
		{
			m_re.LineScroll(-1);
			ptSel = m_re.GetCharPos(m_crCurrentWord.cpMax);
		}

		// one more for good measure
		m_re.LineScroll(-1);
	}
}

void CRichEditSpellCheck::ReplaceCurrentWord(const char* szWord)
{
	m_re.SetSel(m_crCurrentWord);
	m_re.ReplaceSel(ATL::CA2T(szWord), TRUE);
}

void CRichEditSpellCheck::ClearSelection()
{
	m_crCurrentWord.cpMin = m_crCurrentWord.cpMax;
	m_re.SetSel(m_crCurrentWord);
}
