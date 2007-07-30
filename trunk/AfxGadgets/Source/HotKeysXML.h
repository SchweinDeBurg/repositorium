// AfxGadgets library.
// Copyright (c) 2005-2006 by Elijah Zarezky,
// All rights reserved.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// HotKeysXML.h - interface of the CHotKeysXML class

#if !defined(__HotKeysXML_h)
#define __HotKeysXML_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__AFXTEMPL_H__)
#pragma message(__FILE__ " : put <afxtempl.h> in your PCH to speed up compilation")
#include <afxtempl.h>
#endif	// __AFXTEMPL_H__

class CHotKeysXML: public CObject
{
	DECLARE_DYNAMIC(CHotKeysXML)

// construction/destruction
public:
	CHotKeysXML(void);
	virtual ~CHotKeysXML(void);

// operations
public:
	BOOL CreateTable(LPCTSTR pszTableName);
	BOOL DestroyTable(void);
	void AttachToFrame(CFrameWnd* pFrameWnd, BOOL fShowInMenu = TRUE);
	void DetachFromFrame(CFrameWnd* pFrameWnd);

// overridables
protected:
	virtual void GetXMLpath(CString& strDest);

// attributes
public:
	HACCEL m_hAccTable;
	CMap<WORD, WORD, CString, LPCTSTR> m_mapNames;

// implementation helpers
private:
	typedef CArray<ACCEL, ACCEL&> ACCEL_ARRAY;
	BOOL CreateAccelArray(LPCTSTR pszFileXML, ACCEL_ARRAY& arrAccel);
	WORD GetModifierFlag(LPCTSTR pszText);
	WORD GetVKeyCode(LPCTSTR pszText);
	void ParseHotKeyText(LPCTSTR pszText, ACCEL& accData);

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __HotKeysXML_h

// end of file
