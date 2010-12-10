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

// RichEditSpellCheck.cpp: implementation of the CRichEditSpellCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RichEditSpellCheck.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRichEditSpellCheck::CRichEditSpellCheck(CRichEditCtrl& re) : m_re(re)
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

	cr.cpMin = m_re.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, m_crCurrentWord.cpMax + 1);
	cr.cpMax = m_re.SendMessage(EM_FINDWORDBREAK, WB_RIGHTBREAK, cr.cpMin + 1);

	const char* szWord = GetWord(cr);

	if (szWord && *szWord)
		m_crCurrentWord = cr;

	return szWord;
}

const char* CRichEditSpellCheck::GetCurrentWord() const
{
	return GetWord(m_crCurrentWord);
}

const char* CRichEditSpellCheck::GetWord(const CHARRANGE& cr) const
{
	if (cr.cpMax > cr.cpMin)
	{
		static CStringA sWord;

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

		return sWord;
	}

	// else
	return NULL;
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
	m_crCurrentWord.cpMin = m_crCurrentWord.cpMax = 0;
	m_re.SetSel(m_crCurrentWord);
}
