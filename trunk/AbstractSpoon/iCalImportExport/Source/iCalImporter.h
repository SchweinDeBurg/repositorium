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

// iCalImporter.h: interface for the CiCalImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICALIMPORTER_H__36D6AF21_80EA_4361_85E9_B9BCDB38F913__INCLUDED_)
#define AFX_ICALIMPORTER_H__36D6AF21_80EA_4361_85E9_B9BCDB38F913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

class CiCalImporter : public IImportTasklist
{
public:
	CiCalImporter();
	virtual ~CiCalImporter();

	// interface implementation
	void Release()
	{
		delete this;
	}

	// caller must copy only
	const char* GetMenuText()
	{
		return "iCalendar";
	}
	const char* GetFileFilter()
	{
		return "iCalendar Files (*.ics)|*.ics||";
	}
	const char* GetFileExtension()
	{
		return "ics";
	}

	bool Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile);
};

#endif // !defined(AFX_ICALIMPORTER_H__36D6AF21_80EA_4361_85E9_B9BCDB38F913__INCLUDED_)
