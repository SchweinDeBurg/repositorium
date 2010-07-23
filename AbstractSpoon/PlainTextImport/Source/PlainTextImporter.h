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

// PlainTextImporter.h: interface for the CPlainTextImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAINTEXTIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_)
#define AFX_PLAINTEXTIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"

class CPlainTextImporter : public IImportTasklist
{
public:
	CPlainTextImporter();
	virtual ~CPlainTextImporter();

	void Release()
	{
		delete this;
	}
	const char* GetMenuText()
	{
		return "Outline";
	}
	const char* GetFileFilter()
	{
		return "Text Files (*.txt)|*.txt||";
	}
	const char* GetFileExtension()
	{
		return "txt";
	}

	bool Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile);

protected:
	CString INDENT;
	int ROOTDEPTH;

protected:
	int GetDepth(const CString& sLine); // returns the indent
	BOOL GetTitleComments(const CString& sLine, CString& sTitle, CString& sComments);
};

#endif // !defined(AFX_PLAINTEXTIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_)
