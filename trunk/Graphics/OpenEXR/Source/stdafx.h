///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////



#ifndef INCLUDED_STDAFX_H
#define INCLUDED_STDAFX_H

// prevent automatic CRT/STL/MFC/ATL manifest dependencies generation

// see http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx and
// http://blog.m-ri.de/index.php/2008/05/06/hotfix-fuer-usemsprivateassembliesh-und-vc-2008/
// by by Martin Richter for more information

#if defined(_MSC_VER) && (_MSC_VER >= 1400) && !defined(UNDER_CE)
// VC++ 2005 (8.0) or greater

#define _CRT_NOFORCE_MANIFEST
#define _STL_NOFORCE_MANIFEST
#define _AFX_NOFORCE_MANIFEST
#define _ATL_NOFORCE_MANIFEST

#ifdef __cplusplus
extern "C"
{
#endif

__declspec(selectany) int _forceCRTManifest;
__declspec(selectany) int _forceMFCManifest;
__declspec(selectany) int _forceAtlDllManifest;

// the next symbols are used by the several versions of VC++ 2008 (9.0)
__declspec(selectany) int _forceCRTManifestRTM;
__declspec(selectany) int _forceMFCManifestRTM;
__declspec(selectany) int _forceMFCManifestCUR;

#ifdef __cplusplus
}   // extern "C"
#endif

#endif   // _MSC_VER && !UNDER_CE

#include <assert.h>
#include <float.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>

#include <limits>
#include <iostream>
#include <iomanip>
#include <string>
#include <exception>
#include <sstream>
#include <map>
#include <algorithm>
#include <set>
#include <iosfwd>
#include <fstream>
#include <vector>
#include <cmath>
#include <list>
#include <iomanip>
#include <complex>

#include <zlib.h>

#if defined _WIN32 || defined _WIN64
    #ifdef NOMINMAX
        #undef NOMINMAX
    #endif
    #define NOMINMAX
    #include <windows.h>
    #include <process.h>
#elif HAVE_PTHREAD
    #include <pthread.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#endif
