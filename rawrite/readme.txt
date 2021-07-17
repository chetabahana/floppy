RAWRITE for OS/2, DOS and Windows NT
====================================

Overview
--------

RAWRITE is available in two versions; both are provided in this release. 

There is a pure 32-bit version, which runs only on OS/2 version 2.0 and
above; this is the smaller, more functional version.

There is also a 16-bit dual mode version, that will run on any version
of OS/2, and will also run on any version of DOS as long as it is
version 3.2 or later.  Additionally, it will run in a command window on
Windows NT.  Unless otherwise noted, everything herein applies to both
versions. Naturally, it may be useful to rename the 16-bit version to
RAWRITE.EXE before use.

RAWRITE copies a diskette image file to a diskette.  The image file can
be the same size as a diskette, or smaller.  If the image is smaller,
the rest of the last used track is set to zeros, and the rest of the
diskette is left untouched. 

The program works only with 3.5 inch diskettes; it is unlikely that
functionality for 5.25 inch diskettes is required. It does, however,
work with 720KB, 1.44MB and 2.88MB 3.5 inch diskettes.

RAWRITE attempts to discover the size of the output diskette
dynamically.  First, it attempts a hardware media sense [only on the
32-bit version].  This seems to work on MCA systems, but not on some
others (e.g., it doesn't work on a PC Server 325 (8639-PT0).  If the
media sense fails [and always on the 16-bit version], the program uses
the size of the image file as a clue, assuming the smallest diskette
size that is sufficient to accept the whole of the image.  In case this
guess is incorrect, or the media sense [if done] is inaccurate, a flag
can be used to force a particular diskette size. 

Usage of the program differs from some other versions.  The program is
driven entirely from the command line; no prompts are issued.  Partly,
this is because it is in the UNIX 'tradition', and partly because it is
easier to specify options. 

Version summary
---------------

32-bit		- Runs on OS/2 2.x and above only
		- Can detect diskette drive type on some hardware
		- Smaller executable file; more efficient

16-bit		- Runs on any version of OS/2, and DOS, and Windows NT
		- Cannot detect diskette drive types
		- Larger executable file; less efficient

Using the program
-----------------

Synopsis: rawrite [-dhe] imagefile drive
 where:
    -d           forces DD (720K) diskette type
    -h           forces HD (1.44MB) diskette type
    -e           forces ED (2.88MB) diskette type
    imagefile    is the name of the file containing the diskette image
    drive        is the drive to be written to

Examples:  rawrite boot.img a:
           rawrite -e bigboot.img a:

If the program is invoked by name alone, or with the wrong number of
parameters, a short help text is generated. 

Windows NT limitations
----------------------

When using the program on Windows NT, the target diskette must first be
formatted with a file system understandable by Windows NT; this is an NT
imposed limitation.  However, the image written to the diskette may be
of any format at all. 

Package contents
----------------

README.TXT	this file
RAWRITE.EXE	32-bit OS/2 executable
RAWRITEB.EXE	16-bit dual mode executable

Versions
--------
1.0	- Initial version.
1.1	- Improved some error messages.
	- Now checks that output is really a diskette.
1.2	- Fixed media sense broken by version 1.1.
	- Added author contact information to help text.
2.0	- 16-bit dual mode, and compatible 32-bit single mode, versions.

Bob Eager
rde@tavi.co.uk
10th June 2000


