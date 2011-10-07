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

//
// Assorted defines and ids for the RulerRichEditCtrl
//

#ifndef _IDS_H_
#define _IDS_H_

// Some measures
#define TOOLBAR_HEIGHT      27
#define RULER_HEIGHT        26
#define TOP_HEIGHT          RULER_HEIGHT + TOOLBAR_HEIGHT
#define FONT_COMBO_WIDTH    128
#define SIZE_COMBO_WIDTH    48
#define COMBO_HEIGHT        256
#define COMBO_WIDTH         40
#define MIN_FONT_COMBO_WIDTH    60

#define FONT_NAME_POS       0
#define FONT_SIZE_POS       2

// Measures for the ruler
#define MODE_INCH   0
#define MODE_METRIC 1

// Mouse handling
extern UINT urm_RULERACTION;
extern UINT urm_GETSCROLLPOS;
#define UP      0
#define DOWN    1
#define MOVE    2

#endif // _IDS_H_
