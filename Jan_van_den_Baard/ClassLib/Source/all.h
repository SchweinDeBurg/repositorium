//
// all.h
//
// (C) Copyright 1999-2003 Jan van den Baard
//     All Rights Reserved.
//

#ifndef _ALL_H_
#define _ALL_H_

#define OEMRESOURCE

// CRT headers

// _CRTDBG_MAP_ALLOC is not #define-d because we use our own implementation
// of the new and delete operators (see application.cpp file)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <tchar.h>
#include <malloc.h>
#include <crtdbg.h>
#include <new.h>

// PSDK headers

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>

// ClassLib headers

#include "standard.h"
#include "application.h"
#include "ado/all.h"
#include "collectors/all.h"
#include "coords/all.h"
#include "menus/all.h"
#include "strings/all.h"
#include "threads/all.h"
#include "tools/all.h"
#include "windows/all.h"
#include "windows/common controls/all.h"
#include "windows/common dialogs/all.h"
#include "windows/controls/all.h"
#include "windows/custom controls/all.h"
#include "windows/custom controls/customtabctrl/all.h"
#include "gdi/all.h"
#include "exceptions/all.h"
#include "layout/all.h"
#include "io/all.h"
#include "shell/all.h"

// import libraries

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Comdlg32.lib")

#endif // _ALL_H_
