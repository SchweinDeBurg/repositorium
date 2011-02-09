/*
Module : W3MFC.cpp
Purpose: Implementation for a simple MFC class encapsulation of a HTTP server
Created: PJN / 22-04-1999
History: PJN / 24-06-1999 1.  Implemented support for "HEAD" command
                          2.  Sample provided now also displays the HTTP verb used
                          3.  Sample provided now also displays the date and time of each request
                          4.  Now fully supports multiple virtual directories
                          5.  Now fully supports URL's with encoded characters
                          6.  Implemented support for "DELETE" command
                          7.  Now returns an "Allow:" HTTP header
                          8.  Timeout for requests is now 90 seconds if built for debug
                          9.  Now supports directory listing
                          10. User name is now displayed in the log window
         PJN / 29-02-2000 1.  Fixed a number of VC 6 level 4 warnings
                          2.  Now empties the username and password once they are used
                          3.  Now implements W3C Common Logfile logging to the console window
         PJN / 15-04-2001 1.  Updated copyright information   
                          2.  A new generalised thread pool wrapper class is now included which can
                          be used in other projects.
                          3.  A new generalised sockets wrapper class is now included which can be 
                          used in other winsock projects
         PJN / 23-04-2001 1. Fixed a stack overwrite issue in CHttpResponseHeader::DateToStr
                          2. Fixed a bug in CHttpClient::TransmitFile which was causing "304 Not Modified"
                          headers to never be returned
         PJN / 25-07-2001 1. Code now uses Winsock 2 functionality, namely "WSAEventSelect" to avoid a busy loop
                          in the listening socket thread. This means that you will have to have Winsock 2 
                          downloaded and installed if running on Windows 95 which did not ship with 
                          Winsock 2 built in.
         PJN / 23-08-2001 1. Added support for CGI v1.1 (has been verfied with perl scripts and batch files)
                          2. Added support for reverse DNS lookups
                          3. Now fully supports POST HTTP method.
                          4. Following values have been added to CHttpRequest:
                             m_pszRawRequest, m_pszRawEntity, m_sRemoteHost and m_sContentType
                          5. Improved the robustness of the code by making sure exceptions are caught
         PJN / 30-09-2001 1. Fixed a bug in CWSocket::Receive which can occur when a disconnection occurs
                          while a receive is in progress
                          2. Fixed a bug where entities sent i.e. m_pszRawEntity did not correctly handle the 
                          case where it had enbedded NULL's e.g. a file upload. Thanks to Christian Hett for 
                          spotting this problem.
                          3. Removed some unnecessary memcpy's from CHttpClient::ParseRequest
                          4. Made the main program configurable via an ini file instead of hard coding all the
                          settings.
                          5. Fixed a resource leak on an anonymous pipe handle in CHttpClient::TransmitCGIResponse.
         PJN / 03-11-2001 1. Fixed an issue where socket sends would sometimes throw the exception WSAEWOULDBLOCK.
                          The httpsocket class now includes a "SendWithRetry" method which handles this case.
         PJN / 27-12-2001 1. Fixed an exception memory leak in the method CHttpSocket::SendWithRetry. Thanks 
                          to Olivier Meyer for spotting this problem.
                          2. Removed an unreferrenced variable from CHttpSocket::ReadResponse
                          3. Added a flag to TransmitFile to allow the transmission of an "Expires: " header
                          4. Added CHttpDirectory parameter to all transmit methods. Allows derived classes
                          more control over the response procedure.
                          5. Added optional Non-NT authentication to the web server at the virtual directory
                          level. 
                          6. Reviewed TRACE statements throughtout W3MFC for completeness and validity
                          7. Addition of a CHttpClient::TransmitBuffer method to allow for easy transmission of 
                          in-memory generated responses. Thanks to "Golden Crater" for most of those suggestions.
                          8. W3MFC now takes advantage of the MS Winsock extension API "TransmitFile". This
                          should improve the performance of the code by avoiding inordinate kernel transitions
                          on NT based operating systems.
                          9. Includes a new error reporting mechanism via the CHttpServer::OnError method
                          10. Now includes a nice icon for the generated binary
                          11. Version info resource is now included in the generated binary
                          12. Modified "Return..." functions to return body length sent
                          13. Check all PostLog calls to ensure indicated parameter size is correct
                          14. W3MFC now takes advantage of the MS Winsock extension API "TransmitPackets". This
                          should improve the performance of the code by avoiding inordinate kernel transitions
                          on Windows XP or later. Please note that I have been unable to test this feature so 
                          feeback on this would be appreciated.
         PJN / 08-01-2002 1. Code only uses TransmitFile and TransmitPackets APIs if running on NT/2000/XP/.Net Server
                          2. Provided a virtual CHttpClient::TransmitFile and CHttpClient::TransmitBuffer functions. 
                          The default implemenations just pass the buck to the _W3MFC_DATA function pointers if 
                          available.
                          3. Made CHttpClient::HandleClient function virtual.
                          4. Fixed an issue in CHttpClient::TransmitDirectory where created URL's could sometimes forget 
                          to put in directory separators in the URL.
                          5. Updated copyright messages to be 2002.
                          6. Provided a virtual CHttpClient::ParseRequestLine to allow derived classes to easily pull
                          out any additional http headers e.g. "User-Agent" or "Accept-Language" etc.
                          7. Provided a virtual CHttpClient::FormCGIEnvironment to allow derived classes to easily change
                          the environment variables sent to CGI processes.
                          8. Fixed a serious bug in CHttpResponseHeader::GetData while code was compiled for UNICODE
                          9. Addition of a very handy new "CW32Handle" class which implements a "Smart Pointer" class
                          for Win32 Handles. For one thing it looks after calling CloseHandle in the destructor. The class
                          turns out to be very handy when you are implementing CGI in this web server as you have to
                          create loads of Win32 handles such as anonymous pipes etc.
                          10. Improved the error reporting in CHttpClient::TransmitCGIResponse
                          11. Rewrote the CGI handling code in CHttpClient::TransmitCGIResponse to use the information 
                          provided by MS Knowledge base article "Q190351".
         PJN / 26-01-2002 1. CIOCP9x clas: added an additional semaphore as I have forgotten that we need to synchronise 
                          the removal of items from the queue in addition to synchronising the addition of items to the 
                          queue.
                          2. Fixed a level 4 warning in the CHttpClient class.
                          3. Only now uses TransmitPackets and TransmitFile if running 2000 / XP or .Net Server. Thanks 
                          to Olivier Meyer for spotting this problem on NT 4.
         PJN / 21-02-2002 1. Fixed an issue in CHttpClient::GetCGICommandLine where the code was using RegQueryValue 
                          instead of RegQueryValueEx which is the recommended way of doing registry queries in Win32.
                          Thanks to Josh Clarke for spotting this problem.
                          2. CGI command line is now expanded with any environment variables it may contain. Again 
                          thanks goes to Josh Clarke for this nice update.
         PJN / 23-03-2002 1. W3MFC ships with a sample CGI app "TestCGI.exe"
                          2. CHttpClient::GetCGICommandLine function now defaults to the local file path if a registry 
                          entry cannot be found for it. Thanks to Gilad Novik for this suggestion.
         PJN / 23-05-2002 1. Now fully supports SSL via the OpenSSL open source Library.
                          2. Removed IOCP9x module from project as not required.
                          3. Fixed a bug in CHttpServer::ListenSocketFunction which was causing the thread pool to initialize
                          incorrectly.
         PJN / 24-05-2002 1. Fixed a bug in the SSL code path which was causing a crash in CHttpClient::TransmitBuffer.
                          2. Provision of wrapper classes for OpenSSL C type variables used by W3MFC
                          3. Tidied up the SSL code in CHttpClient::HandleClient
         PJN / 25-05-2002 1. Improved the performance of CHttpClient::URLDecode method
                          2. Improved the performance of CHttpClient::TransmitDirectory. Thanks to "Jim" for this fix.
                          3. URL's transmitedd down to client in CHttpClient::TransmitDirectory are now URL encoded.
                          4. Fixed a crash which was occurring for some requests in CHttpClient::ParseRequest where the code
                          ended up parsing into the entity body instead of stopping when it reached the separator after the
                          http header. Thanks to Harold B Johns for spotting this.
         PJN / 29-05-2002 1. Environment variables for CGI processes now includes all the environment variables which W3MFC
                          itself has. These missing environment variables were causing CGI programs to fail in calls to
                          CoInitialize if they were using COM. Thanks to Harold B Johns for spotting this.
         PJN / 30-05-2002 1. Fixed a #include issue when doing a non SSL build. Thanks to Harold B Johns for spotting this.
                          2. Fixed an issue where a request for a non existent CGI file caused the web server to become
                          unresponsive. Thanks to Harold B Johns for spotting this.
                          3. Fixed an issue in the CGI code when SSL is enabled. Thanks to Gilad Novik for reporting this
                          problem.
         PJN / 14-06-2002 1. Fixed a problem in CHttpClient::FindNextTerminatorFromRequest which was parsing out header
                          details incorrectly. Thanks to Gilad Novik for reporting this problem.                         
         PJN / 28-07-2002 1. Now CHttpMimeManager can be replaced at runtime. This allows the mime map to be easily configured
                          or replaced with a custom implementation.
                          2. Mime map as used by the sample app is now configured from the W3MFC ini file instead of from
                          the system registry. In addition the shipped W3MFC ini file now includes a fully fleshed out mime 
                          section
                          3. You can now improve the robustness of the SSL behaviour by initializing the OpenSSL PRNG (Pseudo
                          Random Number generator) with the contents of a file as specified via 
                          CHttpServerSettings::m_sSSLRandomnessFile.
                          4. A fresh default web site is now shown using the default ini settings, instead of expecting a web 
                          site to be configured at c:\inetpub\wwwroot
                          5. Improved the performance of looking up mime types by implementing a hash table for storage
         PJN / 18-08-2002 1. Now uses v1.09 of the CThreadPoolServer class.                 
                          2. Sample app now allows you to decide via a ini value whether or not to use the CIOCPThreadPoolQueue
                          class instead of the CDirectedThreadPoolQueue class. While I was at it, I avoiding an issue with
                          leaving an item on the Q if updating the "Get" semaphores failed.
         PJN / 17-10-2002 1. Fixed a bug in CHttpClient::HexToInt which was causing it to not work for upper case characters.
                          Thanks to Frank Schmidt for spotting this problem.
                          Failure to setup thread pool recycling is now reported as an error.
         PJN / 16-11-2002 1. Class now supports the use of the "Connection: Keep-Alive" Header.
                          2. Removed the StringReplace method and replaced it with CString::Replace.
                          3. Fixed some Log message's text to correctly reflect the function they are used in.
                          4. Code now makes the call to CHttpClient::AllowThisConnection before the incoming request is parsed.
                          5. Fixed a typo in the name of the CHttpClient::HandleDirectoryAuthorization function
                          6. Made the operation of the delivery of the "Expires" header contingent on a configurable setting
                          7. CHttpResponseHeader::AddW3MfcAllowFields now returns that it can handle POST requests
                          8. The "DELETE" verb support is now contingent on a configurable setting.
                          9. Fixed a typo in the creation of the "SERVER_PROTOCOL" CGI environment variable
                          10. Updated the sample CGI app to also send a Content-Length HTTP header. This allows the sample
                          app to work correctly when it is used in conjuction with HTTP Keep Alives  
         PJN / 17-11-2002 1. Optimized the use of the Keep Alives variables in the function CHttpClient::HandleClient
                          2. Implemented code to find whether CGI programs send Keep Alives.
         PJN / 18-11-2002 1. Now allows anonymous access to be enabled / disabled
                          2. Now allows Basic authentication to be enabled / disabled.
                          3. Reworked the Base64 class to be based on the ATL Server Base64 routines in ATL Server in VC.Net
                          4. Renamed and Reworked the CHttpClient::ReturnUnauthorizedMessage method.
                          5. Impersonation handle used during Basic authentication now is a CW32Handle instance instead of a 
                          raw SDK handle
                          6. Now fully supports NTLM Authentication via the SSPI interface. The code is enabled for support
                          for Kerberos, "Negotiate" and Digest authentication which will be added in a future release. The use 
                          of SSPI support can be changed via the compile time define "W3MFC_NO_SSPI_SUPPORT". Thanks to 
                          Harold B Johns for suggesting this.
                          7. Fixed a problem in CHttpClient::ParseRequest which was causing failures to parse a certain line
                          to be overwritten upon successfull parsing of subsequent lines. 
                          8. test CGI app now is implemented without MFC support and deliberatly does not use a Content-Length
                          or Keep-Alive header for testing purposes.
                          9. Tidied up the importing of various header files throughout W3MFC and also the associated
                          #pragma messages which these something display.
         PJN / 18-11-2002 1. CHttpSocket::ReadResponse now uses waitable timers (if it can) instead of a loop which has calls
                          to Sleep. This should improve the scalabilty of the code under heavy load. Please note that if you
                          want to compile the code, then you will need to download the CWaitableTimer source code separately
                          from my web site and copy in the wtimer.h & wtimer.cpp into your W3MFC's source code directory.
                          2. Added code to protect against NTFS alternate streams being accessed by client browsers.
                          3. Improved the robustness of parsing the authorization header fields.
         PJN / 20-11-2002 1. Fixed a number of level 4 warnings about unreferrenced variables. Thanks to Harold Johns for 
                          reporting these.
                          2. Updated the documentation to refer to the fact that you will need an up to date Platform SDK 
                          to be installed to compile W3MFC with SSPI support.
                          3. Fixed a memory leak of CHttpRequest objects which can occur when you shutdown the web server and
                          you are using a directed thread pool queue and there are outstanding items in the queue which are
                          not shutdown requests
         PJN / 03-02-2003 1. W3MFC now compares HTTP headers without regard to case. Thanks to Gilad Novik and Frank Hahn for 
                          reporting these problems.
                          2. Tidied up inclusion of afxpriv.h throughout W3MFC modules.
         PJN / 21-02-2003 1. Made the m_Directories member of CHttpServerSettings a pointer array. This allows client code
                          to add their own derived instances of CHttpDirectory to the array. This allows per directory 
                          customisation of the web server. This change also necessitated changing the settings class of the 
                          CHttpServer to be a pointer also. Thanks to "Edgar" for this update.
                          2. Remove the digest authentication boolean from the settings class as Digest authentication is 
                          currently not supported.
                          3. Made the CHttpClient::LoadHTMLResource method virtual
                          4. Moved a lot of the CHttpClient implementation code to CHttpDirectory. This allows more additional
                          complex client customisation of the code.
                          5. Split off the CHttpDirectory class into its own module.
                          6. Implemented a virtual CHttpDirectory::HandleClient. This allows customisation of per directory
                          responses. Thanks to "Edgar" for this suggestion.
         PJN / 22-02-2003 1. Fixed a bug in the parsing of SSL requests which was causing heap corruption problems on
                          subsequent reads of SSL requests on the same thread.
                          2. Reworked the CHttpClient::HandleClient to avoid having to call various functions when the code
                          needs to exit prematurely
                          3. Added a member variable "m_bResponseKeepAlive" to avoid having to pass a keep alive variable
                          throughout all the CHttpClient code.
                          4. Fixed a deadlock bug in CHttpServer::Stop. Thanks to "Edgar" for reporting this problem.
                          5. modified the Mime manager class method "GetMimeType" to be sent the full request rather than just
                          the extension of the URL when it is called to determine the mime type.
                          6. Addition of a string version of the HTTP verb to CHttpRequest. This speeds up the CGI and 
                          ISAPI implementations somewhat.
                          7. Addition of a hash table in the request structure which contains all the HTTP request headers. These
                          values are now available via CGI or ISAPI
                          8. Split of the CGI implementation into its own module and made it optional via a preprocessor directive
                          9. Split of the ISAPI implementation into its own module and made it optional via a preprocessor directive
                          10. W3MFC now uses Win32 TransmitFile API in SSL configuration and only falls back to user mode sockets
                          if SSL is actively being used.
                          11. W3MFC now supports PATH_INFO information in URL's
                          12. Optimized loading of error pages which W3MFC uses
         PJN / 02-03-2002 1. Removed the function CHttpDirectory::SetClient as it can lead to thread synchronisation problems. 
                          Thanks to "Edgar" for reporting this problem.
         PJN / 03-03-2202 1. Added a few defines to allow the W3MFC to compile if you do not have the latest version of the Platform
                          SDK installed. Thanks to "Edgar" for reporting this.
                          2. Fixed a copy and paste gremlin in CHttpServer::Start which was causing a critical section to not be
                          acquired. Thanks to "Edgar" for reporting this.
                          3. Removed the use of "friend classes" entirely throughout the W3MFC codebase. This avoids potential 
                          compilation problems in any derived classes used by client code of W3MFC.
                          4. Addition of a number of preprocessor defines, namely W3MFC_EXT_CLASS, THRDPOOL_EXT_CLASS and 
                          SOCKMFC_EXT_CLASS. This allows the classes to easily be added and exported from a MFC extension dll.
         PJN / 12-09-2003 1. Removed double definition of SCRIPT_NAME from CGI environment. Thanks to Dave Horner for reporting
                          this.
                          2. SCRIPT_NAME CGI environment variable now uses forward slashes rather than back slashes as
                          directory separators. Thanks to Dave Horner for reporting this problem.
                          3. Added a "REQUEST_URI" CGI environment variable. Thanks to Dave Horner for reporting this.
                          4. CGI implementation now returns a HTTP return code line if one if the CGI script does not provide one.
                          Again thanks to Dave Horner for reporting this issue.
                          5. CGI implementation now returns immedately from CHttpCGI::ReadFromClientStdout from a socket error is
                          detected. Again thanks to Dave Horner for reporting this issue.
                          6. "PATH_TRANSLATED" CGI and ISAPI environment variable is now reported as an absolute path. Again thanks 
                          to Dave Horner for reporting this issue.
                          7. "SCRIPT_NAME" CGI and ISAPI environment variable now includes an initial "/". Again thanks to Dave 
                          Horner for reporting this issue.
                          8. CGI now uses pClient->m_Request.m_dwRawEntitySize variable when determining when W3MFC needs to 
                          write to the CGI child process.
                          9. ISAPI implementation now sends just the HTTP body to client ISAPI dlls instead of the whole HTTP
                          request. Thanks to Michael St. Laurent for reporting this problem.
         PJN / 03-11-2003 1. Simplified the code in CHttpSocket::ReadResponse. Thanks to Clarke Brunt for reporting this issue.
         PJN / 10-11-2003 1. URL Encoded spaces as "+" are now correctly handled. Thanks to Dan Baker for reporting this issue. 
                          2. CHttpRequest now includes a m_sRawURL member variable which provides access to the raw URL 
                          before it is URL decoded. Thanks to Dan Baker for suggesting this addition.
                          3. Shipped binary for W3MFC is now linked with the OpenSSL 0.9.6l which is the latest version currently.
         PJN / 30-03-2004 1. Tidied up the interaction of the various classes by removing all friend classes
         PJN / 25-08-2004 1. The binaries included in the download now link against OpenSSL 0.9.7d.
                          2. Per thread cleanup is now done for OpenSSL. Again thanks to Leandro Gustavo Biss Becker for reporting 
                          this problem.
                          3. OpenSSL is now configured by the W3MFC sample app to operate in a thread safe manner. Again thanks 
                          to Leandro Gustavo Biss Becker for reporting this problem.
                          4. Application initialization of OpenSSL has been taken out of W3MFC and is now the responsibility of 
                          your application. See the sample W3MFC application code in main.cpp for an example on how to correctly
                          initialize and terminate OpenSSL.
                          5. CSSL::Close() now calls SSL_shutdown to ensure SSL connections are properly terminated. Thanks to 
                          to Leandro Gustavo Biss Becker for reporting this problem.
                          6. W3MFC now correctly handles UTF8 encoded URL requests. Thanks to Huang Wei for reporting this problem
                          and providing the fix.
         PJN / 15-10-2004 1. Removed an unnecessary ASSERT from CHttpClient::MapURLToLocalFilename. Thanks to Jan Bares for 
                          reporting this problem.
                          2. Changed the behaviour of the code in CHttpClient::MapURLToLocalFilename as follows:

                          Before Change

                          Requesting the URL "/name" would return the file "name" in the root directory of the web server even if
                          a "/name" virtual directory existed. If "name" did not exist in the root directory then a 404 error 
                          would be returned

                          After Change

                          Requesting the URL "/name" will return the virtual directory "/name" if such a virtual directory exists. 
                          If not then the file "name" in the root directory will be returned.

                          Thanks to Jan Bares for pointing out this behaviour which is inconsistent with other Web Server implementations.

                          3. URLs which include "@" now are parsed correctly. Previously the code was parsing the URI
                          expecting it to contain username and password in the URL itself. Instead when the URI arrives at the 
                          server it is not in the URI itself but is represented in the HTTP request headers. Thanks to Jan Bares for 
                          pointing out this problem.
                          4. Passthrough authentication can now be disabled via a new CHttpServerSettings::m_bPerformPassthroughAuthentication
                          setting. This is useful where you want to setup per directory protection using a username / password pair but
                          you do not want to use these credentials in an attempt to impersonate that user by calling the SDK function 
                          "LogonUser". Again thanks to Jan Bares for this suggestion.
         PJN / 11-11-2004 1. Added a m_sRawExtra variable to CHttpRequest. This value is now passed to CGI and ISAPI instead of m_sExtra. 
                          This allows apps to correctly parse form parameters. Thanks to Jan Bares for reporting this problem.
         PJN / 19-02-2004 1. Fixed a bug when directory lists were displayed by W3MFC. It incorrectly always dropped the last entry from
                          the listing. Thanks to Pablo Romero for reporting this bug.
         PJN / 30-10-2005 1. Fixed a bug in CHttpCGI::ReadFromClientStdout when parsing CGI responses which had embedded HTTP headers in them.
                          Thanks to Gregoire Gentil for reporting this bug.
                          2. Shipped binary for W3MFC is now linked with the OpenSSL 0.9.7i which is the latest version currently. 
         PJN / 04-11-2005 1. Fixed a further bug in the CGI implementation where subsequent HTTP chunks which are transmitted to clients
                          include an additional HTTP Status line. Thanks to Gregoire Gentil for reporting this persistant bug.
                          2. Fixed an issue where the returned HTTP status line by W3MFC did not have the correct EOL terminator.
         PJN / 03-01-2006 1. Updated the copyrights in all the W3MFC modules.
                          2. Fixed an issue in the CGI implementation so that HTTP headers such as User-Agent get mapped to HTTP_USER_AGENT 
                          environment variables. Basically all HTTP_... values should have any hyphens in their names replaced by underscores.
                          3. Now includes support for the DOCUMENT_ROOT CGI environment variable
                          4. Now returns a HTTP status code of 302 if a "Content:" header is found but no status code is present. Thanks to 
                          Hector Santos for reporting this issue.
                          5. Now CGI implementation supports Complete (Non-Parsed) CGI scripts. If you prefix your CGI script with "NPR-" then 
                          the data as received from the script will be sent to the client unmodified. Thanks to Hector Santos for suggesting this
                          nice addition.
                          6. Parsing of the HTTP status code line e.g. "HTTP/1.0 200 Ok" is now done using the CGI "Status:" header rather than 
                          looking for a "HTTP/.." line. Again thanks to Hector Santos for reporting this.
         PJN / 19-02-2006 1. Replaced all calls to ZeroMemory and CopyMemory with memset and memcpy
         PJN / 31-05-2006 1. Fixed a memory leak in CHttpServer::ListenSocketFunction which occurs when an exception is thrown because the client
                          socket connection is not accepted or when the request fails to be sent down to the thread pool. Thanks to Daniel Berman 
                          for reporting this bug.
         PJN / 27-06-2006 1. Code now uses new C++ style casts rather than old style C casts where necessary. 
                          2. Updated the code to clean compile on VC 2005
         PJN / 21-09-2006 1. Code now securely disposes of CHttpServer::m_sUsername, CHttpServer::m_sPassword, CHttpRequest::m_sUserName, 
                          CHttpRequest::m_sPassword using SecureZeroMemory. Note that the CHttpDirectory::m_sUsername and CHttpDirectory::m_sPassword
                          values are also securely cleaned up but these are long lived values and as such if a crash occurs, these values will
                          most likely appear in any memory dumps.
                          2. The W3MFC binary which is include in the download is now built with VC 2005 and dynamically links to the latest 
                          version of OpenSSL namely 0.9.8c, which you will need to build yourself, if you want to use the W3MFC exe as is. Also
                          bear in mind that the new exe requires the VC 2005 MFC support dlls to be in place for it to run on client machines.
         PJN / 12-01-2007 1. Updated copyright details.
                          2. Optimized CHttpServerSettings constructor code.
                          3. Fixed a simple copy and paste issue in the IDH_302 built in html resource. Thanks to G. Wilmes for reporting this
                          issue.
         PJN / 19-11-2007 1. Updated W3MFC binary to now link to OpenSSL 0.9.8g
         PJN / 31-12-2007 1. Updates sample apps to clean compile on VC 2008
         PJN / 31-05-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
         PJN / 08-06-2008 1. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
         PJN / 23-05-2009 1. Updated copyright details
                          2. Updated the sample app's project settings to more modern default values.
                          3. Removed some outstanding VC 6 style code from the codebase
                          4. Removed use of CT2A throughout the code.
         PJN / 09-01-2011 1. Updated copyright details.
                          2. Updated code to compile with latest version of OpenSSL
                          3. Updated TestISAPI project to warn if it is compiled in VC 2008 or later (MFC ISAPI support was dropped from VC 2008!)

Copyright (c) 1999 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////// Includes /////////////////////////////////////////////////////

#include "stdafx.h"
#include "W3MfcRes.rh"
#include "SocMFC.h"
#include "W3MFCClient.h"
#include "W3MFC.h"
#ifdef W3MFC_SSL_SUPPORT
#include "OpenSSLMfc.h"
#include <openssl\rand.h>
#endif


//////////////// Macros / Defines /////////////////////////////////////////////

//automatically pull in Winsock 2
#pragma comment(lib, "Ws2_32.lib")

//automatically pull in SSPI (if required)
#ifndef W3MFC_NO_SSPI_SUPPORT
#pragma comment(lib, "secur32.lib")
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////// Implementation ///////////////////////////////////////////////

CW3MFCServerSettings::CW3MFCServerSettings() : m_nPort(80),                  //Default to the standard HTTP port
                                               m_bBind(FALSE),               //Default to not binding to a specific IP address
                                               m_bIPv6(FALSE),
                                             #ifdef _DEBUG
                                               m_dwIdleClientTimeout(90000), //Default to client idle timeout of 90 seconds (when in debug mode)
                                             #else
                                               m_dwIdleClientTimeout(30000), //Default to client idle timeout of 30 seconds
                                             #endif
                                               m_sServerName(_T("W3MFC/1.92")), //Default server name will be the name of the MFC classes i.e "W3MFC" plus the current version number 
                                               m_pRuntimeClientClass(RUNTIME_CLASS(CW3MFCClient)), //Default class to use is CW3MFCClient
                                               m_nThreadPoolSize(10),
                                               m_bDNSLookup(FALSE),
                                               m_dwWritableTimeout(10000),
                                               m_bEnableThreadLifetime(FALSE),
                                               m_dwThreadLifetime(120),
                                               m_pMimeManager(NULL),
                                               m_bUseIOCPQueue(FALSE),
                                               m_bKeepAlives(TRUE),
                                               m_bAutoExpire(FALSE),
                                             #ifndef W3MFC_NO_CGI_SUPPORT
                                               m_dwCGIResponseBufferSize(4096),
                                             #endif
                                               m_bAllowAnonymous(TRUE),
                                               m_bAllowBasicAuthentication(TRUE),
                                               m_bAllowNTLMAuthentication(FALSE),
                                               m_bPerformPassthroughAuthentication(TRUE),
                                             #ifndef W3MFC_NO_ISAPI_SUPPORT
                                               m_bCacheISAPI(TRUE),
                                               m_pISAPIManager(NULL),
                                               m_pISAPI(NULL),
                                             #endif
                                             #ifndef W3MFC_NO_CGI_SUPPORT
                                               m_pCGI(NULL),
                                             #endif
                                             #ifdef W3MFC_SSL_SUPPORT
                                               m_SSLProtocol(SSL_NONE),
                                               m_sServerCertificateFile(_T("W3MFC.pem")),
                                               m_bReuseSessions(TRUE),
                                               m_dwSSLSessionTimeout(300),
                                               m_dwSSLNegotiationTimeout(10000),
                                             #endif
                                               m_bAllowDeleteRequest(FALSE)
{
  //Default root directory will be where the exe is running from
  TCHAR sPath[_MAX_PATH];
  GetModuleFileName(NULL, sPath, _MAX_PATH);
  TCHAR sDrive[_MAX_DRIVE];   
  TCHAR sDir[_MAX_DIR];
  _tsplitpath_s(sPath, sDrive, sizeof(sDrive)/sizeof(TCHAR), sDir, sizeof(sDir)/sizeof(TCHAR), NULL, 0, NULL, 0);
  _tmakepath_s(sPath, sizeof(sPath)/sizeof(TCHAR), sDrive, sDir, NULL, NULL);

  CW3MFCDirectory* pDir = new CW3MFCDirectory;
  pDir->SetDirectory(sPath);    
  pDir->SetAlias(_T("/"));
  pDir->SetDefaultFile(_T("default.htm")); //Default filename returned for root requests will be "default.htm"
  m_Directories.Add(pDir);
}

CW3MFCServerSettings::~CW3MFCServerSettings()
{
  FreeDirectoryArray();
}

void CW3MFCServerSettings::FreeDirectoryArray()
{
  for (INT_PTR i=0; i<m_Directories.GetSize(); i++)
  {
    CW3MFCDirectory* pDir = m_Directories.GetAt(i);
    delete pDir; 
    pDir = NULL;
  }
  m_Directories.RemoveAll();
}



IMPLEMENT_DYNCREATE(CW3MFCDirectedThreadPoolQueue, CDirectedThreadPoolQueue)

CW3MFCDirectedThreadPoolQueue::~CW3MFCDirectedThreadPoolQueue()
{
  //Before we are destroyed, deallocate any user requests
  //which may still be lying around in the requests array
  for (int i=0; i<m_Requests.GetSize(); i++)
  {
    CThreadPoolRequest& request = m_Requests.ElementAt(i);
    if (request.m_dwID == CThreadPoolRequest::THREADPOOL_USER_DEFINED_REQUEST)
    {
      CW3MFCThreadPoolRequest* pW3MFCRequest = static_cast<CW3MFCThreadPoolRequest*>(request.m_pData);
      delete pW3MFCRequest;
      pW3MFCRequest = NULL;
    }
  }
}


CW3MFCServer::CW3MFCServer() : m_ListenStartEvent(TRUE), 
                               m_pListenThread(NULL)
{
#ifndef W3MFC_NO_SSPI_SUPPORT
  memset(&m_hCredHandle, 0, sizeof(m_hCredHandle));
#endif

  LoadHTMLResources();
}

IMPLEMENT_DYNAMIC(CW3MFCServer, CObject)

void CW3MFCServer::LoadHTMLResources()
{
  LoadHTMLResource(IDH_302, m_s302HTML);
  LoadHTMLResource(IDH_400, m_s400HTML);
  LoadHTMLResource(IDH_401, m_s401HTML);
  LoadHTMLResource(IDH_404, m_s404HTML);
  LoadHTMLResource(IDH_500, m_s500HTML);
  LoadHTMLResource(IDH_FILE_DELETED_OK, m_sDeletedHTML);
}

CW3MFCServer::~CW3MFCServer()
{
  Stop();
}

#ifndef W3MFC_NO_SSPI_SUPPORT
BOOL CW3MFCServer::AcquireSSPI()
{
  //Get a connection to SSPI
  TimeStamp tsExpires;
  SECURITY_STATUS ss = AcquireCredentialsHandle(NULL, _T("NTLM"), SECPKG_CRED_INBOUND, NULL, NULL, NULL, NULL, &m_hCredHandle, &tsExpires);
  BOOL bSuccess = (ss == SEC_E_OK);

  //Report the error
  if (!bSuccess)
  {
    CString sError;
    sError.Format(_T("CW3MFCServer::AcquireSSPI, Failed in call to AcquireCredentialsHandle, Error:%d"), ss);
    OnError(sError);
  }

  return bSuccess;
}

void CW3MFCServer::ReleaseSSPI()
{
  SECURITY_STATUS ss = FreeCredentialsHandle(&m_hCredHandle);
  if (ss != SEC_E_OK)
  {
    CString sError;
    sError.Format(_T("CW3MFCServer::ReleaseSSPI, Failed in call to FreeCredentialsHandle, Error:%d"), ss);
    OnError(sError);
  }
}
#endif

BOOL CW3MFCServer::Start(CW3MFCServerSettings* pSettings)
{
  CSingleLock sl(&m_csListenThread, TRUE); //synchronize access to the listening thread

  AFXASSUME(m_pListenThread == NULL); //Trying to start an already started server
  AFXASSUME(pSettings);
  AFXASSUME(pSettings->m_pMimeManager); //must be assigned before you call this function
#ifndef W3MFC_NO_ISAPI_SUPPORT
  AFXASSUME(pSettings->m_pISAPIManager); //must be assigned before you call this function
  AFXASSUME(pSettings->m_pISAPI); //must be assigned before you call this function
  pSettings->m_pISAPI->SetServer(this);
#endif
#ifndef W3MFC_NO_CGI_SUPPORT
  AFXASSUME(pSettings->m_pCGI); //must be assigned before you call this function
#endif

  //Initialize SSPI
#ifndef W3MFC_NO_SSPI_SUPPORT
  if (!AcquireSSPI())
    return FALSE;
#endif

  //Start the listener thread
  m_pSettings = pSettings;
  m_bListenerRunningOK = FALSE;
  m_StopEvent.ResetEvent();
  m_ListenStartEvent.ResetEvent();
  m_pListenThread = AfxBeginThread(ListenSocketFunction, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
  if (m_pListenThread == NULL)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::Start, Failed to create listener thread, please check settings"));
    OnError(sError);
    return FALSE;
  }
  m_pListenThread->m_bAutoDelete = FALSE;
  m_pListenThread->ResumeThread();

  //Wait until the thread has completely started and return the success indicator
  ::WaitForSingleObject(m_ListenStartEvent, INFINITE);
  return m_bListenerRunningOK;
}

BOOL CW3MFCServer::Wait()
{
  CSingleLock sl(&m_csListenThread, TRUE); //synchronize access to the listening thread

  //If the listener thread is running, then just wait for it to exit
  if (m_pListenThread)
  {
    ::WaitForSingleObject(m_pListenThread->m_hThread, INFINITE);
    delete m_pListenThread;
    m_pListenThread = NULL;
    return TRUE;
  }
  else
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::Wait, HTTP server is not running, so no need to wait"));
    OnError(sError);
    return FALSE;
  }
}

BOOL CW3MFCServer::Stop()
{
  //Signal the listener thread to stop and wait for it to do so, Note that we do 
  //this before we enter the critical section because otherwise we could end
  //up in a deadlock situation between this function and Wait
  m_StopEvent.SetEvent();

  CSingleLock sl(&m_csListenThread, TRUE); //synchronize access to the listening thread

  //If the listener thread is running, then stop it
  if (m_pListenThread)
  {
    ::WaitForSingleObject(m_pListenThread->m_hThread, INFINITE);
    delete m_pListenThread;
    m_pListenThread = NULL;

    //Release SSPI
  #ifndef W3MFC_NO_SSPI_SUPPORT
    ReleaseSSPI();
  #endif
  }

  return TRUE;
}

BOOL CW3MFCServer::ReverseDNSLookup(in_addr sin_addr, CString& sDomainName)
{
  BOOL bSuccess = FALSE;
  HOSTENT* pHostEnt = gethostbyaddr(reinterpret_cast<const char*>(&sin_addr), sizeof(sin_addr), AF_INET); 
  if (pHostEnt)
  {
    bSuccess = TRUE;
    sDomainName = pHostEnt->h_name;
  }

  return bSuccess;
}

#ifdef W3MFC_SSL_SUPPORT
void CW3MFCServer::LogSSLErrors()
{
  char sError[1024];
  unsigned long nError;
  do
  { 
    nError = ERR_get_error();
    if (nError)
    {
      ERR_error_string(nError, sError);
      OnError(CString(sError));
    }
  }
  while (nError);
}
#endif

void CW3MFCServer::ListenSocketFunction()
{
  //Setup the thread pool 
  AFXASSUME(m_pSettings);
  AFXASSUME(m_pSettings->m_pRuntimeClientClass);
  if (!m_ThreadPool.Start(m_pSettings->m_pRuntimeClientClass, m_pSettings->m_bUseIOCPQueue ? RUNTIME_CLASS(CIOCPThreadPoolQueue) : RUNTIME_CLASS(CW3MFCDirectedThreadPoolQueue), 
                          m_pSettings->m_nThreadPoolSize, m_pSettings->m_nThreadPoolSize, TRUE))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to create thread pool, GetLastError:%d"), ::GetLastError());
    OnError(sError);
    m_ListenStartEvent.SetEvent();
    return;
  }

  //Setup the pointers in the thread pool instances and resume the threads
  for (int i=0; i<m_pSettings->m_nThreadPoolSize; i++)
  {
    CW3MFCClient* pClient = static_cast<CW3MFCClient*>(m_ThreadPool.GetAtClient(i));
    AFXASSUME(pClient);
    ASSERT(pClient->IsKindOf(RUNTIME_CLASS(CW3MFCClient)));
    pClient->m_pServer = this;
    pClient->m_pWorkerThread->ResumeThread();
  }

  //Setup the recycling of threads
  if (!m_ThreadPool.SetMaxThreadClientLifetime(m_pSettings->m_bEnableThreadLifetime, m_pSettings->m_dwThreadLifetime))
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed in call to m_ThreadPool.SetMaxThreadClientLifetime, GetLastError:%d"), ::GetLastError());
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    return;
  }

  //Bind the server socket
  CW3MFCSocket serverSocket;
  try
  {
    serverSocket.SetBindAddress(m_pSettings->m_sBindAddress);
    serverSocket.CreateAndBind(m_pSettings->m_nPort, SOCK_STREAM, m_pSettings->m_bIPv6 ? AF_INET6 : AF_INET);
  }
  catch(CWSocketException* pEx)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to bind server socket, GetLastError:%d"), pEx->m_nError);
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    pEx->Delete();  
    return;
  }

  //Put the server socket in a listening state
  try
  {
    serverSocket.Listen();
  }
  catch(CWSocketException* pEx)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to listen on server socket, GetLastError:%d"), ::GetLastError());
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    pEx->Delete();
    return;
  }

  //Run the server under a different account if configured to do so
  BOOL bUseAccount = (m_pSettings->m_sUserName.GetLength() != 0);
  BOOL bLoggedOn = FALSE;
  BOOL bImpersonated = FALSE;
  if (bUseAccount)
  {
    LPTSTR pszUser = m_pSettings->m_sUserName.GetBuffer(m_pSettings->m_sUserName.GetLength());
    LPTSTR pszPassword = m_pSettings->m_sPassword.GetBuffer(m_pSettings->m_sPassword.GetLength());
    ASSERT(m_hImpersonation.m_h == NULL);
    bLoggedOn = LogonUser(pszUser, NULL, pszPassword, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &m_hImpersonation.m_h);
    if (bLoggedOn)
      bImpersonated = ImpersonateLoggedOnUser(m_hImpersonation);
    else
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to logon using user name: %s, GetLastError:%d"), pszUser, ::GetLastError());
      OnError(sError);
    }
    m_pSettings->m_sUserName.ReleaseBuffer();
    m_pSettings->m_sPassword.ReleaseBuffer();

    //Cleanly dispose of the password now that we no longer require them (Note we do not dispose of the Username as it may be required
    //for the CGI and ISAPI implementations
    CWSocket::SecureEmptyString(m_pSettings->m_sPassword);
  } 

  //Handle the case if the impersonation failed
  if (bUseAccount && !bImpersonated)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to impersonate using supplied user credentials"));
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    return;
  }

  //Initialize SSL context if required to do so
#ifdef W3MFC_SSL_SUPPORT
  //Initialize the PRNG in OpenSSL
  if (m_pSettings->m_sSSLRandomnessFile.IsEmpty())
  {
    //Get the default rand file from OpenSSL
    char sSSLFile[1024];
    const char* pszRandFile = RAND_file_name(sSSLFile, sizeof(sSSLFile));
    if (pszRandFile)
      m_pSettings->m_sSSLRandomnessFile = pszRandFile;
    else
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to get the default RAND file from OpenSSL"));
      OnError(sError);

      LogSSLErrors();
    }
  }
  RAND_load_file(CStringA(m_pSettings->m_sSSLRandomnessFile), -1);
  if (RAND_status() == 0)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to initialize the PRNG in OpenSSL"));
    OnError(sError);

    LogSSLErrors();

    //Failing to initialize the PRNG is not considered critical for W3MFC
  }

  CSSLContext sslContext;
  if (m_pSettings->m_SSLProtocol != CW3MFCServerSettings::SSL_NONE)
  {
    //Pick the SSL protocol to use
    const SSL_METHOD* pSSLMethod = NULL;
    switch (m_pSettings->m_SSLProtocol)
    {
      case CW3MFCServerSettings::SSL_V2:
      {
        pSSLMethod = SSLv23_server_method();
        break;
      }
      case CW3MFCServerSettings::SSL_V3:
      {
        pSSLMethod = SSLv3_server_method();
        break;
      }
      case CW3MFCServerSettings::SSL_V2_OR_V3:
      {
        pSSLMethod = SSLv23_server_method();
        break;
      }
      case CW3MFCServerSettings::TLS_V1:
      {
        pSSLMethod = TLSv1_server_method();
        break;
      }
      default:
      {
        ASSERT(FALSE);
        break;
      }
    }

    //Create the SSL context object
    ASSERT(pSSLMethod != NULL);
    SSL_CTX* pSSLContext = SSL_CTX_new(pSSLMethod);
    if (pSSLContext == NULL) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to create SSL context object"));
      OnError(sError);

      LogSSLErrors();

      m_ListenStartEvent.SetEvent();
      return;
    }
    else
      sslContext.Attach(pSSLContext);

    //Set the certificate to use
    if (SSL_CTX_use_certificate_file(sslContext, CStringA(m_pSettings->m_sServerCertificateFile), SSL_FILETYPE_PEM) != 1) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to load up server certificate"));
      OnError(sError);

      LogSSLErrors();

      m_ListenStartEvent.SetEvent();
      return;
    }

    //Set the private key to use
    if (SSL_CTX_use_PrivateKey_file(sslContext, m_pSettings->m_sPrivateKeyFile.GetLength() ? 
                                    CStringA(m_pSettings->m_sPrivateKeyFile) : CStringA(m_pSettings->m_sServerCertificateFile), SSL_FILETYPE_PEM) != 1) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed to load up private key"));
      OnError(sError);

      LogSSLErrors();

      m_ListenStartEvent.SetEvent();
      return;
    }

    //Check that we the private key is ok
    if (SSL_CTX_check_private_key(sslContext) != 1) 
    {
      //Report the error
      CString sError;
      sError.Format(_T("CW3MFCServer::ListenSocketFunction, Private key does not match the certificate public key"));
      OnError(sError);

      LogSSLErrors();  

      m_ListenStartEvent.SetEvent();
      return;
    }

    //Set the SSL session settings
    if (m_pSettings->m_bReuseSessions)
    {
      int nOldTimeout;
      nOldTimeout = SSL_CTX_set_timeout(sslContext, m_pSettings->m_dwSSLSessionTimeout);
    }
    else
      SSL_CTX_set_timeout(sslContext, 0);
  }
#endif

  //Use a Win32 event notification on the server socket
  int nError = WSAEventSelect(serverSocket, m_SocketEvent, FD_ACCEPT);
  if (nError == SOCKET_ERROR)
  {
    //Report the error
    CString sError;
    sError.Format(_T("CW3MFCServer::ListenSocketFunction, Failed in call to WSAEventSelect, GetLastError:%d"), ::GetLastError());
    OnError(sError);

    m_ListenStartEvent.SetEvent();
    return;
  }

  //We're now ready for accepting client connections, inform
  //the main thread that everthing is ok
  m_bListenerRunningOK = TRUE;
  m_ListenStartEvent.SetEvent();

  //Create the wait handles array (used for waiting for an event)
  HANDLE Handles[2];
  Handles[0] = m_StopEvent;
  Handles[1] = m_SocketEvent;

  //Wait for any incoming connections and the signal to exit the thread m_ListenStopEvent
  BOOL bWantStop = FALSE;
  CThreadPoolQueue* pQueue = m_ThreadPool.GetQueue();
  ASSERT(pQueue);
  while (!bWantStop)
  {
    DWORD dwWait = WaitForMultipleObjects(2, Handles, FALSE, INFINITE);
    int nSignaledHandle = dwWait - WAIT_OBJECT_0;
    if (nSignaledHandle == 0) //It was the stop request
      bWantStop = TRUE;
    else if (nSignaledHandle == 1) //It was a socket event
    {
      CW3MFCThreadPoolRequest* pRequest = new CW3MFCThreadPoolRequest;
      try
      {
      #ifdef W3MFC_SSL_SUPPORT
        pRequest->m_pSSLContext = &sslContext;
      #endif

        //Accept the client connection
        int nClientAddressLength = sizeof(pRequest->m_ClientAddress);
        serverSocket.Accept(pRequest->m_ClientSocket, reinterpret_cast<SOCKADDR*>(&pRequest->m_ClientAddress), &nClientAddressLength);

        //Send the request down to the thread pool
        CThreadPoolRequest poolRequest;
        poolRequest.m_dwID = CThreadPoolRequest::THREADPOOL_USER_DEFINED_REQUEST;
        poolRequest.m_pData = pRequest;  
        if (!pQueue->PostRequest(poolRequest))
        {
          //Avoid the memory leak by cleaning up the CW3MFCThreadPoolRequest object
          delete pRequest;

          //Report the error
          CString sError;
          sError.Format(_T("CW3MFCServer::ListenSocketFunction, An error occurred posted the request to the thread pool, GetLastError:%d"), ::GetLastError());
          OnError(sError);
        }
      }
      catch(CWSocketException* pEx)
      {
        //Avoid the memory leak by cleaning up the CW3MFCThreadPoolRequest object
        delete pRequest;

        //Report the error
        CString sError;
        sError.Format(_T("CW3MFCServer::ListenSocketFunction, An error occurred accepting a client connection, GetLastError:%d"), pEx->m_nError);
        OnError(sError);

        pEx->Delete();
      }
    }
  }

  //Revert back to normal security settings
  if (bUseAccount)
  {
    //Revert to the usual security settings
    if (bImpersonated)
      RevertToSelf();
  }

  //Bring down the thread pool
  m_ThreadPool.Stop();
}

UINT CW3MFCServer::ListenSocketFunction(LPVOID pParam)
{
  //Get back the "this" pointer from the pParam parameter
  CW3MFCServer* pServer = static_cast<CW3MFCServer*>(pParam);
  AFXASSUME(pServer);
  ASSERT(pServer->IsKindOf(RUNTIME_CLASS(CW3MFCServer)));

  //Call the run method of the CW3MFCServer instance
  pServer->ListenSocketFunction();

#ifdef W3MFC_SSL_SUPPORT
  //cleanup SSL thread state
  ERR_remove_state(0);
#endif

  //Return the thread exit code
  return TRUE;
}

#ifdef _DEBUG
void CW3MFCServer::OnError(const CString& sError) //To avoid level 4 warning in release
#else
void CW3MFCServer::OnError(const CString& /*sError*/)
#endif
{
#ifdef _DEBUG
  //The default is to just TRACE the details
  TRACE(_T("%s\n"), sError.operator LPCTSTR());
#endif
}

