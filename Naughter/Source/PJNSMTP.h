/*
Module : PJNSMTP.H
Purpose: Defines the interface for a MFC class encapsulation of the SMTP protocol
Created: PJN / 22-05-1998

Copyright (c) 1998 - 2006 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

Please note that I have been informed recently that C(PJN)SMTPConnection is being used to develop and send unsolicted bulk mail. 
This was not the intention of the code and the author explicitly forbids use of the code for any software of this kind without 
my explicit written consent.

*/


/////////////////////////////// Defines ///////////////////////////////////////

#pragma once

#ifndef __PJNSMTP_H__
#define __PJNSMTP_H__

#ifndef __AFXTEMPL_H__
#pragma message("To avoid this message, please put afxtempl.h in your PCH (usually stdafx.h)")
#include <afxtempl.h>
#endif

#ifndef _WINSOCKAPI_
#pragma message("To avoid this message, please put afxsock.h or winsock.h in your PCH (usually stdafx.h)")
#include <WinSock.h>
#endif

#ifndef __AFXPRIV_H__
#pragma message("To avoid this message, please put afxpriv.h in your PCH (usually stdafx.h)")
#include <afxpriv.h>
#endif

#ifndef CPJNSMTP_NOMXLOOKUP
#ifndef _WINDNS_INCLUDED_
#pragma message("To avoid this message, please put WinDNS.h in your PCH (usually stdafx.h)")
#include <WinDNS.h> //If you get a compilation error on this line, then you need to download, install and configure the MS Platform SDK if you are compiling the code under Visual C++ 6
#endif
#endif

#pragma warning(push, 3) //Avoid all the level 4 warnings in STL
#ifndef _STRING_
#pragma message("To avoid this message, please put string in your PCH (usually stdafx.h)")
#include <string>
#endif
#pragma warning(pop)

#include "SocMFC.h" //If you get a compilation error about this missing header file, then you need to download my CWSocket and Base64 classes from http://www.naughter.com/w3mfc.html
#include "Base64.h" //If you get a compilation error about this missing header file, then you need to download my CWSocket and Base64 classes from http://www.naughter.com/w3mfc.html
#ifndef CPJNSMTP_NOSSL
#include "OpenSSLMfc.h" //If you get a compilation error about this missing header file, then you need to download my CSSLSocket classes from http://www.naughter.com/w3mfc.html
#endif

#ifndef CPJNSMTP_NONTLM
#include "PJNNTLMAuth.h"
#endif

#ifndef PJNSMTP_EXT_CLASS
#define PJNSMTP_EXT_CLASS
#endif

#if defined(__INTEL_COMPILER)
// remark #383: value copied to temporary, reference to temporary used
#pragma warning(disable: 383)
// remark #271: trailing comma is nonstandard
#pragma warning(disable: 271)
// remark #444: destructor for base class is not virtual
#pragma warning(disable: 444)
#endif	// __INTEL_COMPILER

//Constants
const DWORD PJNSMTP_DSN_NOT_SPECIFIED = 0xFFFFFFFF; //We are not specifying if we should be using a DSN or not  
const DWORD PJNSMTP_DSN_SUCCESS       = 0x01;       //A DSN should be sent back for messages which were successfully delivered
const DWORD PJNSMTP_DSN_FAILURE       = 0x02;       //A DSN should be sent back for messages which was not successfully delivered
const DWORD PJNSMTP_DSN_DELAY         = 0x04;       //A DSN should be sent back for messages which were delayed



/////////////////////////////// Classes ///////////////////////////////////////

///////////// Class which makes using CBase64 class easier ////////////////////

class PJNSMTP_EXT_CLASS CPJNSMPTBase64 : public CBase64
{
public:
//Constructors / Destructors
  CPJNSMPTBase64();
  ~CPJNSMPTBase64();

//methods
	void	Encode(const BYTE* pbyData, int nSize, DWORD dwFlags);
	void	Decode(LPCSTR pData, int nSize);
	void	Encode(LPCSTR pszMessage, DWORD dwFlags);
	void	Decode(LPCSTR sMessage);

	LPSTR Result() const { return m_pBuf; };
	int	  ResultSize() const { return m_nSize; };

protected:
  char* m_pBuf;
  int   m_nSize;
};

///////////// Exception class /////////////////////////////////////////////////

