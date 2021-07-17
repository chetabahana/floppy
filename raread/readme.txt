RAREAD for OS/2, DOS and Windows NT
===================================

Overview
--------

RAREAD is available in two versions; both are provided in this release. 
There is a pure 32-bit version, which runs only on OS/2 version 2.0 and
above; this is the smaller, more functional version.  There is also a
16-bit dual mode version, that will run on any version of OS/2, and will
also run on any version of DOS as long as it is version 3.2 or later. 
Additionally, it will run in a command window on Windows NT.  Unless
otherwise noted, everything herein applies to both versions. Naturally,
it may be useful to rename the 16-bit version to RAREAD.EXE before use.

RAREAD copies a diskette to a diskette image file.  The image file is
not compressed or truncated in any way.

The program works only with 3.5 inch diskettes; it is unlikely that
functionality for 5.25 inch diskettes is required. It does, however,
work with 720KB, 1.44MB and 2.88MB 3.5 inch diskettes.

RAREAD attempts to discover the size of the output diskette dynamically,
by attempting a hardware media sense [only on the 32-bit version].  This
seems to work on MCA systems, but not on some others (e.g., it doesn't
work on a PC Server 325 (8639-PT0).  If the media sense fails [and
always on the 16-bit version], a flag can be used to force a particular
diskette size. 

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

Synopsis: raread [-dhe] drive imagefile
 where:
    -d           forces DD (720K) diskette type
    -h           forces HD (1.44MB) diskette type
    -e           forces ED (2.88MB) diskette type
    drive        is the drive to be read from
    imagefile    is the name of the file to contain the diskette image

Examples:  raread a: boot.img
           raread -e a: bigboot.img

If the program is invoked by name alone, or with the wrong number of
parameters, a short help text is generated. 

Windows NT limitations
----------------------

When used on Windows NT, the program cannot read diskettes that contain
a filing system not understood by Windows NT; this is an NT limitation. 

Package contents
----------------

README.TXT	this file
RAREAD.EXE	32-bit OS/2 executable
RAREADB.EXE	16-bit dual mode executable

Versions
--------
1.0	- Initial version.
2.0	- 16-bit dual mode, and compatible 32-bit single mode, versions.

Bob Eager
rde@tavi.co.uk
10th June 2000


