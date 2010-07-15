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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

// XmlFileEx.cpp: implementation of the CXmlFileEx class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "XmlFileEx.h"
#include "StringRes.h"
#include "../../CodeProject/Source/EnString.h"
#include "IEncryption.h"
#include "PasswordDialog.h"
#include ".../../CodeProject/Source/Base64Coder.h"
#include "XmlNodeWrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString CXmlFileEx::s_sPasswordExplanation(ENCRYPT_ENTERPWD);
CString CXmlFileEx::s_sDecryptFailed(ENCRYPT_DECRYPTFAILED);

const LPCTSTR ENCODEDDATA = "ENCODEDDATA";
const LPCTSTR ENCODEDDATALEN = "DATALEN";

CXmlFileEx::CXmlFileEx(LPCTSTR szRootItemName, LPCTSTR szPassword)
: CXmlFile(szRootItemName), m_pEncryptor(NULL), m_sPassword(szPassword)
{

}

CXmlFileEx::~CXmlFileEx()
{

}

void CXmlFileEx::SetUIStrings(UINT nIDPasswordExplanation,
										UINT nIDDecryptFailed)
{
	s_sPasswordExplanation.LoadString(nIDPasswordExplanation);
	s_sDecryptFailed.LoadString(nIDDecryptFailed);
}


BOOL CXmlFileEx::Encrypt(LPCTSTR szPassword)
{
	if (!szPassword)
		szPassword = m_sPassword;

	if (!(*szPassword) || !InitEncryptor())
		return FALSE;

	// 1. export everything below the root to a string
	CXmlDocumentWrapper doc;
	doc.LoadXML("<a></a>");

	CXmlNodeWrapper nodeDoc(doc.AsNode());

	CString sXml;
	POSITION pos = m_xiRoot.GetFirstItemPos();
	int nNode = 0;

	while (pos)
	{
		const CXmlItem* pXI = m_xiRoot.GetNextItem(pos);
		ASSERT (pXI);

		while (pXI)
		{
			CString sItem = pXI->GetName();
			CXmlNodeWrapper nodeChild(nodeDoc.InsertNode(nNode++, (LPCTSTR)sItem));
			ASSERT (nodeChild.IsValid());

			Export(pXI, &nodeChild);
			sXml += nodeChild.GetXML();

			// siblings if there are any
			pXI = pXI->GetSibling();
		}
	}

	// 2. encrypt it
	unsigned char* pEncrypted = NULL;
	int nLenEncrypted = 0;

	if (!m_pEncryptor->Encrypt((unsigned char*)(LPCTSTR)sXml, sXml.GetLength() + 1, szPassword, // RB - Added sPassword parameter instead of NULL
		pEncrypted, nLenEncrypted))
		return FALSE;

	// 3. convert the binary to a string
	Base64Coder b64;

	b64.Encode(pEncrypted, nLenEncrypted);
	const char* pEncodedDataBuffer = b64.EncodedMessage();

	// 4. replace file contents with a single CDATA item
	m_xiRoot.DeleteAllItems();
	m_xiRoot.AddItem(ENCODEDDATA, pEncodedDataBuffer, XIT_CDATA);
	m_xiRoot.AddItem("DATALEN", nLenEncrypted);

	// 5. cleanup
	m_pEncryptor->FreeBuffer(pEncrypted);

	return TRUE;
}

BOOL CXmlFileEx::IsEncrypted()
{
	return (GetItemValueI(ENCODEDDATALEN) > 0);
}

