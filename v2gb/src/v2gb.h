/*
 * File: v2gb.h
 *
 * Virtual device driver to fix DOS 2GB disk space problem
 *
 * Common header file
 *
 * Bob Eager   February 2003
 *
 */

/*
 * History:
 *
 *	1.0	Initial version.
 *	1.1	Corrected failure to preserve AH on 1BH and 1CH calls.
 *	1.2	Corrected failure to preserve AH on error exit from
 *		1BH and 1CH calls.
 *	1.3	Fix for crash on later fixpack levels of Warp 4.
 *
 */

/*
 * The purpose of this driver is to limit the values returned from INT 21H
 * calls to determine total and available disk space. This is necessary
 * because the limit on a real FAT file system is 2GB, yet an OS/2 file system
 * may be HPFS (or something else) and may be a lot bigger. This causes
 * overflowed or negative values to be calculated by many DOS programs,
 * causing spurious errors connected with a perceived lack of disk space.
 *
 * The driver modifies the return values from three DOS function calls,
 * noted below. It first limits the size of an allocation unit (cluster)
 * to 32KB, because larger values are known to cause problems (OS/2 constructs
 * pseudo-values in excess of this in an attempt to return a true overall
 * value for a large file system). The number of (supposed) allocation units
 * both total and, if applicable, available) is scaled the other way to
 * retain a 'true' value as a product. However, since the number of allocation
 * units is an unsigned 16 bit number, it is maximised to 65535. In effect,
 * this limits any calculation by a DOS program so thgat it never results
 * in a value in excess of 2GB.
 *
 * The functions handled are:
 *
 * ===>	Call:		Get allocation table information for current drive
 *	Input:		AH	1BH
 *	Returns:	AL	Sectors per allocation unit
 *			CX	Sector size
 *			DX	Number of allocation units
 *
 * ===>	Call:		Get allocation table information for specified drive
 *			AH	1CH
 *			DL	drive (0=current, 1=A, 2=B, ...)
 *	Returns:	AL	Sectors per allocation unit (0xff => error)
 *			CX	Sector size
 *			DX	Number of allocation units
 *
 * ===>	Call:		Get disk free space
 *			AH	36H
 *			DL	drive (0=current, 1=A, 2=B, ...)
 *	Returns:	AX	Sectors per allocation unit (0xffff => error)
 *			BX	Free allocation units
 *			CX	Sector size
 *			DX	Number of allocation units
 *
 */

#define	INCL_NONE
#define	INCL_VDH
#define	INCL_MI
#define	__VACSTDCALL__
#define	__VACSYSCALL__

#include "mvdm.h"

/* Constants */

#define	FALSE		0
#define	TRUE		1

#define	MAXAUSIZE	32768		/* Maximum size of an allocation unit */
#define	MAXAU		65535		/* Maximum number of allocation units */

/* Property names and values */

#define	PROP_ENABLE	"DSK_LIMIT_ENABLE"

/* Swappable global data */

#pragma	data_seg(CSWAP_DATA)

extern	UCHAR		prop_enable[];

/* Swappable instance data */

#pragma	data_seg(SWAPINSTDATA)

extern	HHOOK	hhookInt21PostProcess;	/* Hook for post INT 21H processing */
extern	UINT	function;		/* INT 21H function being processed */

/* Exported functions */

BOOL	EXPENTRY	V2GBCreate(HVDM);
VOID	HOOKENTRY	V2GBInt21PostProcess(PVOID, PCRF);
BOOL	HOOKENTRY	V2GBInt21Proc(PCRF);
ULONG	EXPENTRY	V2GBValidate(ULONG, HVDM, ULONG, PSZ);

/*
 * End of file: v2gb.h
 *
 */
