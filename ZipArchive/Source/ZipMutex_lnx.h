////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2007 by Artpol Software - Tadeusz Dracz
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// For the licensing details refer to the License.txt file.
//
// Web Site: http://www.artpol-software.com
////////////////////////////////////////////////////////////////////////////////

#ifndef ZIPARCHIVE_ZIPMUTEX_DOT_H
	#error Do not include this file directly. Include ZipMutex.h instead
#endif

#ifdef ZIP_ARCHIVE_USE_LOCKING

#include <pthread.h>

namespace ZipArchiveLib
{

class ZIP_API CZipMutex
{
	pthread_mutex_t m_mutex;
public:
	CZipMutex()
	{
		if (pthread_mutex_init(&m_mutex, NULL) != 0)
			CZipException::Throw(CZipException::mutexError);
	}

	void Lock()
	{
		if (pthread_mutex_lock(&m_mutex) != 0)
			CZipException::Throw(CZipException::mutexError);
	}

	void Unlock()
	{
		if (pthread_mutex_unlock(&m_mutex) != 0)
			CZipException::Throw(CZipException::mutexError);
	}

	~CZipMutex()
	{		 
		if (pthread_mutex_destroy(&m_mutex) != 0)
			CZipException::Throw(CZipException::mutexError);
	}
};

}

#endif
