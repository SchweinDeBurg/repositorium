

The ZipArchive library				

Copyright (c) 2000 - 2006 Tadeusz Dracz

Version: 2.4.11 				
Date: 31 Jul 2006

This library adds zip compression and decompression functionality to your program, allowing you to create and modify ZIP files in the compatible way with WinZip, PKZIP and other popular archivers.
Its easy and practical interface makes the library suitable for the beginners as well as for the advanced users. 

See what's new in this version.
To be notified about the future library updates, sign up for the \ref pageSubsc.

Platforms supported: 
- Windows 9x\Me\NT\2000\XP (MFC and STL) Microsoft Visual Studio (C++ 6.0, 2003 and 2005), Borland C++ - Linux (STL) , Mac OS X, MinGW

Author: Tadeusz Dracz		
E-Mail: tdracz@artpol-software.com 
Web Site: http://www.artpol-software.com 

This library uses the zlib library by Jean-loup Gailly and Mark Adler to perform inflate and deflate operations.

Features Summary:
- work in a compatible way with PKZIP and WinZip
- create, modify, extract and test zip archives
- create and extract multi-disk archives (on non-removable disks as well)
- add file to the archive from another archive without decompressing the file (copy compressed data) (see CZipArchive::GetFromArchive)
- highly optimized deleting multiple files from the archive
- optimized replacing and renaming files in the archive
- compression from and decompression to memory, create the whole archive in memory, extract the archive from memory 
- password encryption and decryption supported
- possibility to create or extract self-extracting archives
- smart compression, if enabled, prevents the file in the archive to be larger after compression (see CZipArchive::Smartness)
- safe compression with CZipArchive::Flush function
- using callback objects for notifications
	- to provide easy disk change in a multi-disk archives
	- for the progress control when adding, extracting, testing or deleting files or saving archive's central directory
- extracting and deleting using wildcard pattern matching (see CZipArchive::FindMatches)
- UNC and Windows Unicode paths recognized (still, due to the zip format specification, the filnames in the archive are stored using OEM-defined character set)
- wide characters supported
- support for the Java TM Archive (jar) File Format (see CZipArchive::SetIgnoreCRC)
- can be used as a static library or DLL (necessary VC++ projects included)
- possibility to integrate help system with MSDN 
- easy interface
- easy transfer to other system platforms
- speedy
- well documented
- full source code provided
- Visual Studio Projects: version 2005 (available with the library), versions 6.0 and 2003 (available at download page .)
- sample applications provided (for the STL version located in stl/zippie,
the MFC version (multithreaded) is available separately - please visit
 the download page )

If you find a bug, please mail me. Many thanks to the people that already tracked them down and submitted.

 