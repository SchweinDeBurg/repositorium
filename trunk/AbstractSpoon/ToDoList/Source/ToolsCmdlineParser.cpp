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

// ToolsCmdlineParser.cpp: implementation of the CToolsCmdlineParser class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "ToolsCmdlineParser.h"

#include "../../../CodeProject/Source/DateHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMap<CString, LPCTSTR, CLA_TYPE, CLA_TYPE&> CToolsCmdlineParser::s_mapTypes;

CToolsCmdlineParser::CToolsCmdlineParser(LPCTSTR szCmdLine)
{
	// init static map first time only
	if (s_mapTypes.GetCount() == 0)
	{
		s_mapTypes["pathname"] = CLAT_PATHNAME;
		s_mapTypes["filetitle"] = CLAT_FILETITLE;
		s_mapTypes["folder"] = CLAT_FOLDER;
		s_mapTypes["filename"] = CLAT_FILENAME;
		s_mapTypes["seltid"] = CLAT_SELTASKID;
		s_mapTypes["selttitle"] = CLAT_SELTASKTITLE;
		s_mapTypes["userfile"] = CLAT_USERFILE;
		s_mapTypes["userfolder"] = CLAT_USERFOLDER;
		s_mapTypes["usertext"] = CLAT_USERTEXT;
		s_mapTypes["userdate"] = CLAT_USERDATE;
		s_mapTypes["todaysdate"] = CLAT_TODAYSDATE;
		s_mapTypes["todolist"] = CLAT_TODOLIST;
		s_mapTypes["seltextid"] = CLAT_SELTASKEXTID;
		s_mapTypes["seltcomments"] = CLAT_SELTASKCOMMENTS;
		s_mapTypes["seltfile"] = CLAT_SELTASKFILELINK;
		s_mapTypes["seltallocby"] = CLAT_SELTASKALLOCBY;
		s_mapTypes["seltallocto"] = CLAT_SELTASKALLOCTO;
	}

	SetCmdLine(szCmdLine);
}

CToolsCmdlineParser::~CToolsCmdlineParser()
{

}

void CToolsCmdlineParser::SetCmdLine(LPCTSTR szCmdLine)
{
	m_sCmdLine = szCmdLine;
	m_aArgs.RemoveAll();

	ParseCmdLine();

	// replace todaysdate
	ReplaceArgument(CLAT_TODAYSDATE, CDateHelper::FormatCurrentDate(TRUE));
}

int CToolsCmdlineParser::GetArguments(CCLArgArray& aArgs) const
{
	aArgs.Copy(m_aArgs);
	return aArgs.GetSize();
}

int CToolsCmdlineParser::GetUserArguments(CCLArgArray& aArgs) const
{
	int nArg = m_aArgs.GetSize();

	// add to start to maintain order
	while (nArg--)
	{
		switch (m_aArgs[nArg].nType)
		{
		case CLAT_USERFILE:
		case CLAT_USERFOLDER:
		case CLAT_USERTEXT:
		case CLAT_USERDATE:
			{
				CMDLINEARG cla = m_aArgs[nArg];
				aArgs.InsertAt(0, cla);
			}
			break;
		}
	}

	return aArgs.GetSize();
}

BOOL CToolsCmdlineParser::ReplaceArgument(CLA_TYPE nType, LPCTSTR szValue)
{
	// see if we have that type
	int nArg = m_aArgs.GetSize();
	BOOL bFound = FALSE;

	// and replace all of them
	while (nArg--)
	{
		if (m_aArgs[nArg].nType == nType)
			bFound |= ReplaceArgument(nArg, szValue);
	}

	// not found
	return bFound;
}

BOOL CToolsCmdlineParser::ReplaceArgument(CLA_TYPE nType, DWORD dwValue)
{
	CString sValue;
	sValue.Format("%d", dwValue);

	return ReplaceArgument(nType, sValue);
}

