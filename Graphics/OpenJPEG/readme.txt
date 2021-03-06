
OPENJPEG LIBRARY and APPLICATIONS
----------------------------------

Details on folders hierarchy:

* libopenjpeg: contains the sources of the openjpeg library
    * jpwl: contains the additional sources if you want to build a JPWL-flavoured library.
* applications: contains all applications that use the openjpeg library
    * common: common files to all applications
    * codec: a basic codec
    * mj2: motion jpeg 2000 executables
    * JavaOpenJPEG: java jni to use openjpeg in a java program
    * jpip: complete client-server architecture for remote browsing of jpeg 2000 images. See corresponding README for more details.
    * OPJViewer: gui for displaying j2k files (based on wxWidget)
* thirdparty: thirdparty libraries used by some applications. These libraries will be built only if there are not found on the system. Note that libopenjpeg itself does not have any dependency.
* doc: doxygen documentation setup file and man pages
* tests: configuration files and utilities for the openjpeg test suite. All test images are located in 'http://openjpeg.googlecode.com/svn/data' folder.
* CMake: cmake related files
* m4: autotools related files

see LICENSE for license and copyright information.
see INSTALL for installation procedures.
see NEWS for user visible changes in successive releases.
see CHANGES for per-revision changes.
