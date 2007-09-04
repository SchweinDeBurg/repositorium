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

/**
* \file DeflateCompressor.h
*	Includes the ZipArchiveLib::CDeflateCompressor class.
*
*/

#if !defined(ZIPARCHIVE_DEFLATECOMPRESSOR_DOT_H)
#define ZIPARCHIVE_DEFLATECOMPRESSOR_DOT_H

#if _MSC_VER > 1000
#pragma once
#endif

#include "ZipExport.h"
#include "BaseLibCompressor.h"
#include "ZipException.h"
#include "_features.h"
#include "../../zlib/Source/zlib.h"

namespace ZipArchiveLib
{

/**
	Compresses and decompresses data using the Zlib library.
*/
class ZIP_API CDeflateCompressor : public CBaseLibCompressor
{		
	bool m_bCheckLastBlock;
public:

	/**
		Initializes a new instance of the CDeflateCompressor class.

		\param pStorage
			The current storage object.
		
		\param pBuffer
			A pre-allocated buffer that receives compressed data or provides data for decompression.

		\param bDetectLibMemoryLeaks
			\c true, if the ZipArchive Library should detect memory leaks in an external library; \c false otherwise. 
			Recommended to be set to \c true.
	*/
	CDeflateCompressor(CZipStorage* pStorage, CZipAutoBuffer* pBuffer, bool bDetectLibMemoryLeaks = true);

	bool CanProcess(WORD uMethod) {return uMethod == methodStore || uMethod == methodDeflate;}

	void InitCompression(int iLevel, CZipFileHeader* pFile, CZipCryptograph* pCryptograph);
	void InitDecompression(CZipFileHeader* pFile, CZipCryptograph* pCryptograph);

	DWORD Decompress(void *pBuffer, DWORD uSize);
	void Compress(const void *pBuffer, DWORD uSize);
	
	void FinishCompression(bool bAfterException);
	void FinishDecompression(bool bAfterException);

	/**
		Enables or disables checking, if the compressed data ends with a correct block.
		This should be enabled to protect against malformed data.
		
		\param bCheckLastBlock
			\c true, if the checking of the last block should be enabled; \c false otherwise.

		\see 
			GetCheckLastBlock
	*/
	void SetCheckLastBlock(bool bCheckLastBlock)
	{
		m_bCheckLastBlock = bCheckLastBlock;
	}

	/**
		Returns the value indicating whether checking of the last block is enabled or not.

		\return
			\c true, if checking of the last block is enabled; \c false otherwise.

		\see 
			SetCheckLastBlock
	*/
	bool GetCheckLastBlock()
	{
		return m_bCheckLastBlock;
	}

protected:
	int ConvertInternalError(int iErr)
	{
		switch (iErr)
		{
		case Z_NEED_DICT:
			return CZipException::needDict;
		case Z_STREAM_END:
			return CZipException::streamEnd;
		case Z_ERRNO:
			return CZipException::errNo;
		case Z_STREAM_ERROR:
			return CZipException::streamError;
		case Z_DATA_ERROR:
			return CZipException::dataError;
		case Z_MEM_ERROR:
			return CZipException::memError;
		case Z_BUF_ERROR:
			return CZipException::bufError;
		case Z_VERSION_ERROR:
			return CZipException::versionError;
		default:
			return CZipException::genericError;
		}
	}

	bool IsCodeErrorOK(int iErr)
	{
		return iErr == Z_OK || iErr == Z_NEED_DICT;
	}
private:		
	zarch_z_stream m_stream;
	
};

} // namespace

#endif

