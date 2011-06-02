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
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
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

// EnBitmap.cpp: implementation of the CEnBitmapEx class (c) daniel godson 2002.
//
// credits: Peter Hendrix's CPicture implementation for the original IPicture code
//          Yves Maurer's GDIRotate implementation for the idea of working directly on 32 bit representations of bitmaps
//          Karl Lager's 'A Fast Algorithm for Rotating Bitmaps'
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnBitmapEx.h"
#include "ImageProcessors.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnBitmapEx::CEnBitmapEx(COLORREF crBkgnd) : CEnBitmap(crBkgnd)
{
}

CEnBitmapEx::~CEnBitmapEx()
{
}

BOOL CEnBitmapEx::LoadSysBitmap(UINT nID)
{
	if (LoadBitmap(nID))
	{
		return RemapSysColors();
	}

	return FALSE;
}

BOOL CEnBitmapEx::RotateImage(int nDegrees, BOOL bEnableWeighting)
{
	CImageRotator proc(nDegrees, bEnableWeighting);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ShearImage(int nHorz, int nVert, BOOL bEnableWeighting)
{
	CImageShearer proc(nHorz, nVert, bEnableWeighting);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::GrayImage(COLORREF crMask)
{
	CImageGrayer proc;
	return ProcessImage(&proc, crMask);
}

BOOL CEnBitmapEx::BlurImage(int nAmount)
{
	CImageBlurrer proc(nAmount);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::SharpenImage(int nAmount)
{
	CImageSharpener proc(nAmount);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ResizeImage(double dFactor)
{
	CImageResizer proc(dFactor);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::LightenImage(double dAmount, COLORREF crMask)
{
	CImageLightener proc(dAmount);
	return ProcessImage(&proc, crMask);
}

BOOL CEnBitmapEx::FlipImage(BOOL bHorz, BOOL bVert)
{
	CImageFlipper proc(bHorz, bVert);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::NegateImage()
{
	CImageNegator proc;
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ReplaceColor(COLORREF crFrom, COLORREF crTo)
{
	CColorReplacer proc(crFrom, crTo);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ColorizeImage(COLORREF color)
{
	CImageColorizer proc(color);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::ContrastImage(int nAmount)
{
	CImageContraster proc(nAmount);
	return ProcessImage(&proc);
}

BOOL CEnBitmapEx::TintImage(COLORREF color, int nAmount, COLORREF crMask)
{
	CImageTinter proc(color, nAmount);
	return ProcessImage(&proc, crMask);
}

BOOL CEnBitmapEx::RemapSysColors()
{
	CImageSysColorMapper proc;
	return ProcessImage(&proc);
}
