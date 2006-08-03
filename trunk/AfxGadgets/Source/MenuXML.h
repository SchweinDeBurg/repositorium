// AfxGadgets library.
// Copyright (c) 2005-2006 by Elijah Zarezky,
// All rights reserved.

// MenuXML.h - interface of the CMenuXML class

#if !defined(__MenuXML_h)
#define __MenuXML_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

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
