// XMLMenus application.
// Copyright (c) 2005 by Elijah Zarezky,
// All rights reserved.

// HotKeysXML.h - interface of the CHotKeysXML class

#if !defined(__HotKeysXML_h)
#define __HotKeysXML_h

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
