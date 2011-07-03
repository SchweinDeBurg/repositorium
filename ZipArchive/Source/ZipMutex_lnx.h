////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000 - 2011 by Artpol Software - Tadeusz Dracz
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
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --min-conditional-indent=0
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - implemented support for the Windows Mobile/CE tragets
// - added possibility to seamless usage in the ATL-based projects
////////////////////////////////////////////////////////////////////////////////

#ifndef ZIPARCHIVE_ZIPMUTEX_DOT_H
	#error Do not include this file directly. Include ZipMutex.h instead
#endif

#ifdef _ZIP_USE_LOCKING

#include <pthread.h>

namespace ZipArchiveLib
{

	class ZIP_API CZipMutex
	{
		pthread_mutex_t m_mutex;
		bool m_bOpened;
	public:
		CZipMutex(bool bOpen = false)
		{
			if (bOpen)
			{
				Open();
			}
			else
			{
				m_bOpened = false;
			}
		}

		void Open()
		{
			Close();
			if (pthread_mutex_init(&m_mutex, NULL) == 0)
			{
				m_bOpened = true;
			}
			else
			{
				CZipException::Throw(CZipException::mutexError);
			}
		}

		void Lock()
		{
			if (pthread_mutex_lock(&m_mutex) != 0)
			{
				CZipException::Throw(CZipException::mutexError);
			}
		}

		void Unlock()
		{
			if (pthread_mutex_unlock(&m_mutex) != 0)
			{
				CZipException::Throw(CZipException::mutexError);
			}
		}

		CZipMutex& operator=(const CZipMutex&)
		{
			m_bOpened = false;
			return *this;
		}


		void Close()
		{
			if (m_bOpened)
				if (pthread_mutex_destroy(&m_mutex) == 0)
				{
					m_bOpened = false;
				}
				else
				{
					CZipException::Throw(CZipException::mutexError);
				}
		}

		~CZipMutex()
		{
			Close();
		}
	};

} // namespace

#endif