class PJNSMTP_EXT_CLASS CPJNSMTPException : public CException
{
public:
//Constructors / Destructors
  CPJNSMTPException(HRESULT hr, const CString& sLastResponse = _T("")); 
	CPJNSMTPException(DWORD dwError = 0, DWORD dwFacility = FACILITY_WIN32, const CString& sLastResponse = _T(""));

//Methods
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(LPTSTR lpstrError, UINT nMaxError,	PUINT pnHelpContext = NULL);
	CString GetErrorMessage();

//Data members
	HRESULT m_hr;
  CString m_sLastResponse;

protected:
	DECLARE_DYNAMIC(CPJNSMTPException)
};
                     
////// Encapsulation of an SMTP email address /////////////////////////////////

class PJNSMTP_EXT_CLASS CPJNSMTPAddress
{
public: 
//Constructors / Destructors
  CPJNSMTPAddress();
  CPJNSMTPAddress(const CPJNSMTPAddress& address);
	CPJNSMTPAddress(const CString& sAddress);
	CPJNSMTPAddress(const CString& sFriendly, const CString& sAddress);
	CPJNSMTPAddress& operator=(const CPJNSMTPAddress& r);

//Methods
  CString GetRegularFormat(BOOL bEncode, const CString& sCharset) const;

//Data members
	CString m_sFriendlyName; //Would set it to contain something like "PJ Naughter"
  CString m_sEmailAddress; //Would set it to contains something like "pjna@naughter.com"
};

////// Encapsulatation of an SMTP MIME body part //////////////////////////////

class PJNSMTP_EXT_CLASS CPJNSMTPBodyPart
{
public:
//Constructors / Destructors
  CPJNSMTPBodyPart();
  CPJNSMTPBodyPart(const CPJNSMTPBodyPart& bodyPart);
  CPJNSMTPBodyPart& operator=(const CPJNSMTPBodyPart& bodyPart);
  virtual ~CPJNSMTPBodyPart();

//Accessors / Mutators
  BOOL    SetFilename(const CString& sFilename);
  CString GetFilename() const { return m_sFilename; }; 

  void    SetText(const CString& sText);
  CString GetText() const { return m_sText; };

	void    SetTitle(const CString& sTitle) { m_sTitle = sTitle; };
	CString GetTitle() const { return m_sTitle; };

	void    SetContentType(const CString& sContentType) { m_sContentType = sContentType; };
	CString GetContentType() const { return m_sContentType; };

	void    SetCharset(const CString& sCharset) { m_sCharset = sCharset; };
	CString GetCharset() const { return m_sCharset; };

  void    SetContentBase(const CString& sContentBase) { m_sContentBase = sContentBase; };
  CString GetContentBase() const { return m_sContentBase; };

  void    SetContentID(const CString& sContentID);
  CString GetContentID() const;

  void    SetContentLocation(const CString& sContentLocation);
  CString GetContentLocation() const;