BOOL CW3MFCServer::LoadHTMLResource(UINT nID, CStringA& sHTML)
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  HMODULE hModule = AfxFindResourceHandle(MAKEINTRESOURCE(nID), RT_HTML);
  HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(nID), RT_HTML);
  if (hRsrc)
  {
    //Load up the resource
    DWORD dwSize = SizeofResource(hModule, hRsrc); 
    HGLOBAL hGlobal = LoadResource(hModule, hRsrc);

    //Allocate a new char array and copy the HTML resource into it 
    if (hGlobal)
    {
      LPSTR pszHTML = sHTML.GetBuffer(dwSize + 1);
      char* pszResource = static_cast<char*>(LockResource(hGlobal));
      if (pszResource)
      {
        strncpy_s(pszHTML, dwSize + 1, pszResource, dwSize);
        pszHTML[dwSize] = _T('\0');
        bSuccess = TRUE;
      }
      else
      {
        //Report the error
        TRACE(_T("CW3MFCServer::LoadHTMLResource, Failed to load HTML resource, GetLastError:%d\n"), ::GetLastError());
      }
      sHTML.ReleaseBuffer();
    }
  }

  return bSuccess;
}

CStringA CW3MFCServer::LoadHTML(int nStatusCode)
{
  //What will be the return value
  CStringA sHTML;

  switch (nStatusCode)
  {
    case 302:
    {
      sHTML = m_s302HTML;
      break;
    }
    case 400:
    {
      sHTML = m_s400HTML;
      break;
    }
    case 401:
    {
      sHTML = m_s401HTML;
      break;
    }
    case 404:
    {
      sHTML = m_s404HTML;
      break;
    }
    default:
    {
      sHTML = m_s500HTML;
      break;
    }
  }

  return sHTML;
}

CStringA CW3MFCServer::GetFileDeletedHTML()
{
  return m_sDeletedHTML;
}