BOOL CToolsCmdlineParser::ReplaceArgument(LPCTSTR szName, LPCTSTR szValue)
{
	// see if we have a user item with that name
	CString sName(szName);
	sName.MakeLower();

	int nArg = m_aArgs.GetSize();

	while (nArg--)
	{
		switch (m_aArgs[nArg].nType)
		{
		case CLAT_USERFILE:
		case CLAT_USERFOLDER:
		case CLAT_USERTEXT:
		case CLAT_USERDATE:
			if (sName.CompareNoCase(m_aArgs[nArg].sName) == 0)
				return ReplaceArgument(nArg, szValue);
			break;
		}
	}

	// not found
	return FALSE;

}

BOOL CToolsCmdlineParser::IsUserInputRequired() const
{
	// see if we have any 'USER' types
	int nArg = m_aArgs.GetSize();

	while (nArg--)
	{
		switch (m_aArgs[nArg].nType)
		{
		case CLAT_USERFILE:
		case CLAT_USERFOLDER:
		case CLAT_USERTEXT:
		case CLAT_USERDATE:
			return TRUE;
		}
	}

	// not found
	return FALSE;
}

BOOL CToolsCmdlineParser::HasTasklistArgument() const
{
	// see if we have any tasklist related types
	int nArg = m_aArgs.GetSize();

	while (nArg--)
	{
		switch (m_aArgs[nArg].nType)
		{
		case CLAT_PATHNAME:
		case CLAT_FILETITLE:   
		case CLAT_FOLDER:        
		case CLAT_FILENAME:
			return TRUE;
		}
	}

	// not found
	return FALSE;
}

BOOL CToolsCmdlineParser::IsUserInputType(LPCTSTR szVarType)
{
	switch (GetType(szVarType))
	{
		case CLAT_USERFILE:
		case CLAT_USERFOLDER:
		case CLAT_USERTEXT:
		case CLAT_USERDATE:
			return TRUE;
	}

	return FALSE;
}

