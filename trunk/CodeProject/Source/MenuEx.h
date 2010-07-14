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

// MenuEx.h: interface for the CMenuEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUEX_H__5AB11CC8_CCF5_4D52_ADC7_27FDC151F3FE__INCLUDED_)
#define AFX_MENUEX_H__5AB11CC8_CCF5_4D52_ADC7_27FDC151F3FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "EnBitmapEx.h"

enum MENUEX_BTN
{
	MEB_MINIMIZE,
	MEB_RESTORE,
	MEB_CLOSE,
};

class CMenuEx : public CMenu  
{
public:
	CMenuEx();
	virtual ~CMenuEx();

	void SetBackgroundColor(COLORREF color);
	
	// pass -1 as nThemeBMID is you want ownerdraw
	BOOL AddMDIButton(MENUEX_BTN nBtn, UINT nCmdID, BOOL bRightJustify = TRUE);
	BOOL DeleteMDIMenu(UINT nCmdID);

	// for themed buttons only
	BOOL DrawMDIButton(LPDRAWITEMSTRUCT lpDrawItemStruct); 
	BOOL MeasureMDIButton(LPMEASUREITEMSTRUCT lpMeasureItemStruct); 

protected:
	CMap<UINT, UINT, int, int> m_mapCmd2ID;
	CBrush m_brBkgnd;

protected:
	static BOOL IsThemed();
};

#endif // !defined(AFX_MENUEX_H__5AB11CC8_CCF5_4D52_ADC7_27FDC151F3FE__INCLUDED_)
