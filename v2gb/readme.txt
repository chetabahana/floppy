The V2GB.SYS virtual device driver
==================================

Overview
--------

The purpose of this driver is to provide a solution to the problem
caused by the inability of many DOS programs to handle a partition in
excess of 2GB in size.  This causes spurious reports of insufficient
disk space.

That's all you need to know if you just want to use the driver.  See the
next section for how to install it.  If you want to know more, see the
section on Technical Details.

Installation and use
--------------------

To install the driver, copy the file V2GB.SYS to any convenient
directory.  One possibility is \OS2\MDOS on the boot drive, since that's
where other virtual device drivers live.  On the other hand, a
reinstallation of OS/2 would wipe it out so you may wish to choose
somewhere else.  For now, let's assume that your boot drive is C:, and
that you copied the file to \OS2\MDOS on that drive.

Now edit CONFIG.SYS, and add the following line.  Position isn't
particularly significant; a good place might be with all the other
V*.SYS drivers, or at the end:

    DEVICE=C:\OS2\MDOS\V2GB.SYS

modifying it as appropriate.

Reboot the system.  The fix takes effect in ALL DOS sessions without
further intervention.

Should you wish to disable the effect in a particular DOS session, open
the properties notebook for the program object concerned.  Select the
"Session" tab.  Press the "DOS Properties" button.  Select the "All DOS
settings" radio button, and push the "OK" button.  Scroll down and
select the property named "DSK_LIMIT_ENABLE".  Press the "Off" radio
button and press the "Save" button.  This affects only that particular
program object and associated DOS session, but does not take effect
until the next time the session is opened.

Technical Details
-----------------

The purpose of this driver is to limit the values returned from INT 21H
calls to determine total and available disk space.  This is necessary
because the limit on a real FAT file system is 2GB, yet an OS/2 file
system may be HPFS (or something else) and may be a lot bigger.  This
causes overflowed or negative values to be calculated by many DOS
programs, causing spurious errors connected with a perceived lack of
disk space.

The driver is a virtual device driver.  This means that it is a 32-bit
driver that loads into system memory rather than the virtual DOS
machine, and thus it occupies ZERO bytes inside the VDM.  It is thus
preferable to other solutions such as resident programs and conventional
device drivers that hook interrupt vectors.  It also means that control
of the driver is possible on a per-VDM basis, without modifying DOS
startup files such as AUTOEXEC.BAT.

The driver modifies the return values from three DOS function calls,
noted below.  It first limits the size of an allocation unit (cluster)
to 32KB, because larger values are known to cause problems (OS/2
constructs pseudo-values in excess of this in an attempt to return a
true overall value for a large file system).  The number of (supposed)
allocation units both total and, if applicable, available) is scaled the
other way to retain a 'true' value as a product.  However, since the
number of allocation units is an unsigned 16 bit number, it is maximised
to 65535.  In effect, this limits any calculation by a DOS program so
that it never results in a value in excess of 2GB.

The functions handled are:

===>    Call:           Get allocation table information for current drive
        Input:          AH      1BH
        Returns:        AL      Sectors per allocation unit
                        CX      Sector size
                        DX      Number of allocation units

===>    Call:           Get allocation table information for specified drive
                        AH      1CH
                        DL      drive (0=current, 1=A, 2=B, ...)
        Returns:        AL      Sectors per allocation unit (0xff => error)
                        CX      Sector size
                        DX      Number of allocation units

===>    Call:           Get disk free space
                        AH      36H
                        DL      drive (0=current, 1=A, 2=B, ...)
        Returns:        AX      Sectors per allocation unit (0xffff => error)
                        BX      Free allocation units
                        CX      Sector size
                        DX      Number of allocation units

Licence
-------

This program may be used freely without any payment to anyone.  However,
if redistributed, the program file must be accompanied by this
README.TXT file at all times, preferably by being contained within the
same ZIP file.

Contents of release
-------------------

V2GB.SYS        driver file
README.TXT      this file

History
-------

1.0     Initial version.
1.1     Corrected failure to preserve AH on 1BH and 1CH calls.
1.2     Corrected failure to preserve AH on error exit from
        1BH and 1CH calls.
1.3     Corrected VDM crash on later converged kernels.

Author
------

Bob Eager
rde@tavi.co.uk
20th February 2003


