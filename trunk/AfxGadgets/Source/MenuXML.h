// AfxGadgets library.
// Copyright (c) 2004-2010 by Elijah Zarezky,
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

// MenuXML.h - interface of the CMenuXML class

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__MenuXML_h)
#define __MenuXML_h

class CMenuXML: public CMenu
{
	DECLARE_DYNCREATE(CMenuXML)

// construction/destruction
public:
	CMenuXML(void);
	virtual ~CMenuXML(void);

// initialization
public:
	static const MENUTEMPLATE* CreateMenuExTemplate(LPCTSTR pszFileXML);

// operations
public:
	BOOL CreateMenuXML(LPCTSTR pszMenuName);

// overridables
protected:
	virtual void GetXMLpath(CString& strDest);

// implementation helpers
private:
	static void RecurseMenuTree(CMemFile& fileMem, CPugXmlBranch& branchMenu);
	struct MENU_FLAG { LPCTSTR pszName; DWORD fdwValue; };
	static DWORD ParseMenuFlags(MENU_FLAG amfDict[], LPCTSTR pszFlagsStr);
	// MFT_* parser
	static DWORD ParseMenuItemType(LPCTSTR pszTypeStr);
	// MFS_* parser
	static DWORD ParseMenuItemState(LPCTSTR pszStateStr);

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __MenuXML_h

// end of file
