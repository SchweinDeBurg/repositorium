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
// - added AbstractSpoon Software copyright notice and licenese information
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

// EnBitmap.h: interface for the CEnBitmap class.
//
// credits: Peter Hendrix's CPicture implementation for the original IPicture code
//          Yves Maurer's GDIRotate implementation for the idea of working directly on 32 bit representations of bitmaps
//          Karl Lager's 'A Fast Algorithm for Rotating Bitmaps'
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENBITMAPEX_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_)
#define AFX_ENBITMAPEX_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EnBitmap.h"


/////////////////////////////////////////////////////////////////////////////////////

class CEnBitmapEx : public CEnBitmap
{
public:
	CEnBitmapEx(COLORREF crBkgnd = RGB(255, 255, 255));
	virtual ~CEnBitmapEx();

	BOOL LoadSysBitmap(UINT nID);

	BOOL RotateImage(int nDegrees, BOOL bEnableWeighting = FALSE); // rotates about centrepoint, +ve (clockwise) or -ve (anticlockwise) from 12 o'clock
	BOOL ShearImage(int nHorz, int nVert, BOOL bEnableWeighting = FALSE); // shears +ve (right, down) or -ve (left, up)
	BOOL GrayImage(COLORREF crMask = -1);
	BOOL BlurImage(int nAmount = 5);
	BOOL SharpenImage(int nAmount = 5);
	BOOL ResizeImage(double dFactor);
	BOOL FlipImage(BOOL bHorz, BOOL bVert = 0);
	BOOL NegateImage();
	BOOL ReplaceColor(COLORREF crFrom, COLORREF crTo);
	BOOL LightenImage(double dAmount, COLORREF crMask = -1);
	BOOL ColorizeImage(COLORREF color); // converts to grayscale first
	BOOL ContrastImage(int nAmount);
	BOOL TintImage(COLORREF color, int nAmount, COLORREF crMask = -1);
	BOOL RemapSysColors(); // just like AfxLoadSysColorBitmap does

};


#endif // !defined(AFX_ENBITMAPEX_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_)
