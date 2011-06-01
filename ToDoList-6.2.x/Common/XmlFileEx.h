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
// - adjusted #include's paths
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

// XmlFileEx.h: interface for the CXmlFileEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLFILEEX_H__C7DFA5B5_2B36_4D63_942C_9054EF4240CB__INCLUDED_)
#define AFX_XMLFILEEX_H__C7DFA5B5_2B36_4D63_942C_9054EF4240CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XmlFile.h"

class IEncryption;

enum
{
	XFL_NOENCRYPTIONDLL     = XFL_LAST - 1,
	XFL_UNKNOWNENCRYPTION   = XFL_LAST - 2,
};

class CXmlFileEx : public CXmlFile
{
public:
	CXmlFileEx(LPCTSTR szRootItemName = NULL, LPCTSTR szPassword = NULL);
	virtual ~CXmlFileEx();

	void SetPassword(LPCTSTR szPassword)
	{
		m_sPassword = szPassword;
	}
	CString GetPassword()
	{
		return m_sPassword;
	}

	// allows for selective decrypting
	BOOL Load(LPCTSTR szFilePath, LPCTSTR szRootItemName = NULL, IXmlParse* pCallback = NULL, BOOL bDecrypt = TRUE);
	BOOL Open(LPCTSTR szFilePath, XF_OPEN nOpenFlags, BOOL bDecrypt = TRUE);
	virtual BOOL LoadEx(LPCTSTR szRootItemName = NULL, IXmlParse* pCallback = NULL);

	// call before standard save
	virtual BOOL Encrypt(LPCTSTR szPassword = NULL);
	virtual BOOL Decrypt(LPCTSTR szPassword = NULL);
	static BOOL CanEncrypt(); // false if encryptor dll cannot be loaded

	static void SetUIStrings(UINT nIDPasswordExplanation, UINT nIDDecryptFailed);

protected:
	IEncryption* m_pEncryptor;
	CString m_sPassword;
	BOOL m_bDecrypt;

	static CString s_sPasswordExplanation, s_sDecryptFailed;

protected:
	BOOL Decrypt(LPCTSTR szInput, CString& sOutput, LPCTSTR szPassword);
	BOOL InitEncryptor();
	BOOL IsEncrypted();
	CXmlItem* GetEncryptedBlock();
};

#endif // !defined(AFX_XMLFILEEX_H__C7DFA5B5_2B36_4D63_942C_9054EF4240CB__INCLUDED_)
