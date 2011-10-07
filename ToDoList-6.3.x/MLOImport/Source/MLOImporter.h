// Copyright (C) 2003-2011 AbstractSpoon Software.
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
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// MLOImporter.h: interface for the CMLOImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MLOIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_)
#define AFX_MLOIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"

class CXmlItem;

class CMLOImporter : public IImportTasklist
{
public:
	CMLOImporter();
	virtual ~CMLOImporter();

	void Release()
	{
		delete this;
	}
	const char* GetMenuText()
	{
		return "My Life Organized";
	}
	const char* GetFileFilter()
	{
		return "MLO Task Files (*.xml)|*.xml||";
	}
	const char* GetFileExtension()
	{
		return "xml";
	}

	bool Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile);

protected:
	bool ImportTask(const CXmlItem* pXIMLOTask, ITaskList5* pDestTaskFile, HTASKITEM hParent) const;
	time_t GetDate(const CXmlItem* pXIMLOTask, LPCTSTR szDateField) const;

};

#endif // !defined(AFX_MLOIMPORTER_H__E1C1DB38_D45E_481E_8D91_7D8455C5155E__INCLUDED_)
