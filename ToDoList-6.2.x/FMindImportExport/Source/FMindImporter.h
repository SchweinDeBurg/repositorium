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
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
//*****************************************************************************

// FMindImporter.h: interface for the CFMindImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FMINDIMPORTER_H__48AE3CCE_E042_432D_B5DB_D7E310CF99CE__INCLUDED_)
#define AFX_FMINDIMPORTER_H__48AE3CCE_E042_432D_B5DB_D7E310CF99CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/IImportExport.h"
#include "../../Common/ITaskList.h"
#include "../../Common/XmlFile.h"

class CFMindImporter : public IImportTasklist
{
public:
	CFMindImporter();
	virtual ~CFMindImporter();
	// interface implementation
	void Release()
	{
		delete this;
	}

	// caller must copy only
	const char* GetMenuText()
	{
		return "FreeMind";
	}
	const char* GetFileFilter()
	{
		return "FreeMind Files (*.mm)|*.mm||";
	}
	const char* GetFileExtension()
	{
		return "mm";
	}

	bool Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile);

protected:
	bool ImportTask(const CXmlItem* pFMTask, ITaskList7* pDestTaskFile, HTASKITEM hParent) const;
	long GetColor(const CXmlItem* pFMTask, LPCTSTR szColorField) const;
	long GetDate(const CXmlItem* pFMTask, LPCTSTR szColorField) const;
	CString GetAttribValueS(const CXmlItem* pFMTask , const char* szAttribName) const;
	int GetAttribValueI(const CXmlItem* pFMTask , const char* szAttribName) const;
	bool GetAttribValueB(const CXmlItem* pFMTask , const char* szAttribName) const;
	double GetAttribValueD(const CXmlItem* pFMTask , const char* szAttribName) const;
	CString GetTaskRichContent(const CXmlItem* pFMTask , const char* szRichType) const;

};

#endif // !defined(AFX_FMINDIMPORTER_H__48AE3CCE_E042_432D_B5DB_D7E310CF99CE__INCLUDED_)
