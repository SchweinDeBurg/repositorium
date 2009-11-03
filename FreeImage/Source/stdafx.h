// ==========================================================
// Include file for standard system include files
//
// Design and implementation by
// - Elijah Zarezky (elijah.zarezky@gmail.com)
//
// COVERED CODE IS PROVIDED UNDER THIS LICENSE ON AN "AS IS" BASIS, WITHOUT WARRANTY
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, WITHOUT LIMITATION, WARRANTIES
// THAT THE COVERED CODE IS FREE OF DEFECTS, MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE
// OR NON-INFRINGING. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE COVERED
// CODE IS WITH YOU. SHOULD ANY COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT
// THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY
// SERVICING, REPAIR OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
// PART OF THIS LICENSE. NO USE OF ANY COVERED CODE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
//
// Use at your own risk!
// ==========================================================

#ifndef STDAFX_H
#define STDAFX_H

// remove automatic CRT/MFC dependencies generation

// see http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx and
// http://blog.m-ri.de/index.php/2008/05/06/hotfix-fuer-usemsprivateassembliesh-und-vc-2008/
// by by Martin Richter for more information

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
// VC++ 8.0 or greater

#define _STL_NOFORCE_MANIFEST
#define _CRT_NOFORCE_MANIFEST
#define _AFX_NOFORCE_MANIFEST
#define _ATL_NOFORCE_MANIFEST

#ifdef __cplusplus
extern "C"
{
#endif

__declspec(selectany) int _forceCRTManifest;
__declspec(selectany) int _forceMFCManifest;
__declspec(selectany) int _forceAtlDllManifest;

// the next symbols are used by the several versions of VC++ 9.0
__declspec(selectany) int _forceCRTManifestRTM;
__declspec(selectany) int _forceMFCManifestRTM;
__declspec(selectany) int _forceMFCManifestCUR;

#ifdef __cplusplus
}   // extern "C"
#endif

#endif   // _MSC_VER

#include <wchar.h>   // needed for UNICODE functions

#include <math.h>
#include <stdlib.h> 
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <sstream>
#include <algorithm>

#include <malloc.h>
#include <float.h>

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlconv.h>
#include <io.h>
#endif // _WIN32

#include <sys/stat.h>

#include <zlib.h>

#endif // STDAFX_H
