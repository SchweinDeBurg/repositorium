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
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// DlgUnits.h: interface for the CDlgUnits class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGUNITS_H__18921FC5_A659_4AA7_BDF8_EF630D0D13E6__INCLUDED_)
#define AFX_DLGUNITS_H__18921FC5_A659_4AA7_BDF8_EF630D0D13E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDlgUnits
{
public:
	CDlgUnits(HWND hWnd = NULL);
	CDlgUnits(CWnd* pWnd);
	virtual ~CDlgUnits();

	int ToPixelsX(int x) const;
	int ToPixelsY(int y) const;

	void ToPixels(long& x, long& y) const;
	void ToPixels(int& x, int& y) const;
	void ToPixels(short& x, short& y) const;
	void ToPixels(POINT& point) const;
	void ToPixels(RECT& rect) const;
	void ToPixels(SIZE& size) const;
	void FromPixels(long& x, long& y) const;
	void FromPixels(int& x, int& y) const;
	void FromPixels(short& x, short& y) const;
	void FromPixels(POINT& point) const;
	void FromPixels(RECT& rect) const;
	void FromPixels(SIZE& size) const;

protected:
	SIZE m_dlgBaseUnits;
	HWND m_hWnd;

};

#endif // !defined(AFX_DLGUNITS_H__18921FC5_A659_4AA7_BDF8_EF630D0D13E6__INCLUDED_)
