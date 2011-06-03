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
//*****************************************************************************

// UIExtensionUIHelper.h: interface for the CUIExtensionUIHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIEXTENSIONUIHELPER_H__AC709E3B_C876_43D8_A965_CAD2D2E7AD34__INCLUDED_)
#define AFX_UIEXTENSIONUIHELPER_H__AC709E3B_C876_43D8_A965_CAD2D2E7AD34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CUIExtensionMgr;
class CMenuIconMgr;

class CUIExtensionUIHelper
{
public:
	CUIExtensionUIHelper(const CUIExtensionMgr& mgrUIExt, UINT nStart, int nSize = 16);
	virtual ~CUIExtensionUIHelper();

	void UpdateMenu(CCmdUI* pCmdUI, CMenuIconMgr& mgrIcon, BOOL bEnabled = TRUE) const;
	void AppendExtensionsToToolbar(CToolBar& toolbar, UINT nCmdAfter);
	void RemoveExtensionsFromToolbar(CToolBar& toolbar, UINT nCmdAfter);

protected:
	const CUIExtensionMgr& m_mgrUIExt;
	UINT m_nStartID;
	int m_nSize;
};

#endif // !defined(AFX_UIEXTENSIONUIHELPER_H__AC709E3B_C876_43D8_A965_CAD2D2E7AD34__INCLUDED_)