  CString GetBoundary() const { return m_sBoundary; };

//Misc methods
  BOOL GetHeader(LPSTR& pszHeader, int& nHeaderSize);
  BOOL GetBody(BOOL bDoSingleDotFix, LPSTR& pszBody, int& nBodySize);
  BOOL GetFooter(LPSTR& pszFooter, int& nFooterSize);
  void FreeHeader(LPSTR& pszHeader);
  void FreeBody(LPSTR& pszBody);
  void FreeFooter(LPSTR& pszFooter);
  CPJNSMTPBodyPart* FindFirstBodyPart(const CString sContentType);
  void SetQuotedPrintable(BOOL bValue) { m_bQuotedPrintable = bValue; };
  BOOL GetQuotedPrintable() const { return m_bQuotedPrintable; };
  void SetBase64(BOOL bValue) { m_bBase64 = bValue; };
  BOOL GetBase64() const { return m_bBase64; };
  void SetMaxAttachmentSize(DWORD dwSize) { m_dwMaxAttachmentSize = dwSize; };
  DWORD GetMaxAttachementSize() const { return m_dwMaxAttachmentSize; };

//Child Body part methods
	int            GetNumberOfChildBodyParts() const;
	int            AddChildBodyPart(CPJNSMTPBodyPart& bodyPart);
	void           RemoveChildBodyPart(int nIndex);
	CPJNSMTPBodyPart* GetChildBodyPart(int nIndex);
  CPJNSMTPBodyPart* GetParentBodyPart();

//Static methods
  static std::string QuotedPrintableEncode(const std::string& sText);
  static int         ConvertToUTF8(const CString& in, std::string &);
  static int         UnicodeToUTF8(LPCWSTR wszSrc, int nSrc, LPSTR szDest,int nDest);
  static char        HexDigit(int nDigit);
  static std::string HeaderEncode(const CString& sText, const CString& sCharset);
  static std::string QEncode(LPCSTR sText, LPCSTR sCharset);

protected:
//Member variables
  CString                                       m_sFilename;           //The file you want to attach
  CString                                       m_sTitle;              //What is it to be know as when emailed
  CString                                       m_sContentType;        //The mime content type for this body part
  CString                                       m_sCharset;            //The charset for this body part
  CString                                       m_sContentBase;        //The absolute URL to use for when you need to resolve any relative URL's in this body part
  CString                                       m_sContentID;          //The uniqiue ID for this body part (allows other body parts to refer to us via a CID URL)
  CString                                       m_sContentLocation;    //The relative URL for this body part (allows other body parts to refer to us via a relative URL)
  CString                                       m_sText;               //If using strings rather than file, then this is it!
  CPJNSMPTBase64                                m_Coder;	             //Base64 encoder / decoder instance for this body part
  CArray<CPJNSMTPBodyPart*, CPJNSMTPBodyPart*&> m_ChildBodyParts;      //Child body parts for this body part
  CPJNSMTPBodyPart*                             m_pParentBodyPart;     //The parent body part for this body part
  CString                                       m_sBoundary;           //String which is used as the body separator for all child mime parts
  BOOL                                          m_bQuotedPrintable;    //Should the body text by quoted printable encoded
  BOOL                                          m_bBase64;             //Should the body be base64 encoded. Overrides "m_bQuotedPrintable"
  DWORD                                         m_dwMaxAttachmentSize; //The maximum size this body part can be if it is a file attachment (Defaults to 50 MB)

//Methods
  static void FixSingleDotA(std::string& sBody);
  static void FixSingleDotT(CString& sBody);

  friend class CPJNSMTPMessage;
  friend class CPJNSMTPConnection;
};

////////////////// typedefs ///////////////////////////////////////////////////

typedef CArray<CPJNSMTPAddress, CPJNSMTPAddress&> CPJNSMTPAddressArray;

////////////////// Forward declaration ////////////////////////////////////////

class PJNSMTP_EXT_CLASS CPJNSMTPConnection;

/////// Encapsulation of an SMTP message //////////////////////////////////////

class PJNSMTP_EXT_CLASS CPJNSMTPMessage
{
public:
//Enums
	enum RECIPIENT_TYPE 
	{ 
	  TO  = 0, 
	  CC  = 1, 
	  BCC = 2 
	};
  enum PRIORITY 
  { 
    NO_PRIORITY     = 0, 
    LOW_PRIORITY    = 1, 
    NORMAL_PRIORITY = 2, 
    HIGH_PRIORITY   = 3 
  };
  enum DSN_RETURN_TYPE
  {
    HEADERS_ONLY = 0,
    FULL_EMAIL = 1
  };

//Constructors / Destructors
  CPJNSMTPMessage();
  CPJNSMTPMessage(const CPJNSMTPMessage& message);
  CPJNSMTPMessage& operator=(const CPJNSMTPMessage& message);
  virtual ~CPJNSMTPMessage();

//Recipient support
	int                  GetNumberOfRecipients(RECIPIENT_TYPE RecipientType = TO) const;
	int                  AddRecipient(CPJNSMTPAddress& recipient, RECIPIENT_TYPE RecipientType = TO);
	void                 RemoveRecipient(int nIndex, RECIPIENT_TYPE RecipientType = TO);
	CPJNSMTPAddress*     GetRecipient(int nIndex, RECIPIENT_TYPE RecipientType = TO);
  BOOL                 AddMultipleRecipients(const CString& sRecipients, RECIPIENT_TYPE RecipientType);
  static int           ParseMultipleRecipients(const CString& sRecipients, CPJNSMTPAddressArray& recipients);

//Body Part support
  int                  GetNumberOfBodyParts() const;
	int                  AddBodyPart(CPJNSMTPBodyPart& bodyPart);
	void                 RemoveBodyPart(int nIndex);
	CPJNSMTPBodyPart*    GetBodyPart(int nIndex);
  int                  AddMultipleAttachments(const CString& sAttachments);

//Misc methods
  virtual std::string  getHeader();
  void                 AddTextBody(const CString& sBody);
  CString              GetTextBody();
  void                 AddHTMLBody(const CString& sBody, const CString& sContentBase);
  CString              GetHTMLBody();
  void                 AddCustomHeader(const CString& sHeader);
  CString              GetCustomHeader(int nIndex);
  int                  GetNumberOfCustomHeaders() const;
  void                 RemoveCustomHeader(int nIndex);
  void                 SetCharset(const CString& sCharset);
  CString              GetCharset() const;
  void                 SetMime(BOOL bMime);
  BOOL                 GetMime() const { return m_bMime; };
  void                 SaveToDisk(const CString& sFilename);
                                
//Data Members
	CPJNSMTPAddress      m_From;
	CString              m_sSubject;
  CString              m_sXMailer;
	CPJNSMTPAddress      m_ReplyTo;
  CPJNSMTPBodyPart     m_RootPart;
  PRIORITY             m_Priority;
  DSN_RETURN_TYPE      m_DSNReturnType;
  DWORD                m_DSN;     //To be filled in with the PJNSMTP_DSN_... flags
  CString              m_sENVID;  //The "Envelope ID" to use for requesting DSN's. If you leave this empty when you are sending the message
                                  //then one which be generated for you based on a GUID and you can examine/store this value after the 
                                  //message was sent

protected:
//Methods
  void        WriteToDisk(HANDLE hFile, CPJNSMTPBodyPart* pBodyPart, BOOL bRoot);
  CString     ConvertHTMLToPlainText(const CString& sHtml);

//Member variables
	CArray<CPJNSMTPAddress*, CPJNSMTPAddress*&> m_ToRecipients;
	CArray<CPJNSMTPAddress*, CPJNSMTPAddress*&> m_CCRecipients;
	CArray<CPJNSMTPAddress*, CPJNSMTPAddress*&> m_BCCRecipients;
  CStringArray                                m_CustomHeaders;
  BOOL                                        m_bMime;