BOOL CToolsCmdlineParser::ReplaceArgument(int nArg, LPCTSTR szValue)
{
	if (nArg < 0 || nArg >= m_aArgs.GetSize())
		return FALSE;

	CMDLINEARG& cla = m_aArgs[nArg];

	if (m_sCmdLine.Replace(cla.sPlaceHolder, szValue))
	{
		// also check if there are any user references to this variable name
		// and replace those too
		int nUserArg = m_aUserArgs.GetSize();

		while (nUserArg--)
		{
			CMDLINEARG& claUser = m_aUserArgs[nUserArg];

			if (claUser.sName.CompareNoCase(cla.sName) == 0)
			{
				m_sCmdLine.Replace(claUser.sPlaceHolder, szValue);

				// and remove
				m_aUserArgs.RemoveAt(nUserArg);
			}
		}

		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CToolsCmdlineParser::HasArgument(CLA_TYPE nType) const
{
	int nArg = m_aArgs.GetSize();

	while (nArg--)
	{
		if (m_aArgs[nArg].nType == nType)
			return TRUE;
	}

	return FALSE;
}

void CToolsCmdlineParser::ParseCmdLine()
{
	CString sCmdLine(m_sCmdLine); // preserve original

	int n$Find = sCmdLine.Find('$');

	while (-1 != n$Find)
	{
		// find opening bracket
		int nOpenFind = sCmdLine.Find('(', n$Find);

		if (nOpenFind == -1)
			break;

		// find closing bracket
		int nCloseFind = sCmdLine.Find(')', nOpenFind);

		if (nCloseFind == -1)
			break;

		// parse variable in the form of (vartype, varname, varlabel, vardefvalue)
		CString sVarArgs = sCmdLine.Mid(nOpenFind + 1, nCloseFind - nOpenFind - 1);
		CString sType, sName, sLabel, sDefValue;

		int nComma1Find = sVarArgs.Find(','), nComma2Find = -1;

		if (nComma1Find != -1)
		{
			sType = sVarArgs.Left(nComma1Find);

			nComma2Find = sVarArgs.Find(',', nComma1Find + 1);

			if (nComma2Find != -1)
			{
				sName = sVarArgs.Mid(nComma1Find + 1, nComma2Find - nComma1Find - 1);

				int nComma3Find = sVarArgs.Find(',', nComma2Find + 1);

				if (nComma3Find != -1)
				{
					// this comma can either be a comma in the label string
					// or the delimeter before 'vardefvalue'
					// we determine which of these it is by looking for double quotes
					int nQuoteStartFind = sVarArgs.Find('\"', nComma2Find + 1);

					// and seeing if they preceed nComma3Find
					if (nQuoteStartFind != -1 && nQuoteStartFind < nComma3Find)
					{
						// now look for closing quotes
						int nQuoteEndFind = sVarArgs.Find('\"', nQuoteStartFind + 1);

						if (nQuoteEndFind != -1) // test for nComma3Find again cos it was previously a false find
							nComma3Find = sVarArgs.Find(',', nQuoteEndFind + 1);
						else
							nComma3Find = -1; // safest thing to do because no end quotes found
					}
						
					if (nComma3Find != -1)
					{
						sLabel = sVarArgs.Mid(nComma2Find + 1, nComma3Find - nComma2Find - 1);
						sDefValue = sVarArgs.Mid(nComma3Find + 1);

						sDefValue.Replace("\"", ""); // remove double quotes
					}
					else
						sLabel = sVarArgs.Mid(nComma2Find + 1);
				}
				else
					sLabel = sVarArgs.Mid(nComma2Find + 1);

				sLabel.Replace("\"", ""); // remove double quotes
			}
			else
				sName = sVarArgs.Mid(nComma1Find);
		}
		else
			sType = sVarArgs;

		// process
		sType.TrimLeft();
		sType.TrimRight();
		sName.TrimLeft();
		sName.TrimRight();
		sName.MakeLower();
		sLabel.TrimLeft();
		sLabel.TrimRight();
		sDefValue.TrimLeft();
		sDefValue.TrimRight();

		CMDLINEARG cla;

		cla.nType = GetType(sType);
		cla.sPlaceHolder = sCmdLine.Mid(n$Find, nCloseFind - n$Find + 1);

		switch (cla.nType)
		{
		// user input types must have a valid name that is not a 'keyword'
		case CLAT_USERFILE:
		case CLAT_USERFOLDER:
		case CLAT_USERTEXT:
		case CLAT_USERDATE:
			ASSERT(!sName.IsEmpty() && GetType(sName) == CLAT_NONE);
			
			if (!sName.IsEmpty() && GetType(sName) == CLAT_NONE)
			{
				cla.sName = sName;
				cla.sLabel = sLabel;
				cla.sDefValue = sDefValue;

				m_aArgs.Add(cla);
			}
			break;

		case CLAT_TODOLIST:      
		case CLAT_PATHNAME:      
		case CLAT_FILETITLE:    
		case CLAT_FOLDER:        
		case CLAT_FILENAME:      
		case CLAT_TODAYSDATE:
		case CLAT_SELTASKID:
		case CLAT_SELTASKTITLE:
		case CLAT_SELTASKEXTID:
		case CLAT_SELTASKCOMMENTS:
		case CLAT_SELTASKFILELINK:
		case CLAT_SELTASKALLOCBY:
		case CLAT_SELTASKALLOCTO:      
			m_aArgs.Add(cla);
			break;

		default: // it might a reference to a user type (or it may be a mistake)
			ASSERT (cla.nType == CLAT_NONE); // what else?
			cla.sName = sType;
			m_aUserArgs.Add(cla);
			break;
		}

		// next arg
		n$Find = sCmdLine.Find('$', nCloseFind);
	}
}

CLA_TYPE CToolsCmdlineParser::GetType(LPCTSTR szVarType)
{
	CString sType(szVarType);
	sType.TrimLeft();
	sType.TrimRight();
	sType.MakeLower();

	CLA_TYPE nType = CLAT_NONE;
	s_mapTypes.Lookup(sType, nType);

	return nType;
}