BOOL CXmlFileEx::Decrypt(LPCTSTR szPassword)
{
	if (!IsEncrypted())
		return TRUE; // nothing to do

	// we don't try to decrypt if no encryption capabilities
	if (!CanEncrypt())
	{
		m_nFileError = XFL_NOENCRYPTIONDLL;
		return FALSE;
	}

	if (!szPassword)
		szPassword = m_sPassword;

	CXmlItem* pXI = GetEncryptedBlock();

	if (pXI && !pXI->GetSibling())
	{
		// else keep getting password till success or user cancels
		while (TRUE)
		{
			CString sPassword(szPassword);

			if (sPassword.IsEmpty())
			{
				CString sExplanation(s_sPasswordExplanation);

				if (sExplanation.Find("%s") != -1)
					sExplanation.Format(s_sPasswordExplanation, GetFileName());

				if (!CPasswordDialog::RetrievePassword(FALSE, sPassword, sExplanation))
				{
					// RB - Set m_nFileError to avoid "The selected task list could not be opened..." message when cancelling
					m_nFileError = XFL_CANCELLED;
					return FALSE;
				}
			}

			CString sFile;

			if (Decrypt(pXI->GetValue(), sFile, sPassword))
			{
				m_sPassword = sPassword;

				sFile.TrimLeft();
				sFile.TrimRight();
				sFile = "<ROOT>" + sFile + "</ROOT>";

				// delete the cdata item
				m_xiRoot.DeleteItem(pXI);

				try
				{
					CXmlDocumentWrapper doc;

					// reparse decrypted xml
					if (doc.LoadXML(sFile))
					{
						CXmlNodeWrapper node(doc.AsNode());

						return ParseItem(m_xiRoot, &node);
					}
				}
				catch (...)
				{
					m_nFileError = XFL_BADMSXML;
				}

				return FALSE;
			}
			// RB - Added code to format the error message before calling AfxMessage
			else
			{
				CString sMessage(s_sDecryptFailed);

				if (sMessage.Find("%s") != -1)
					sMessage.Format(s_sDecryptFailed, GetFileName());

				if (IDNO == AfxMessageBox(sMessage, MB_YESNO))
				{
					m_nFileError = XFL_CANCELLED;
					return FALSE;
				}
				// else user will try again
			}
		}
	}

	// else
	m_nFileError = XFL_UNKNOWNENCRYPTION;
	return FALSE;
}

CXmlItem* CXmlFileEx::GetEncryptedBlock()
{
	CXmlItem* pXI = NULL;
	int nDataLen = GetItemValueI(ENCODEDDATALEN);

	if (nDataLen)
	{
		pXI = GetItem(ENCODEDDATA);

		if (!pXI)
		{
			// backwards compatibility
			pXI = GetItem("CDATA");

			if (!pXI)
			{
				// missing tags (last ditched effort)
				pXI = &m_xiRoot;
			}
		}
	}

	return pXI;
}

BOOL CXmlFileEx::Load(LPCTSTR szFilePath, LPCTSTR szRootItemName, IXmlParse* pCallback, BOOL bDecrypt)
{
	m_bDecrypt = bDecrypt;

	return CXmlFile::Load(szFilePath, szRootItemName, pCallback);
}

BOOL CXmlFileEx::Open(LPCTSTR szFilePath, XF_OPEN nOpenFlags, BOOL bDecrypt)
{
	m_bDecrypt = bDecrypt;

	return CXmlFile::Open(szFilePath, nOpenFlags);
}

BOOL CXmlFileEx::LoadEx(LPCTSTR szRootItemName, IXmlParse* pCallback)
{
	if (!CXmlFile::LoadEx(szRootItemName, pCallback))
		return FALSE;

	// we assume the file is encrypted if it contains a single CDATA element
	if (m_bDecrypt)
		return Decrypt();

	return TRUE;
}

BOOL CXmlFileEx::Decrypt(LPCTSTR szInput, CString& sOutput, LPCTSTR szPassword)
{
	if (!InitEncryptor())
		return FALSE;

	// 1. convert the input string back to binary
	Base64Coder b64;
	b64.Decode(szInput);

	DWORD nReqBufLen = 0;
	unsigned char* pDecodedDataBuffer = b64.DecodedMessage(nReqBufLen);

	nReqBufLen = GetItemValueI("DATALEN");

	// 2. decrypt it
	unsigned char* pDecrypted = NULL;
	int nLenDecrypted = 0;

	if (!m_pEncryptor->Decrypt((unsigned char*)pDecodedDataBuffer, nReqBufLen, szPassword,
		pDecrypted, nLenDecrypted))
		return FALSE;

	// 3. result should be a null-terminated string
	sOutput = pDecrypted;

	// 4. cleanup
	m_pEncryptor->FreeBuffer(pDecrypted);

	return TRUE;
}

BOOL CXmlFileEx::InitEncryptor()
{
	if (m_pEncryptor)
		return TRUE;

	m_pEncryptor = CreateEncryptionInterface("EncryptDecrypt.dll");

	return (m_pEncryptor != NULL);
}

BOOL CXmlFileEx::CanEncrypt()
{
	return CXmlFileEx().InitEncryptor();
}
