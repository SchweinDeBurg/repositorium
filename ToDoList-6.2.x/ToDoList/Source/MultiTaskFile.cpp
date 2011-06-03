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
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// MultiTaskFile.cpp: implementation of the CMultiTaskFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "MultiTaskFile.h"
#include "TaskFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiTaskFile::CMultiTaskFile()
{
}

CMultiTaskFile::~CMultiTaskFile()
{
	// cleanup
	int nTaskFile = GetTaskListCount();

	while (nTaskFile--)
	{
		delete m_aTaskFiles[nTaskFile];
	}
}

CTaskFile& CMultiTaskFile::GetTaskFile(int nTaskFile)
{
	while (m_aTaskFiles.GetSize() <= nTaskFile)
	{
		CTaskFile* pTF = new CTaskFile;
		m_aTaskFiles.Add(pTF);
	}

	return *(m_aTaskFiles[nTaskFile]);
}

HRESULT CMultiTaskFile::QueryInterface(REFIID /*riid*/, void __RPC_FAR* __RPC_FAR* ppvObject)
{
	*ppvObject = reinterpret_cast<ITaskList*>(this);

	// compare riid against our supported version numbers
	/*
		if (IsEqualIID(riid, IID_TASKLIST))
		{
			*ppvObject = reinterpret_cast<ITaskList*>(this);
			AddRef();
		}
		else if (IsEqualIID(riid, IID_TASKLIST2))
		{
			*ppvObject = reinterpret_cast<ITaskList2*>(this);
			AddRef();
		}
		else if (IsEqualIID(riid, IID_TASKLIST3))
		{
			*ppvObject = reinterpret_cast<ITaskList3*>(this);
			AddRef();
		}
		else if (IsEqualIID(riid, IID_TASKLIST4))
		{
			*ppvObject = reinterpret_cast<ITaskList4*>(this);
			AddRef();
		}
		else if (IsEqualIID(riid, IID_TASKLIST5))
		{
			*ppvObject = reinterpret_cast<ITaskList5*>(this);
			AddRef();
		}
		else if (IsEqualIID(riid, IID_TASKLIST6))
		{
			*ppvObject = reinterpret_cast<ITaskList6*>(this);
			AddRef();
		}
		else if (IsEqualIID(riid, IID_TASKLIST7))
		{
			*ppvObject = reinterpret_cast<ITaskList7*>(this);
			AddRef();
		}
	*/

	return (*ppvObject ? S_OK : E_NOTIMPL);
}

int CMultiTaskFile::GetTaskListCount() const
{
	return m_aTaskFiles.GetSize();
}

const ITaskList* CMultiTaskFile::GetTaskList(int nTaskList) const
{
	ASSERT(m_aTaskFiles.GetSize() > nTaskList);

	if (m_aTaskFiles.GetSize() <= nTaskList)
	{
		return NULL;
	}

	// else
	return m_aTaskFiles[nTaskList];
}
