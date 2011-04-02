/*
Module : CMapi.cpp
Purpose: Defines the implementation for an MFC wrapper class for sending an email using simple MAPI
Created: PJN / 11-05-1999
History: PJN / 05-12-1999 1. Fixed potential problem where CMapi is used in a console app which was giving an ASSERT
                          2. Fixed an assert which can occur if you dismiss the login dialog when doing an 
                          interactive MAPI logon
               28-03-2000 1. Provision of a sample app which implements a mini mail sender.
                          2. Added a CMapiMessage::AddMultipleRecipients method similar to my SMTP class.
               02-04-2000 1. Password field in the configuration dialog now uses ES_PASSWORD edit control style.
                          2. Password value used for Mapi logon is now stored in the registry encrypted instead of as plain text.
               21-04-2000 1. Added an optional parameter to CMapi::SendMessage to allow you to decide whether or not addresses 
                          should be resolved.
               22-06-2000 1. Fixed a bug in CMapiMessage::AddMultipleRecipients where BCC recipients were not being added 
                          correctly.
               18-07-2000 1. Fixed check for MAPI being installed when client apps create an incorrect "MAPI" entry in win.ini
                          or the registry, Thanks to Chris Raethke for spotting this.
                          2. A "SharedLogon" method has been provided which tries to acquire a shared mapi session
                          3. SendMessage method has been extended to allow the message to be interactively edited.
               25-01-2003 1. Updated copyright messages.
                          2. Made CMapiSession destructor virtual
                          3. Made a new CMapiRecipient class. This allows finer control over setting the display name
                          and the address individually. Thanks to the people on the discussion board on codeproject.com
                          for reporting this problem. The problem arose when Outlook Express displays its security dialog 
                          about an unattended mapi mail being sent. In this dialog the "To" fields were all empty. It seems
                          that OE only displays the addresses in this dialog. Perhaps because so many viruses abound the 
                          address is more relevant that the actual friendy name.
               05-07-2003 1. Fixed a bug in the ordering of the string parameters to the CMapiRecipient constructor. Thanks
                          to "Louchkov" for spotting this problem.
               22-12-2003 1. Added support for reading mail thro MAPI. Thanks to Marcel Scherpenisse for this addition.
               05-02-2004 1. Added a pragma message to inform uses about including afxtempl.h in the precompiled header so as
                          to avoid compiler errors.
               05-05-2004 1. Added support for deleting emails via MAPI. Thanks to Marcel Scherpenisse for this addition.
                          2. Also added the helper function CMapiMessage::AddMultipleAttachments. Again thanks to Marcel 
                          Scherpenisse for this addition.
               05-06-2004 1. Added a parameter to the Logon methods to allow emails to be download (i.e. MAPI_FORCE_DOWNLOAD).
                          Thanks to "caowen" for this update.
               30-04-2005 1. Fix for a bug where the MapiMessage::flFlags value was not being set when a message was being
                          sent. Failure to set the flags value meant that CMapi did not support the "MAPI_RECEIPT_REQUESTED"
                          flag. Thanks to Marcel Scherpenisse for reporting this bug.
               13-08-2005 1. Fixed a number of bugs related to the use of MAPI_FORCE_DOWNLOAD setting. Thanks to Albert van Peppen
                          for this.
                          2. Optimized AddMultipleReceipients and AddMulipleAttachments function somewhat by using new 
                          CString::Trim method when available as well as avoiding addresses which are empty. Again thanks 
                          to Albert van Peppen for this.
                          3. MAPI_FORCE_DOWNLOAD is now set if Logon method has to attempt a shared logon. Again thanks to
                          Albert van Peppen for this.   
                          4. Fixed a bug in SharedLogon where the MAPI_FORCE_DOWNLOAD flag was not being passed. Again thanks to
                          Albert van Peppen for this.   
                          5. Replaced calls to ZeroMemory with memset.
                          6. Class now supports sending HTML emails (well kindof!). To achieve this a new parameter "bSendAsHTML" is 
                          included in the Send method. Please note that the HTML is sent as an additional attachment before all the 
                          other attachments and its disposition type is still marked as an attachment (at least using the latest 
                          version of Outlook on Windows XP). In addition some mail clients (such as Outlook Express) will still interpret 
                          this as an attachment as opposed to inline. If you are not happy with this then you will need to look into 
                          using Extended MAPI which this class does not provide for (and will never) or better yet, you should use SMTP 
                          directly, since HTML email is really part of the SMTP suite of protocols. For example you could use the 
                          author's own CPJNSMTPConnection class. Again thanks to to Albert van Peppen for this.   
                          7. Sample app now allows multiple attachments to be specified as edit box is now not read only.
                          8. Fixed a duplicate mneumonics issue in the main dialog in the sample app
                          9. File dialog which appears in the sample app when you add an attachment not includes a standard file name
                          filter.
                          10. Fixed a number of compiler warnings when the code is compiled using the new Force conformance in for loop
                          setting in Visual Studio 2003.
                          11. Fixed a number of compiler warnings when the code is compiled using the Detect 64-bit portability issues
                          in Visual Studio 2003. Please note that this means the codes requires a recentish copy of the Platform SDK
                          to be installed if you are compiling the code on Visual C++ 6.0
               14-08-2005 1. Fixed problems when the code is compiled for UNICODE. The issue was that the Send method was using the 
                          various T2A macros which allocates a stack based string. This was causing issues because the recipients and 
                          attachments array for the MAPI message was being allocated in loops which was using scoping. This scoping was 
                          causing the stack based strings to be deallocated before the "MAPISendMail" method was called. This issue has 
                          now been fixed by the introduction of new generic heap based macros in PJNConv.h which allocates the converted 
                          string using the C++ new operator. To ensure orderly cleanup of this heap memory, the MapiMessage structure is 
                          now allocated using a hidden C++ class "CMapiMessageCleanup" which cleans up this memory automatically in its 
                          destructor. These macros are quite generic and can be used independent of the CMapi class in places where your 
                          code has the handle this exact same issue. 
               19-08-2005 1. Fixed a bug in the calculation of the flags parameter in the call to acquire a shared logon in the Logon 
                          method. Thanks to Albert van Peppen for reporting this problem.
               23-07-2006 1. Updated copyright details.
                          2. Optimized CMapiMessage constructor code.
                          3. Optimized CMapiSession constructor code.
                          4. Code now uses newer C++ style casts instead of C style casts.
                          5. Updated documentation to use the same style as the web site.
                          6. Fixed a bug freeing up strings in the sample app's CEnumMessagesDlg::OnDestroy method
                          7. Updated the code to clean compile on VC 2005
                          8. Sample app now allows you to specify no To address if the interactive setting is specified
                          9. Removed an unnecessary assert from CMapiMessage::AddMultipleRecipients and 
                          CMapiMessage::AddMultipleAttachements.
                          10. Updated some assert logic in CMapiSession::Send to not assert on the recipient count if we are sending 
                          a message "Interactive". Thanks to Sergiu Scobici for reporting this issue.
                          11. Refactored some code in CMapiSession::Send into CMapiSession::SetupRecipients. Again thanks to Sergiu 
                          Scobici for reporting this 
               26-07-2006 1. Minor update to Logoff to avoid 2 asserts which occur when MAPI is not installed on client machines. 
                          Thanks to Sergiu Scobici for reporting this
               22-12-2006 1. Further updates to fix compile issues for the sample app on VC 2005
                          2. Subject and body and email messages are now allocated using the H_T2A macro. This will avoid problems
                          with large CString values for these fields
                          3. Fixed a bug in CMapiSession::Logoff where the return value would always be TRUE
                          4. A empty profile name is now allowed in the sample app.
                          5. Configuration dialog is now not shown unnecessarily when you send an email in the sample app.
                          6. Logon is now only attempted in the sample app if the Interactive flag is not set
                          7. Addition of a CMAPI_EXT_CLASS preprocessor macro to allow the classes to be more easily added to an 
                          extension dll
               01-01-2007 1. Updated copyright details.
                          2. Fixed incorrect usage of preprocessor macro in CMapiMessage::AddMultipleRecipients method
               15-09-2008 1. Updated copyright details.
                          2. Code now compiles cleanly using Code Analysis (/analyze)
                          3. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          4. Updated sample app to clean compile on VC 2008
                          5. The code has now been updated to support VC 2005 or later only. 
                          6. Removed VC 6 style AppWizard comments from the code.
                          7. Renamed CMapiMessage::AddMultipleAttachements to CMapiMessage::ParseMultipleRecipients and reworked its 
                          parameters to be in line with the author's SMTP class.
                          8. Reworked the way the code handles allocations for the ASCII strings required by simple MAPI. Now these
                          are allocated in the message itself rather than using raw heap allocations via the old H_T2A macro
                          9. AddMultipleRecipients has been renamed to ParseMultipleRecipients in line with the author's SMTP class
                          10. The CMapiRecipient constructor which takes one string parameter now supports breaking the address
                          into email address and friendly name parts
                          11. AddMultipleAttachments now returns the number of attachments added
               06-11-2009 1. Updated the zip file to remove the now defunct PJNConv.h header file
                          2. The sample app now uses DPAPI to encrypt the profile & password configuration settings
               01-04-2011 1. Updated copyright details.
                          2. Replaced use of CT2A with CStringA
                          3. Updated sample app to clean compile on VC 2010

Copyright (c) 1999 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "CMapi.h"


//////////////////////////////// Macros / Defines /////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


////////////////////////////////// Implementation /////////////////////////////

//Class which manages auto cleanup of a "MapiMessage" struct as used by CMapiSession
class CMapiMessageCleanup : public MapiMessage
{
public:
//constructors / Destructors
  CMapiMessageCleanup()
  {
    memset(this, 0, sizeof(MapiMessage));
  }

  ~CMapiMessageCleanup()
  {
    if (lpRecips)
      delete [] lpRecips;
  
    if (lpFiles)
      delete [] lpFiles;
  }
};

CMapiRecipient::CMapiRecipient()
{
}

CMapiRecipient::CMapiRecipient(const CString& sAddress)
{
  //The local variable which we will operate on
  CString sTemp(sAddress);
  sTemp.Trim();

	//divide the substring into friendly names and e-mail addresses
	int nMark = sTemp.Find(_T('<'));
	int nMark2 = sTemp.Find(_T('>'));
	if ((nMark != -1) && (nMark2 != -1) && (nMark2 > (nMark+1)))
	{
		m_sAddress = sTemp.Mid(nMark+1, nMark2 - nMark - 1);
		m_sFriendlyName = sTemp.Left(nMark);
    m_sFriendlyName.Trim();
  }
	else
	{
		nMark = sTemp.Find(_T('('));
		nMark2 = sTemp.Find(_T(')'));
		if ((nMark != -1) && (nMark2 != -1) && (nMark2 > (nMark+1)))
		{
			m_sAddress = sTemp.Left(nMark);
			m_sAddress.Trim();
			m_sFriendlyName = sTemp.Mid(nMark+1, nMark2 - nMark - 1);
			m_sFriendlyName.Trim();
		}
		else
		{
  		m_sAddress = sTemp;
  		m_sFriendlyName = m_sAddress;
    }
	}
}

CMapiRecipient::CMapiRecipient(const CString& sFriendlyName, const CString& sAddress) : m_sAddress(sAddress) , m_sFriendlyName(sFriendlyName)
{
}

void CMapiRecipient::Initialise()
{
  m_sAddress.Empty();
  m_sFriendlyName.Empty();
  m_sAsciiAddress.Empty();
  m_sAsciiFriendlyName.Empty();
}

CMapiRecipient& CMapiRecipient::operator=(const CMapiRecipient& recipient)
{
  m_sAddress = recipient.m_sAddress;
  m_sFriendlyName = recipient.m_sFriendlyName;
  m_sAsciiAddress = recipient.m_sAsciiAddress;
  m_sAsciiFriendlyName = recipient.m_sAsciiFriendlyName;

  return *this;
}


CMapiMessage::CMapiMessage() : m_Flags(0)
{
}

void CMapiMessage::Initialise()
{
  m_To.RemoveAll();
  m_CC.RemoveAll();
  m_BCC.RemoveAll();
  m_sSubject.Empty();
  m_sBody.Empty();
  m_Attachments.RemoveAll();
  m_AttachmentTitles.RemoveAll();
  m_From.Initialise();
  m_sDateReceived.Empty();
  m_sMessageType.Empty();
  m_Flags = 0;
  m_AsciiAttachments.RemoveAll();
  m_AsciiAttachmentTitles.RemoveAll();
}

CMapiMessage& CMapiMessage::operator=(const CMapiMessage& message)
{
  m_To.Copy(message.m_To);
  m_CC.Copy(message.m_CC);
  m_BCC.Copy(message.m_BCC);
  m_sSubject = message.m_sSubject;
  m_sBody = message.m_sBody;
  m_Attachments.Copy(message.m_Attachments);
  m_AttachmentTitles.Copy(message.m_AttachmentTitles);
  m_From = message.m_From;
  m_sDateReceived = message.m_sDateReceived;
  m_sMessageType = message.m_sMessageType;
  m_Flags = message.m_Flags;
  m_AsciiAttachments.Copy(message.m_AsciiAttachments);
  m_AsciiAttachmentTitles.Copy(message.m_AsciiAttachmentTitles);

  return *this;
}

INT_PTR CMapiMessage::ParseMultipleRecipients(const CString& sRecipients, CArray<CMapiRecipient, CMapiRecipient&>& recipients)
{
	ASSERT(sRecipients.GetLength()); //An empty string is now allowed

  //Empty out the array
  recipients.SetSize(0);
	
	//Loop through the whole string, adding recipients as they are encountered
	int length = sRecipients.GetLength();
	TCHAR* buf = new TCHAR[length + 1];	//Allocate a work area (don't touch parameter itself)
  _tcscpy_s(buf, length+1, sRecipients);

  BOOL bLeftQuotationMark=FALSE;
	for (int pos=0, start=0; pos<=length; pos++)
	{
		if (bLeftQuotationMark && buf[pos] != _T('"') )
			continue;
		if (bLeftQuotationMark && buf[pos] == _T('"'))
		{
			bLeftQuotationMark = FALSE;
			continue;
		}
		if (buf[pos] == _T('"'))
		{
			bLeftQuotationMark = TRUE;
			continue;
		}
	
		//Valid separators between addresses are ',' or ';'
		if ((buf[pos] == _T(',')) || (buf[pos] == _T(';')) || (buf[pos] == 0))
		{
			buf[pos] = 0;	//Redundant when at the end of string, but who cares.
      CString sTemp(&buf[start]);
      sTemp.Trim();

      //Let the CMapiRecipient constructor do its work
      CMapiRecipient To(sTemp);
      if (To.m_sAddress.GetLength())
        recipients.Add(To);

      //Move on to the next position
			start = pos + 1;
		}
	}
    
  //Tidy up the heap memory we have used
	delete [] buf;

  //Return the number of recipients parsed
	return recipients.GetSize();
}

int CMapiMessage::AddMultipleAttachments(const CString& sAttachments)
{
  //What will be the return value from this function
  int nAttachments = 0;

	//Loop through the whole string, adding attachments as they are encountered
	int length = sAttachments.GetLength();
  TCHAR* buf = new TCHAR[length + 1];	// Allocate a work area (don't touch parameter itself)
  _tcscpy_s(buf, length+1, sAttachments);
  for (int pos=0, start=0; pos<=length; pos++)
  {
	  //Valid separators between attachments are ',' or ';'
	  if ((buf[pos] == _T(',')) || (buf[pos] == _T(';')) || (buf[pos] == 0))
	  {
		  buf[pos] = 0;	//Redundant when at the end of string, but who cares.
		  CString sTemp(&buf[start]);

      sTemp.Trim();

		  //Now add this attachment if valid
		  if (sTemp.GetLength())
		  {
			  ++nAttachments;
			  m_Attachments.Add(sTemp);
		  }

		  //Move on to the next position
		  start = pos + 1;
	  }
  }
  
  //Tidy up the heap memory we have used
  delete [] buf;
	
	return nAttachments;
}


CMapiSession::CMapiSession() : m_hSession(0),
                               m_nLastError(0),
                               m_hMapi(NULL),
                               m_lpfnMAPILogon(NULL),
                               m_lpfnMAPILogoff(NULL),
                               m_lpfnMAPISendMail(NULL),
                               m_lpfnMAPIResolveName(NULL),
                               m_lpfnMAPIFreeBuffer(NULL),
	                             m_lpfnMAPIReadMail(NULL),
	                             m_lpfnMAPIFindNext(NULL),
                               m_lpfnMAPIDeleteMail(NULL)

{
  Initialise();
}

CMapiSession::~CMapiSession()
{
  //Logoff if logged on
  Logoff();

  //Unload the MAPI dll
  Deinitialise();
}

void CMapiSession::Initialise() 
{
  //First make sure the MAPI32 dll is present on the system. Technically we should
  //be also testing by using the check "GetProfileInt(_T("MAIL"), _T("MAPI"), 0) != 0"
  //also, but some installs of MAPI screw this entry up.
	BOOL bMapiInstalled = (SearchPath(NULL, _T("MAPI32.DLL"), NULL, 0, NULL, NULL) != 0);
  if (bMapiInstalled)
  {
    //Load up the MAPI dll and get the function pointers we are interested in
    m_hMapi = LoadLibrary(_T("MAPI32.DLL"));
    if (m_hMapi)
    {
      m_lpfnMAPILogon = reinterpret_cast<LPMAPILOGON>(GetProcAddress(m_hMapi, "MAPILogon"));
      m_lpfnMAPILogoff = reinterpret_cast<LPMAPILOGOFF>(GetProcAddress(m_hMapi, "MAPILogoff"));
      m_lpfnMAPISendMail = reinterpret_cast<LPMAPISENDMAIL>(GetProcAddress(m_hMapi, "MAPISendMail"));
      m_lpfnMAPIResolveName = reinterpret_cast<LPMAPIRESOLVENAME>(GetProcAddress(m_hMapi, "MAPIResolveName"));
      m_lpfnMAPIFreeBuffer = reinterpret_cast<LPMAPIFREEBUFFER>(GetProcAddress(m_hMapi, "MAPIFreeBuffer"));
		  m_lpfnMAPIReadMail = reinterpret_cast<LPMAPIREADMAIL>(GetProcAddress(m_hMapi, "MAPIReadMail"));
		  m_lpfnMAPIFindNext = reinterpret_cast<LPMAPIFINDNEXT>(GetProcAddress(m_hMapi, "MAPIFindNext"));
      m_lpfnMAPIDeleteMail = reinterpret_cast<LPMAPIDELETEMAIL>(GetProcAddress(m_hMapi, "MAPIDeleteMail"));
  
      //If any of the functions are not installed then fail the load
      if (m_lpfnMAPILogon == NULL || m_lpfnMAPILogoff == NULL ||
          m_lpfnMAPISendMail == NULL || m_lpfnMAPIResolveName == NULL || m_lpfnMAPIFreeBuffer == NULL ||
          m_lpfnMAPIReadMail == NULL || m_lpfnMAPIFindNext == NULL || m_lpfnMAPIDeleteMail == NULL)
      {
        TRACE(_T("CMapiSession::Initialise, Failed to get the required functions pointer in MAPI32.DLL\n"));
        Deinitialise();
      }
    }
  }
  else
    TRACE(_T("CMapiSession::Initialise, MAPI is not installed on this computer\n"));
}

void CMapiSession::Deinitialise()
{
  if (m_hMapi)
  {
    //Unload the MAPI dll and reset the function pointers to NULL
    FreeLibrary(m_hMapi);
    m_hMapi = NULL;
    m_lpfnMAPILogon = NULL;
    m_lpfnMAPILogoff = NULL;
    m_lpfnMAPISendMail = NULL;
    m_lpfnMAPIResolveName = NULL;
    m_lpfnMAPIFreeBuffer = NULL;
		m_lpfnMAPIReadMail = NULL;
		m_lpfnMAPIFindNext = NULL;
    m_lpfnMAPIDeleteMail = NULL;
  }
}

BOOL CMapiSession::Logon(const CString& sProfileName, const CString& sPassword, CWnd* pParentWnd, BOOL bForceDownload)
{
  //Validate our parameters
  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPILogon != NULL); //Function pointer must be valid

  //Initialise the function return value
  BOOL bSuccess = FALSE;

  //Just in case we are already logged in
  Logoff();

  //Get the length of the profile name
  int nProfileLength = sProfileName.GetLength();

  //Setup the flags & UIParam parameters used in the MapiLogon call
  FLAGS flags = 0;
  ULONG_PTR nUIParam = 0;
  if (nProfileLength == 0)
  {
    //No profile name given, then we must interactively request a profile name
    if (pParentWnd)
    {
      nUIParam = reinterpret_cast<ULONG_PTR>(pParentWnd->GetSafeHwnd());
      flags |= MAPI_LOGON_UI;
    }
    else
    {
      //No CWnd given, just use the main window of the app as the parent window
      if (AfxGetMainWnd())
      {
        nUIParam = reinterpret_cast<ULONG_PTR>(AfxGetMainWnd()->GetSafeHwnd());
        flags |= MAPI_LOGON_UI;
      }
    }
  }
  if (bForceDownload)
    flags |= MAPI_FORCE_DOWNLOAD;
  
	//First try to acquire a new MAPI session using the supplied settings using the MAPILogon function
	CStringA sAsciiProfileName(sProfileName);
	LPSTR pszAsciiProfileName = sAsciiProfileName.GetBuffer();
	CStringA sAsciiPassword(sPassword);
	LPSTR pszAsciiPassword = sAsciiPassword.GetBuffer();
  ULONG nError = m_lpfnMAPILogon(nUIParam, nProfileLength ? pszAsciiProfileName : NULL, nProfileLength ? pszAsciiPassword : NULL, flags | MAPI_NEW_SESSION, 0, &m_hSession);
  sAsciiProfileName.ReleaseBuffer();
  sAsciiPassword.ReleaseBuffer();
  if (nError != SUCCESS_SUCCESS && nError != MAPI_E_USER_ABORT)
  {
    FLAGS fSharedFlags = bForceDownload ? MAPI_FORCE_DOWNLOAD : 0;

    //Failed to create a create mapi session, try to acquire a shared mapi session
    TRACE(_T("CMapiSession::Logon, Failed to logon to MAPI using a new session, trying to acquire a shared one\n"));
    nError = m_lpfnMAPILogon(nUIParam, NULL, NULL, fSharedFlags, 0, &m_hSession);
    if (nError == SUCCESS_SUCCESS)
    {
      m_nLastError = SUCCESS_SUCCESS;
      bSuccess = TRUE;
    }
    else
    {
      TRACE(_T("CMapiSession::Logon, Failed to logon to MAPI using a shared session, Error:%d\n"), nError);
      m_nLastError = nError;
    }
  }
  else if (nError == SUCCESS_SUCCESS)
  {
    m_nLastError = SUCCESS_SUCCESS;
    bSuccess = TRUE;
  }

  return bSuccess;
}

BOOL CMapiSession::SharedLogon(BOOL bForceDownload)
{
  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPILogon); //Function pointer must be valid

  //Initialise the function return value
  BOOL bSuccess = FALSE;

  //Just in case we are already logged in
  Logoff();

  //Try to acquire a shared mapi session
  FLAGS flags = bForceDownload ? MAPI_FORCE_DOWNLOAD : 0;
  ULONG nError = m_lpfnMAPILogon(0, NULL, NULL, flags, 0, &m_hSession);
  if (nError == SUCCESS_SUCCESS)
  {
    m_nLastError = SUCCESS_SUCCESS;
    bSuccess = TRUE;
  }
  else
    m_nLastError = nError;

  return bSuccess;
}

BOOL CMapiSession::LoggedOn() const
{
  return (m_hSession != 0);
}

BOOL CMapiSession::MapiInstalled() const
{
  return (m_hMapi != NULL);
}

BOOL CMapiSession::Logoff()
{
  //Initialise the function return value
  BOOL bSuccess = FALSE;

  if (m_hSession)
  {
    ASSERT(m_lpfnMAPILogoff); //Function pointer must be valid

    //Call the low level function
    ULONG nError = m_lpfnMAPILogoff(m_hSession, 0, 0, 0); 
    if (nError != SUCCESS_SUCCESS)
    {
      TRACE(_T("CMapiSession::Logoff, Failed in call to MapiLogoff, Error:%d"), nError);
      m_nLastError = nError;
      bSuccess = FALSE;
    }
    else
    {
      m_nLastError = SUCCESS_SUCCESS;
      bSuccess = TRUE;
    }
    m_hSession = 0;
  }
    
  return bSuccess;
}

BOOL CMapiSession::Resolve(const CString& sName, lpMapiRecipDesc* lppRecip)
{
	//Validate our parameters
  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPIResolveName != NULL); //Function pointer must be valid
  ASSERT(m_hSession); //MAPI session handle must be valid

  //Call the low level function
  CStringA sAsciiName(sName);
  LPSTR pszAsciiName = sAsciiName.GetBuffer();
  ULONG nError = m_lpfnMAPIResolveName(m_hSession, 0, pszAsciiName, 0, 0, lppRecip);
  sAsciiName.ReleaseBuffer();
  if (nError != SUCCESS_SUCCESS)
  {
    TRACE(_T("CMapiSession::Resolve, Failed to resolve the name: %s, Error:%d\n"), sName.operator LPCTSTR(), nError);
    m_nLastError = nError;
  }

  return (nError == SUCCESS_SUCCESS);
}

void CMapiSession::SetupRecipients(MapiMessage& mapiMessage, CMapiMessage& message, BOOL bResolve)
{
  //Validate our parameters
  ASSERT(m_lpfnMAPIFreeBuffer != NULL);

  //Allocate the recipients array
  if (mapiMessage.nRecipCount)
  {
    mapiMessage.lpRecips = new MapiRecipDesc[mapiMessage.nRecipCount];

    //Setup the "To" recipients
    int nRecipIndex = 0;
    INT_PTR nToSize = message.m_To.GetSize();
    for (INT_PTR i=0; i<nToSize; i++)
    {
      MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
      memset(&recip, 0, sizeof(MapiRecipDesc));
      recip.ulRecipClass = MAPI_TO;
      CMapiRecipient& recipient = message.m_To.ElementAt(i);

      if (bResolve)
      {
        //Try to resolve the name
        lpMapiRecipDesc lpTempRecip;  
        if (Resolve(recipient.m_sFriendlyName, &lpTempRecip))
        {
          //Resolve worked, put the resolved name back into the sName
          recipient.m_sFriendlyName = lpTempRecip->lpszName;
          recipient.m_sAddress = lpTempRecip->lpszAddress;

          //Don't forget to free up the memory MAPI allocated for us
          m_lpfnMAPIFreeBuffer(lpTempRecip);
        }
      }

      //Create ASCII versions of the recipient details      
      recipient.m_sAsciiAddress = recipient.m_sAddress;
      recipient.m_sAsciiFriendlyName = recipient.m_sFriendlyName;

      recip.lpszName = recipient.m_sAsciiFriendlyName.GetBuffer(recipient.m_sAsciiFriendlyName.GetLength());
      recip.lpszAddress = recipient.m_sAsciiAddress.GetBuffer(recipient.m_sAsciiAddress.GetLength());

      ++nRecipIndex;
    }

    //Setup the "CC" recipients
    INT_PTR nCCSize = message.m_CC.GetSize();
    for (INT_PTR i=0; i<nCCSize; i++)
    {
      MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
      memset(&recip, 0, sizeof(MapiRecipDesc));
      recip.ulRecipClass = MAPI_CC;
      CMapiRecipient& recipient = message.m_CC.ElementAt(i);

      if (bResolve)
      {    
        //Try to resolve the name
        lpMapiRecipDesc lpTempRecip;  
        if (Resolve(recipient.m_sFriendlyName, &lpTempRecip))
        {
          //Resolve worked, put the resolved name back into the sName
          recipient.m_sFriendlyName = lpTempRecip->lpszName;
          recipient.m_sAddress = lpTempRecip->lpszAddress;

          //Don't forget to free up the memory MAPI allocated for us
          m_lpfnMAPIFreeBuffer(lpTempRecip);
        }
      }

      //Create ASCII versions of the recipient details      
      recipient.m_sAsciiAddress = recipient.m_sAddress;
      recipient.m_sAsciiFriendlyName = recipient.m_sFriendlyName;

      recip.lpszName = recipient.m_sAsciiFriendlyName.GetBuffer(recipient.m_sAsciiFriendlyName.GetLength());
      recip.lpszAddress = recipient.m_sAsciiAddress.GetBuffer(recipient.m_sAsciiAddress.GetLength());

      ++nRecipIndex;
    }

    //Setup the "BCC" recipients
    INT_PTR nBCCSize = message.m_BCC.GetSize();
    for (INT_PTR i=0; i<nBCCSize; i++)
    {
      MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
      memset(&recip, 0, sizeof(MapiRecipDesc));
      recip.ulRecipClass = MAPI_BCC;
      CMapiRecipient& recipient = message.m_BCC.ElementAt(i);

      if (bResolve)
      {
        //Try to resolve the name
        lpMapiRecipDesc lpTempRecip;  
        if (Resolve(recipient.m_sFriendlyName, &lpTempRecip))
        {
          //Resolve worked, put the resolved name back into the sName
          recipient.m_sFriendlyName = lpTempRecip->lpszName;
          recipient.m_sAddress = lpTempRecip->lpszAddress;

          //Don't forget to free up the memory MAPI allocated for us
          m_lpfnMAPIFreeBuffer(lpTempRecip);
        }
      }

      //Create ASCII versions of the recipient details      
      recipient.m_sAsciiAddress = recipient.m_sAddress;
      recipient.m_sAsciiFriendlyName = recipient.m_sFriendlyName;

      recip.lpszName = recipient.m_sAsciiFriendlyName.GetBuffer(recipient.m_sAsciiFriendlyName.GetLength());
      recip.lpszAddress = recipient.m_sAsciiAddress.GetBuffer(recipient.m_sAsciiAddress.GetLength());

      ++nRecipIndex;
    }
  }
}

void CMapiSession::ReleaseRecipients(CMapiMessage& message)
{
  //Iterate thro all the recipients and release their buffers
  INT_PTR nToSize = message.m_To.GetSize();
  for (INT_PTR i=0; i<nToSize; i++)
  {
    CMapiRecipient& recipient = message.m_To.ElementAt(i);
    recipient.m_sAsciiFriendlyName.ReleaseBuffer();
    recipient.m_sAsciiAddress.ReleaseBuffer();
  }

  INT_PTR nCCSize = message.m_CC.GetSize();
  for (INT_PTR i=0; i<nCCSize; i++)
  {
    CMapiRecipient& recipient = message.m_CC.ElementAt(i);
    recipient.m_sAsciiFriendlyName.ReleaseBuffer();
    recipient.m_sAsciiAddress.ReleaseBuffer();
  }

  //Setup the "BCC" recipients
  INT_PTR nBCCSize = message.m_BCC.GetSize();
  for (INT_PTR i=0; i<nBCCSize; i++)
  {
    CMapiRecipient& recipient = message.m_BCC.ElementAt(i);
    recipient.m_sAsciiFriendlyName.ReleaseBuffer();
    recipient.m_sAsciiAddress.ReleaseBuffer();
  }
}

void CMapiSession::SetupAttachment(MapiMessage& mapiMessage, CMapiMessage& message, INT_PTR nIndex)
{
  memset(&(mapiMessage.lpFiles[nIndex]), 0, sizeof(mapiMessage.lpFiles[nIndex]));
  mapiMessage.lpFiles[nIndex].nPosition = static_cast<ULONG>(-1);
  
  //Store ASCII versions of the attachments
  CStringA sAsciiAttachment(message.m_Attachments.ElementAt(nIndex));
  message.m_AsciiAttachments.SetAtGrow(nIndex, sAsciiAttachment);
  CStringA sAsciiAttachmentTitle(message.m_AttachmentTitles.ElementAt(nIndex));
  message.m_AsciiAttachmentTitles.SetAtGrow(nIndex, sAsciiAttachmentTitle);
  
  CStringA& sAttachment = message.m_AsciiAttachments.ElementAt(nIndex);
  mapiMessage.lpFiles[nIndex].lpszPathName = sAttachment.GetBuffer(sAttachment.GetLength());
  CStringA& sAttachmentTitle = message.m_AsciiAttachmentTitles.ElementAt(nIndex);
  if (sAttachmentTitle.GetLength())
    mapiMessage.lpFiles[nIndex].lpszFileName = sAttachmentTitle.GetBuffer(sAttachmentTitle.GetLength());
}

void CMapiSession::SetupAttachments(MapiMessage& mapiMessage, CMapiMessage& message, BOOL bSendAsHTML, const CString& sTempHTMLFile)
{
  //Make the attachment titles array the same size as the attachment array if they are not the same size
  INT_PTR nAttachmentSize = message.m_Attachments.GetSize();
  INT_PTR nTitleSize = message.m_AttachmentTitles.GetSize();
  if (nAttachmentSize)
  { 
    if (nTitleSize != nAttachmentSize)
      message.m_AttachmentTitles.SetSize(nAttachmentSize);
  }

  //If the message is to be sent in HTML, add an extra (temp)file
  if (bSendAsHTML)
  {
    message.m_Attachments.InsertAt(0, sTempHTMLFile);
    message.m_AttachmentTitles.InsertAt(0, _T("default.htm"));
    ++nAttachmentSize;
  }

  if (nAttachmentSize)
  {
    mapiMessage.nFileCount = static_cast<ULONG>(nAttachmentSize);
    mapiMessage.lpFiles = new MapiFileDesc[nAttachmentSize];

    for (INT_PTR i=0; i<nAttachmentSize; i++)
      SetupAttachment(mapiMessage, message, i);
  }
}

void CMapiSession::ReleaseAttachments(CMapiMessage& message)
{
  //Iterate thro all the attachments and release their buffers
  INT_PTR nAttachmentSize = message.m_AsciiAttachments.GetSize();
  for (INT_PTR i=0; i<nAttachmentSize; i++)
  {
    CStringA& sAsciiAttachment = message.m_AsciiAttachments.ElementAt(i);
    sAsciiAttachment.ReleaseBuffer();
  }

  //Iterate thro all the attachment titles and release their buffers
  INT_PTR nAttachmentTitlesSize = message.m_AsciiAttachmentTitles.GetSize();
  for (INT_PTR i=0; i<nAttachmentTitlesSize; i++)
  {
    CStringA& sAsciiAttachmentTitle = message.m_AsciiAttachmentTitles.ElementAt(i);
    sAsciiAttachmentTitle.ReleaseBuffer();
  }
}

BOOL CMapiSession::Send(CMapiMessage& message, BOOL bResolve, BOOL bInteractive, CWnd* pParentWnd, BOOL bSendAsHTML)
{
	//Validate our parameters
  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPISendMail != NULL); //Function pointer must be valid
  ASSERT(m_lpfnMAPIFreeBuffer != NULL); //Function pointer must be valid

  //Initialise the function return value
  BOOL bSuccess = FALSE;  
  
  //Convert the body text up front in the function to ASCII as it is needed in multiple locations in this
  //function and we do not want the overhead of converting it a number of times
  CStringA sAsciiBody(message.m_sBody);
  LPSTR pszAsciiBody = sAsciiBody.GetBuffer();  

	//If the message is to be sent in HTML, setup a file, copy the body text in it and add it to the attachments
	//We create the temp file, copy the body text in it and add it to the MapiMessage as the first file attachment,
	//then we add the 'original' attachments, where we keep in mind there might already been added a file attachment ;)
	CString sTempHTMLFile;
	if (bSendAsHTML) 
  {
		if (!CreateTempPathForHTML(sTempHTMLFile)) 
    {
      //Deliberately set the last error before we return prematurely
      m_nLastError = MAPI_E_FAILURE;

			return FALSE;
		}

		//Now write the body text to this file
    try
    {
      //HTML must be written out as ascii
		  CFile fileDefaulHtm(sTempHTMLFile, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite);
		  fileDefaulHtm.Write(sAsciiBody, sAsciiBody.GetLength());
		  fileDefaulHtm.Close();
    }
    catch(CFileException* pEx)
    {
      //Deliberately set the last error before we return prematurely
      m_nLastError = MAPI_E_ATTACHMENT_WRITE_FAILURE;

      pEx->Delete();
      
      sAsciiBody.ReleaseBuffer();

      return FALSE;
    }
	}

  //Create the MapiMessage structure to match the message parameter send into us
  CMapiMessageCleanup mapiMessage;
  CStringA sAsciiSubject(message.m_sSubject);
  mapiMessage.lpszSubject = sAsciiSubject.GetBuffer();

	//Set the body to NULL for HTML mail; the first attached file should be the HTML <body> file in this case.
  //This is the best we can do to simulate HTML email using just Simple MAPI
	if (bSendAsHTML) 
		mapiMessage.lpszNoteText = NULL;
	else
    mapiMessage.lpszNoteText = pszAsciiBody;

  mapiMessage.nRecipCount = static_cast<ULONG>(message.m_To.GetSize() + message.m_CC.GetSize() + message.m_BCC.GetSize()); 
  ASSERT(mapiMessage.nRecipCount || bInteractive); //Must have at least 1 recipient if we are not "Interactive"!
  mapiMessage.flFlags = message.m_Flags;
  SetupRecipients(mapiMessage, message, bResolve);
  SetupAttachments(mapiMessage, message, bSendAsHTML, sTempHTMLFile);
  
  //Setup the parameters into the function
  FLAGS flags = 0;
  ULONG_PTR uiParam = 0;
  if (!LoggedOn())
    flags |= MAPI_LOGON_UI;
  if (bInteractive)
  {
    flags |= MAPI_DIALOG;
    if (pParentWnd)
      uiParam = reinterpret_cast<ULONG_PTR>(pParentWnd->GetSafeHwnd());
    else
    {
      ASSERT(AfxGetMainWnd());
      uiParam = reinterpret_cast<ULONG_PTR>(AfxGetMainWnd()->GetSafeHwnd());
    }
  }

  //Call the low level function
  ULONG nError = m_lpfnMAPISendMail(m_hSession, uiParam, &mapiMessage, flags, 0);
  if (nError == SUCCESS_SUCCESS)
  {
    bSuccess = TRUE;
    m_nLastError = SUCCESS_SUCCESS;
  }
  else
  {
    TRACE(_T("CMapiSession::Send, Failed to send mail message, Error:%d\n"), nError);
    m_nLastError = nError;
  }
  
  //Tidy up the buffers
  ReleaseRecipients(message);
  ReleaseAttachments(message);
  sAsciiSubject.ReleaseBuffer();
  sAsciiBody.ReleaseBuffer();

  //Delete tempfile and dir if sending HTML
  if (bSendAsHTML)
    DeleteFile(sTempHTMLFile);

  return bSuccess;
}

BOOL CMapiSession::Find(CString& sMessageID, const CString& sSeedMessageID, FLAGS flFlags, const CString& sMessageType, BOOL bInteractive, CWnd* pParentWnd)
{
	//Validate our parameters
  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPIFindNext != NULL); //Function pointer must be valid
  ASSERT(m_hSession); //MAPI session handle must be valid

  //Initialise the function return value
  BOOL bSuccess = FALSE;  

  //Form the UI param
  ULONG_PTR uiParam = 0;
  if (bInteractive)
  {
    if (pParentWnd)
      uiParam = reinterpret_cast<ULONG_PTR>(pParentWnd->GetSafeHwnd());
    else
    {
      ASSERT(AfxGetMainWnd());
      uiParam = reinterpret_cast<ULONG_PTR>(AfxGetMainWnd()->GetSafeHwnd());
    }
  }

  //Form the ascii version of the various strings
  CStringA sAsciiSeedMessageID(sSeedMessageID);
  LPSTR pszAsciiSeedMessageID = sAsciiSeedMessageID.GetBuffer();
  CStringA sAsciiMessageType(sMessageType);
  LPSTR pszAsciiMessageType = sAsciiMessageType.GetBuffer();

  //Call the low level function
  char szAsciiMessageID[512];
  szAsciiMessageID[0] = '\0';
  ULONG nError = m_lpfnMAPIFindNext(m_hSession, uiParam, sAsciiMessageType.GetLength() ? pszAsciiMessageType : NULL, 
                                    sAsciiSeedMessageID.GetLength() ? pszAsciiSeedMessageID : NULL, flFlags, 0, szAsciiMessageID);
  sAsciiMessageType.ReleaseBuffer();                                    
  sAsciiSeedMessageID.ReleaseBuffer();
	if (nError == SUCCESS_SUCCESS) 
  {
    bSuccess = TRUE;
    m_nLastError = nError;
    sMessageID = szAsciiMessageID;    
  }
  else
    m_nLastError = nError;

  return bSuccess;
}

BOOL CMapiSession::Read(const CString& sMessageID, CMapiMessage& message, FLAGS flFlags, BOOL bInteractive, CWnd* pParentWnd)
{
	//Validate our parameters
  ASSERT(MapiInstalled()); //MAPI must be installed
  ASSERT(m_lpfnMAPIReadMail != NULL); //Function pointer must be valid
  ASSERT(m_lpfnMAPIFreeBuffer != NULL); //Function pointer must be valid
  ASSERT(m_hSession); //MAPI session handle must be valid
  ASSERT(sMessageID.GetLength() > 0); //Message ID should have been specified

  //Initialise the function return value
  BOOL bSuccess = FALSE;  

  //Form the UI param
  ULONG_PTR uiParam = 0;
  if (bInteractive)
  {
    if (pParentWnd)
      uiParam = reinterpret_cast<ULONG_PTR>(pParentWnd->GetSafeHwnd());
    else
    {
      ASSERT(AfxGetMainWnd());
      uiParam = reinterpret_cast<ULONG_PTR>(AfxGetMainWnd()->GetSafeHwnd());
    }
  }

  //Form the ascii version of the various strings
  CStringA sAsciiMessageID(sMessageID);
  LPSTR pszAsciiMessageID = sAsciiMessageID.GetBuffer();

  //Call the low level function
  MapiMessage* mapiMessage = NULL;
  ULONG nError = m_lpfnMAPIReadMail(m_hSession, uiParam, pszAsciiMessageID, flFlags, 0, &mapiMessage);
  sAsciiMessageID.ReleaseBuffer();
	if (nError == SUCCESS_SUCCESS) 
  {
    bSuccess = TRUE;
	  m_nLastError = SUCCESS_SUCCESS;

    message.Initialise();
		message.m_From.m_sFriendlyName = mapiMessage->lpOriginator->lpszName;
		message.m_From.m_sAddress = mapiMessage->lpOriginator->lpszAddress;
		message.m_sSubject = mapiMessage->lpszSubject;
		message.m_sBody = mapiMessage->lpszNoteText;
		message.m_sDateReceived = mapiMessage->lpszDateReceived;
    message.m_Flags = mapiMessage->flFlags;
    message.m_sMessageType = mapiMessage->lpszMessageType;

		//Get the recipients
		CMapiRecipient MapiRecipient;
		for (ULONG i=0; i<mapiMessage->nRecipCount; i++) 
    {
      MapiRecipDesc& recip = mapiMessage->lpRecips[i];
			MapiRecipient.m_sFriendlyName= recip.lpszName;
			MapiRecipient.m_sAddress = recip.lpszAddress;
		  switch (recip.ulRecipClass) 
      {
        case MAPI_TO:		
        {
          message.m_To.Add(MapiRecipient);		
          break;
        }
        case MAPI_CC:		
        {
          message.m_CC.Add(MapiRecipient);		
          break;
        }
        case MAPI_BCC:	
        {
          message.m_BCC.Add(MapiRecipient);	
          break;
        }
        default:
        {
          ASSERT(FALSE);
          break;
        }
			}
		}

		//Get the attachments
    message.m_Attachments.SetSize(mapiMessage->nFileCount);
    message.m_AttachmentTitles.SetSize(mapiMessage->nFileCount);
		for (ULONG j=0; j<mapiMessage->nFileCount; j++) 
    {
	    MapiFileDesc& file = mapiMessage->lpFiles[j];
			message.m_Attachments.SetAt(j, CString(file.lpszPathName));
      message.m_AttachmentTitles.SetAt(j, CString(file.lpszFileName));
		}

    //Free up the mapimessage structure returned by MAPIReadMail
		m_lpfnMAPIFreeBuffer(mapiMessage);
	}
	else 
		m_nLastError = nError;

	return bSuccess;
}

BOOL CMapiSession::Delete(const CString& sMessageID, BOOL bInteractive, CWnd* pParentWnd)
{
	//Validate our parameters
	ASSERT(MapiInstalled()); //MAPI must be installed
	ASSERT(m_lpfnMAPIDeleteMail != NULL); //Function pointer must be valid
	ASSERT(m_hSession); //MAPI session handle must be valid
	ASSERT(sMessageID.GetLength() > 0); //Message ID should have been specified

	//Initialise the function return value
	BOOL bSuccess = FALSE;

	//Form the UI param
	ULONG_PTR uiParam = 0;
	if (bInteractive)
	{
		if (pParentWnd)
			uiParam = reinterpret_cast<ULONG_PTR>(pParentWnd->GetSafeHwnd());
		else
		{
			ASSERT(AfxGetMainWnd());
			uiParam = reinterpret_cast<ULONG_PTR>(AfxGetMainWnd()->GetSafeHwnd());
		}
	}
	
  //Form the ascii version of the various strings
  CStringA sAsciiMessageID(sMessageID);
  LPSTR pszAsciiMessageID = sAsciiMessageID.GetBuffer();

  //Call the function
	ULONG nError = m_lpfnMAPIDeleteMail(m_hSession, uiParam, pszAsciiMessageID, 0, 0);
	sAsciiMessageID.ReleaseBuffer();
	if (nError == SUCCESS_SUCCESS) 
	{
		bSuccess = TRUE;
		m_nLastError = SUCCESS_SUCCESS;
	}
	else
		m_nLastError = nError;

	return bSuccess;
}

ULONG CMapiSession::GetLastError() const
{
  return m_nLastError;
}

BOOL CMapiSession::CreateTempPathForHTML(CString& sUniquePath)
{
  //What will the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Get the temp path
  TCHAR szTempPath[_MAX_PATH];
  szTempPath[0] = '\0';
	if (GetTempPath(_MAX_PATH, szTempPath))
  {
    //Now form a unique filename using GetTempFile
    TCHAR szTempFileName[_MAX_PATH];
    szTempFileName[0] = '\0';
    if (GetTempFileName(szTempPath, _T("cma"), 0, szTempFileName))
    {
      bSuccess = TRUE;
      sUniquePath = szTempFileName;
    }
    else
      TRACE(_T("CMapiSession::CreateTempPathForHTML, Unable to create temp path, Error:%d\n"), GetLastError());
  }
  else
    TRACE(_T("CMapiSession::CreateTempPathForHTML, Failed to get temp path, Error:%d\n"), GetLastError());

  return bSuccess;
}
