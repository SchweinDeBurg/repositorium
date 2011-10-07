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

// MultiTaskFile.h: interface for the CMultiTaskFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTITASKFILE_H__3896047E_FE76_4EDC_9F97_191CABB982C0__INCLUDED_)
#define AFX_MULTITASKFILE_H__3896047E_FE76_4EDC_9F97_191CABB982C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "../../Common/ITaskList.h"

class CTaskFile;

class CMultiTaskFile : public IMultiTaskList
{
public:
	CMultiTaskFile();
	virtual ~CMultiTaskFile();

	CTaskFile& GetTaskFile(int nTaskFile = 0);

	/////////////////////////////////////////////////////
	// IMultiTaskList implementation
	virtual int GetTaskListCount() const;
	virtual const ITaskList* GetTaskList(int nTaskList = 0) const;

	/////////////////////////////////////////////////////
	// IUnknown implementation
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR* __RPC_FAR* ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return 1;   // do nothing
	}
	ULONG STDMETHODCALLTYPE Release(void)
	{
		return 1;   // do nothing
	}

protected:
	CArray<CTaskFile*, CTaskFile*&> m_aTaskFiles;
};

#endif // !defined(AFX_MULTITASKFILE_H__3896047E_FE76_4EDC_9F97_191CABB982C0__INCLUDED_)