  friend class CPJNSMTPConnection;
};

//////// The main class which encapsulates the SMTP connection ////////////////

#ifndef CPJNSMTP_NONTLM
class PJNSMTP_EXT_CLASS CPJNSMTPConnection : public CNTLMClientAuth
#else
class PJNSMTP_EXT_CLASS CPJNSMTPConnection
#endif
{
public:

//typedefs
  enum AuthenticationMethod
  {
    AUTH_NONE     = 0,
    AUTH_CRAM_MD5 = 1,
    AUTH_LOGIN    = 2,
    AUTH_PLAIN    = 3,
    AUTH_NTLM     = 4
  };

  enum ConnectToInternetResult
  {
    CTIR_Failure=0,
    CTIR_ExistingConnection=1,
    CTIR_NewConnection=2,
  };

  enum ProxyType
  {
    ptNone = 0,
    ptSocks4 = 1,
    ptSocks5 = 2,
    ptHTTP = 3
  };

//Constructors / Destructors
  CPJNSMTPConnection();
  virtual ~CPJNSMTPConnection();

//Methods
#ifndef CPJNSMTP_NOSSL
  void    Connect(LPCTSTR pszHostName, AuthenticationMethod am = AUTH_NONE, LPCTSTR pszUsername=NULL, LPCTSTR pszPassword=NULL, int nPort=25, BOOL bSSL = FALSE);
#else
  void    Connect(LPCTSTR pszHostName, AuthenticationMethod am = AUTH_NONE, LPCTSTR pszUsername=NULL, LPCTSTR pszPassword=NULL, int nPort=25);
#endif
  void    Disconnect(BOOL bGracefully = TRUE);
  BOOL    IsConnected() const	{ return m_bConnected; };
  CString GetLastCommandResponse() const { return m_sLastCommandResponse; };
  int     GetLastCommandResponseCode() const { return m_nLastCommandResponseCode; };
  DWORD   GetTimeout() const { return m_dwTimeout; };
  void    SetTimeout(DWORD dwTimeout) { m_dwTimeout = dwTimeout; };
	void    SendMessage(CPJNSMTPMessage& Message);
  void    SendMessage(const CString& sMessageOnFile, CPJNSMTPAddressArray& Recipients, const CPJNSMTPAddress& From, CString& sENVID, DWORD dwSendBufferSize = 4096, DWORD DSN = PJNSMTP_DSN_NOT_SPECIFIED, CPJNSMTPMessage::DSN_RETURN_TYPE DSNReturnType = CPJNSMTPMessage::HEADERS_ONLY);
  void    SendMessage(BYTE* pMessage, DWORD dwMessageSize, CPJNSMTPAddressArray& Recipients, const CPJNSMTPAddress& From, CString& sENVID, DWORD dwSendBufferSize = 4096, DWORD DSN = PJNSMTP_DSN_NOT_SPECIFIED, CPJNSMTPMessage::DSN_RETURN_TYPE DSNReturnType = CPJNSMTPMessage::HEADERS_ONLY);
  void    SetHeloHostname(const CString& sHostname);
  CString GetHeloHostName() const { return m_sHeloHostname; };

//Proxy Methods
  void      SetProxyType(ProxyType proxyType) { m_ProxyType = proxyType; };
  ProxyType GetProxyType() const { return m_ProxyType; };
  void      SetProxyServer(const CString& sServer) { m_sProxyServer = sServer; };
  CString   GetProxyServer() const { return m_sProxyServer; };
  void      SetProxyPort(int nPort) { m_nProxyPort = nPort; };
  int       GetProxyPort() { return m_nProxyPort; };
  void      SetBoundAddress(const CString& sLocalBoundAddress) { m_sLocalBoundAddress = sLocalBoundAddress; };
  CString   GetBoundAddress() const { return m_sLocalBoundAddress; };
  void      SetProxyUserName(const CString& sUserName) { m_sProxyUserName = sUserName; };
  CString   GetProxyUserName() const { return m_sProxyUserName; };
  void      SetProxyPassword(const CString& sPassword) { m_sProxyPassword = sPassword; };
  CString   GetProxyPassword() const { return m_sProxyPassword; };
  void      SetHTTPProxyUserAgent(const CString& sUserAgent) { m_sUserAgent = sUserAgent; };
  CString   GetHTTPProxyUserAgent() const { return m_sUserAgent; };

//"Wininet" Connectivity methods
  ConnectToInternetResult ConnectToInternet();
  BOOL CloseInternetConnection();

#ifndef CPJNSMTP_NOMXLOOKUP
  BOOL MXLookup(LPCTSTR lpszHostDomain, CStringArray& arrHosts, CWordArray& arrPreferences, WORD fOptions = DNS_QUERY_STANDARD, PIP4_ARRAY aipServers = NULL);
  BOOL MXLookupAvailable();
#endif

//Static methods
  static void ThrowPJNSMTPException(DWORD dwError = 0, DWORD Facility = FACILITY_WIN32, const CString& sLastResponse = _T(""));
  static void ThrowPJNSMTPException(HRESULT hr, const CString& sLastResponse = _T(""));
  static CString CreateNEWENVID();

//Virtual Methods
  virtual BOOL OnSendProgress(DWORD dwCurrentBytes, DWORD dwTotalBytes);

protected:
//typedefs of the function pointers
  typedef BOOL (WINAPI INTERNETGETCONNECTEDSTATE)(LPDWORD, DWORD);
  typedef INTERNETGETCONNECTEDSTATE* LPINTERNETGETCONNECTEDSTATE;
  typedef BOOL (WINAPI INTERNETAUTODIALHANGUP)(DWORD);
  typedef INTERNETAUTODIALHANGUP* LPINTERNETAUTODIALHANGUP;
  typedef BOOL (WINAPI INTERNETATTEMPCONNECT)(DWORD);
  typedef INTERNETATTEMPCONNECT* LPINTERNETATTEMPCONNECT;

#ifndef CPJNSMTP_NOMXLOOKUP
  typedef VOID (WINAPI DNSRECORDLISTFREE)(PDNS_RECORD, DNS_FREE_TYPE);
  typedef DNSRECORDLISTFREE* LPDNSRECORDLISTFREE;
  typedef DNS_STATUS (WINAPI DNSQUERY)(LPCTSTR, WORD, DWORD, PIP4_ARRAY, PDNS_RECORD*, PVOID*);
  typedef DNSQUERY* LPDNSQUERY;
#endif

//methods
	void         AuthCramMD5(LPCTSTR pszUsername, LPCTSTR pszPassword);
  void         ConnectESMTP(LPCTSTR pszLocalName, LPCTSTR pszUsername, LPCTSTR pszPassword, AuthenticationMethod am);
  void         ConnectSMTP(LPCTSTR pszLocalName);
	void         AuthLogin(LPCTSTR pszUsername, LPCTSTR pszPassword);
	void         AuthPlain(LPCTSTR pszUsername, LPCTSTR pszPassword);
  void         AuthNTLM();
  virtual CString FormMailFromCommand(const CString& sEmailAddress, DWORD DSN, CPJNSMTPMessage::DSN_RETURN_TYPE DSNReturnType, CString& sENVID);
	virtual void SendRCPTForRecipient(DWORD DSN, CPJNSMTPAddress& recipient);
  virtual void SendBodyPart(CPJNSMTPBodyPart* pBodyPart, BOOL bRoot);
	virtual BOOL ReadCommandResponse(int nExpectedCode);
  virtual BOOL ReadCommandResponse(int nExpectedCode1, int nExpectedCode2);
	virtual BOOL ReadResponse(LPSTR pszBuffer, int nInitialBufSize, LPSTR* ppszOverFlowBuffer, int nGrowBy=4096);
#ifndef CPJNSMTP_NONTLM
  virtual SECURITY_STATUS NTLMAuthPhase1(PBYTE pBuf, DWORD cbBuf);
  virtual SECURITY_STATUS NTLMAuthPhase2(PBYTE pBuf, DWORD cbBuf, DWORD* pcbRead);
  virtual SECURITY_STATUS NTLMAuthPhase3(PBYTE pBuf, DWORD cbBuf);
#endif
#ifndef CPJNSMTP_NOSSL
  virtual CString GetOpenSSLError();
#endif
  void _CreateSocket();
  void _ConnectViaSocks4(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, DWORD dwConnectionTimeout);
  void _ConnectViaSocks5(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszSocksServer, UINT nSocksPort, LPCTSTR lpszUserName, LPCTSTR lpszPassword, DWORD dwConnectionTimeout, BOOL bUDP);
  void _ConnectViaHTTPProxy(LPCTSTR lpszHostAddress, UINT nHostPort, LPCTSTR lpszHTTPServer, UINT nHTTPProxyPort, CString & sProxyResponse, LPCTSTR lpszUserName, LPCTSTR pszPassword, DWORD dwConnectionTimeout, LPCTSTR lpszUserAgent);
  void _Connect(LPCTSTR lpszHostAddress, UINT nHostPort);
  int  _Send(const void *pBuffer, int nBuf);
  int  _Receive(void *pBuffer, int nBuf);
  void _Close();
  BOOL _IsReadible(DWORD dwTimeout);

//Member variables
#ifndef CPJNSMTP_NOSSL
  CSSLContext                 m_SSLCtx;                   //SSL Context
  CSSLSocket                  m_SSL;                      //SSL socket wrapper
#endif
  BOOL                        m_bSSL;                     //Are we connecting using SSL?
  CWSocket                    m_Socket;                   //The socket connection to the SMTP server (if not using SSL)
  BOOL                        m_bConnected;               //Are we currently connected to the server 
  CString                     m_sLastCommandResponse;     //The full last response the server sent us  
  CString                     m_sHeloHostname;            //The hostname we will use in the HELO command
	DWORD                       m_dwTimeout;                //The timeout in milliseconds
  int                         m_nLastCommandResponseCode; //The last numeric SMTP response
  ProxyType                   m_ProxyType;
  CString                     m_sProxyServer;
  int                         m_nProxyPort;
  CString                     m_sLocalBoundAddress;
  CString                     m_sProxyUserName;
  CString                     m_sProxyPassword;
  CString                     m_sUserAgent;
  HINSTANCE                   m_hWininet;  //Instance handle of the "Wininet.dll" which houses the functions we want
  LPINTERNETGETCONNECTEDSTATE m_lpfnInternetGetConnectedState;
  LPINTERNETAUTODIALHANGUP    m_lpfnInternetAutoDialHangup;
  LPINTERNETATTEMPCONNECT     m_lpfnInternetAttemptConnect;
#ifndef CPJNSMTP_NOMXLOOKUP
  HINSTANCE                   m_hDnsapi;   //Instance handle of the "Dnsapi.dll" which houses the other functions we want
  LPDNSRECORDLISTFREE         m_lpfnDnsRecordListFree;
  LPDNSQUERY                  m_lpfnDnsQuery; 
#endif
};

//Provide for backward compatability be defining CSMTPConnection as a preprocessor define
//for CPJNSMTPConnection if we are not using the ATL Server's "CSMTPConnection" class.
#ifndef __ATLSMTPCONNECTION_H__
#define CSMTPConnection CPJNSMTPConnection
#endif

#endif //__PJNSMTP_H__
