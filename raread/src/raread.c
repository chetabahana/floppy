/*
 * File: raread.c
 *
 * Creates raw diskette image from a diskette
 *
 * OS/2 version; works with 720KB, 1.44MB and 2.88MB diskettes
 *
 * Can be built as 16 bit bound executable
 *
 * Bob Eager   November 2000
 *
 */

/* Program version information */

#define	VERSION		2
#define	EDIT		1

#define	AUTHOR		"Bob Eager (rde@tavi.co.uk)"

/*
 * History:
 *	1.0	- Initial version.
 *	2.0	- First dual mode capable version.
 *	2.1	- Fixed error with IOCTL in real mode.
 *
 */

#ifdef	DUAL
#define	MODE		"16-bit dual mode"
#define	NOTMODE		"32-bit OS/2-only"
#define	APIRET		USHORT
#else
#define	MODE		"32-bit"
#define	NOTMODE		"16-bit dual mode (DOS and OS/2)"
#endif

/* Includes */

#define	INCL_DOSDEVICES
#define	INCL_DOSERRORS
#define	INCL_DOSFILEMGR
#define	INCL_DOSDEVIOCTL
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Miscellaneous definitions */

#ifdef	DUAL
#define	BLKSIZE		512L		/* Number of bytes in a disk block */
#define	DD_MAX		720L*2L*BLKSIZE	/* Maximum size of 720K image */
#define	HD_MAX		1440L*2L*BLKSIZE/* Maximum size of 1.44MB image */
#else
#define	BLKSIZE		512		/* Number of bytes in a disk block */
#define	DD_MAX		720*2*BLKSIZE	/* Maximum size of 720K image */
#define	HD_MAX		1440*2*BLKSIZE	/* Maximum size of 1.44MB image */
#endif

#define	TY_UNKNOWN	0		/* Diskette type unknown */
#define	TY_DD		1		/* DD diskette specified */
#define	TY_HD		2		/* HD diskette specified */
#define	TY_ED		3		/* ED diskette specified */

/* Forward references */

static	VOID	close_disk(HFILE);
static	VOID	error(PUCHAR, ...);
static	HFILE	open_disk(PUCHAR);
static	BOOL	process_disk(FILE *, HFILE, INT);
static	VOID	usage(VOID);

/* Local storage */

static	PUCHAR	progname;		/* Pointer to program name */

/* Help text */

static	const	PUCHAR helpinfo[] = {
"%s: make image file from 3.5 inch diskette",
"Synopsis: %s [-dhe] drive imagefile",
" where:",
"    -d           forces DD (720K) diskette type",
"    -h           forces HD (1.44MB) diskette type",
"    -e           forces ED (2.88MB) diskette type",
"    drive        is the drive to be read from",
"    imagefile    is the name of the file to contain the diskette image",
" ",
"Examples:  %s a: boot.img",
"           %s -e a: bigboot.img",
#ifndef	DUAL
" ",
"If the diskette size is not specified, an attempt is made to determine",
"the actual media type. If this is not possible, or the decision is wrong,",
"the -d, -e or -h flags can be used to specify the diskette density.",
#endif
""
};


VOID main(INT argc, PUCHAR argv[])
{	FILE *fp;			/* File pointer for image file */
	INT q = 1;			/* First real arg index */
	PUCHAR p;			/* Temporary */
	PUCHAR file;			/* Pointer to image file name */
	PUCHAR drv;			/* Pointer to original drive name */
	UCHAR drive[3];			/* Drive name */
	HFILE dfd;			/* Disk file handle */
	UINT type = TY_UNKNOWN;		/* Diskette type */

	/* Derive program name for use in messages */

	progname = strrchr(argv[0], '\\');
	if(progname != (PUCHAR) NULL)
		progname++;
	else
		progname = argv[0];
	p = strchr(progname, '.');
	if(p != (PUCHAR) NULL) *p = '\0';
	strlwr(progname);

	/* Check and parse arguments */

	if(argc < 3 || argc > 4) {
		usage();
		exit(EXIT_FAILURE);
	}

	if(argv[1][0] == '-') {		/* Flag */
		q++;
		switch(argv[1][1]) {
			case 'D':
			case 'd':
				type = TY_DD;
				break;

			case 'H':
			case 'h':
				type = TY_HD;
				break;

			case 'E':
			case 'e':
				type = TY_ED;
				break;

			default:
				usage();
				exit(EXIT_FAILURE);
		}
	}

	file = argv[q+1];

	/* Open image file */

	fp = fopen(file, "wb");
	if(fp == (FILE *) NULL) {
		error("cannot open file '%s'", file);
		exit(EXIT_FAILURE);
	}

	/* Check and open diskette */

	drv = argv[q];
	if ((strlen(drv) != 2) ||
		!isalpha(drv[0]) ||
		(drv[1] != ':')) {
		usage();
		exit(EXIT_FAILURE);
	}
	strcpy(drive, drv);
	(void) strupr(drive);

	dfd = open_disk(drive);
	if(dfd == (HFILE) NULL)
		exit(EXIT_FAILURE);

	/* Create the image */

	if(process_disk(fp, dfd, type) == FALSE)	/* Read the disk */
		exit(EXIT_FAILURE);

	/* Tidy up and exit */

	close_disk(dfd);		/* Close the drive */

	exit(EXIT_SUCCESS);
}


/*
 * Perform open actions for the disk
 * Returns handle if disk was successfully opened, otherwise NULL.
 *
 */

static HFILE open_disk(PUCHAR drive)
{	APIRET rc;
	UCHAR dbuf[36];			/* DosDevIOCtl data buffer */
	UCHAR parblk[2] = { 0, 0};	/* DosDevIOCtl parameter block */
#ifdef DUAL
	USHORT action;			/* For returned action taken */
	USHORT openflags;		/* For DosOpen */
#else
	ULONG action;			/* For returned action taken */
	ULONG openflags;		/* For DosOpen */
	ULONG plen = sizeof(parblk);	/* Output length for parameters */
	ULONG dlen = sizeof(dbuf);	/* Output length for data */
#endif
	HFILE dfd;			/* Handle for disk */

	openflags = OPEN_ACCESS_READONLY |
		    OPEN_FLAGS_DASD |
#ifndef	DUAL
		    OPEN_FLAGS_FAIL_ON_ERROR |
#endif
		    OPEN_SHARE_DENYREADWRITE;
#ifdef	DUAL
	if(_osmode == OS2_MODE)
		openflags |= OPEN_FLAGS_FAIL_ON_ERROR;
#endif

	/* Open the disk */

	rc = DosOpen(
		drive,			/* drive name */
		&dfd,			/* to return handle */
		&action,		/* to return action taken */
		0L,			/* file size - not used */
		0,			/* file attribute - not used */
		OPEN_ACTION_OPEN_IF_EXISTS |
		OPEN_ACTION_FAIL_IF_NEW,/* open action */
		openflags,		/* open flags */
#ifdef	DUAL
		0L);			/* reserved - must be zero */
#else
		(PEAOP2) 0);		/* extended attributes - not used */
#endif

	if(rc != 0) {
		switch(rc) {
			case ERROR_NOT_READY:
				error("drive %s is not ready", drive);
				break;

			case ERROR_PATH_NOT_FOUND:
				error("%s is not a valid drive name", drive);
				break;

			case ERROR_DISK_CHANGE:
			case ERROR_INVALID_DRIVE:
				error("drive %s does not exist", drive);
				break;

			case ERROR_DRIVE_LOCKED:
				error("drive %s is in use", drive);
				break;

			case ERROR_ACCESS_DENIED:
				error(
					"access to drive %s denied", drive);
				break;

			default:
				error(
					"can't open drive %s, rc = %d",
					drive,
					rc);
		}
		return((HFILE) NULL);
	}

	/* Check that we are dealing with the right kind of media */

	parblk[0] = 0;
	parblk[1] = 0;
#ifdef DUAL
	if(_osmode == OS2_MODE) {
		rc = DosDevIOCtl(
			&dbuf,		/* data block */
			&parblk[0],	/* parameter block */
			DSK_GETDEVICEPARAMS,/* device function - get device details */
			IOCTL_DISK,	/* device category - logical drive */
			dfd);		/* handle from DosOpen */
	} else {
		rc = 0;
	}
#else
	rc = DosDevIOCtl(
		dfd,			/* handle from DosOpen */
		IOCTL_DISK,		/* device category - logical drive */
		DSK_GETDEVICEPARAMS,	/* device function - get device details */
		&parblk[0],		/* parameter block */
		plen,			/* input length of parameter block */
		&plen,			/* output length of parameter block */
		&dbuf,			/* data block */
		dlen,			/* input length of data block */
		&dlen);			/* output length of data block */
#endif
		
	if(rc != 0) {
		error(
			"cannot get device details for %s, rc = %d",
			drive,
			rc);
		(VOID) DosClose(dfd);
		return((HFILE) NULL);
	}

#ifdef	DUAL
	if(_osmode == OS2_MODE) {
#endif
		if(dbuf[33] != 2 && dbuf[33] != 7 && dbuf[33] != 9) {
			error("drive %s is not a valid diskette drive", drive);
			(VOID) DosClose(dfd);
			return((HFILE) NULL);
		}
#ifdef	DUAL
	}
#endif

	/* Lock the disk against access by other processes */

#ifdef	DUAL
	rc = DosDevIOCtl(
		&dbuf,			/* data block */
		&parblk,		/* parameter block */
		DSK_LOCKDRIVE,		/* device function - lock drive */
		IOCTL_DISK,		/* device category - logical drive */
		dfd);			/* handle from DosOpen */
#else
	plen = 1;			/* Re-initialise */
	dlen = 1;

	rc = DosDevIOCtl(
		dfd,			/* handle from DosOpen */
		IOCTL_DISK,		/* device category - logical drive */
		DSK_LOCKDRIVE,		/* device function - lock drive */
		&parblk,		/* parameter block */
		plen,			/* input length of parameter block */
		&plen,			/* output length of parameter block */
		&dbuf,			/* data block */
		dlen,			/* input length of data block */
		&dlen);			/* output length of data block */
#endif

	if(rc != 0) {
		switch(rc) {
			case ERROR_DRIVE_LOCKED:
				error("drive %s is locked", drive);
				break;

			default:
				error(
					"can't lock drive %s, rc = %d",
					drive,
					rc);
		}
		(VOID) DosClose(dfd);
		return((HFILE) NULL);
	}
	return(dfd);
}


/*
 * Perform close actions for the disk
 *
 */

static VOID close_disk(HFILE dfd)
{	APIRET rc;
	UCHAR dbuf;			/* DosDevIOCtl data buffer */
	UCHAR parblk = 0;		/* DosDevIOCtl parameter block */
#ifndef	DUAL
	ULONG plen = sizeof(parblk);	/* Output length for parameters */
	ULONG dlen = sizeof(dbuf);	/* Output length for data */
#endif

	/* Unlock the disk to allow access by other processes */

#ifdef	DUAL
	rc = DosDevIOCtl(
		&dbuf,			/* data block */
		&parblk,		/* parameter block */
		DSK_UNLOCKDRIVE,	/* device function - unlock drive */
		IOCTL_DISK,		/* device category - logical drive */
		dfd);			/* handle from DosOpen */
#else
	rc = DosDevIOCtl(
		dfd,			/* handle from DosOpen */
		IOCTL_DISK,		/* device category - logical drive */
		DSK_UNLOCKDRIVE,	/* device function - unlock drive */
		&parblk,		/* parameter block */
		plen,			/* input length of parameter block */
		&plen,			/* output length of parameter block */
		&dbuf,			/* data block */
		sizeof(dbuf),		/* input length of data block */
		&dlen);			/* output length of data block */
#endif

	if(rc != 0)
		error("can't unlock drive, rc = %d", rc);

	/* Close the disk */

	rc = DosClose(dfd);
	if(rc != 0)
		error("can't close drive, rc = %d", rc);
}


/*
 * Process the disk. This simply means that tracks are copied from
 * successive tracks and heads, to the image file.
 *
 */

static BOOL process_disk(FILE *fp, HFILE dfd, INT type)
{	APIRET rc;
	UINT i;
	size_t n;
	UINT curcyl, curhead;		/* Current position while writing */
	UINT cyls, heads, sectors;	/* Drive geometry */
	UCHAR dpb;			/* DosDevIOCtl data buffer */
	PTRACKLAYOUT parblk;		/* DosDevIOCtl parameter block */ 
#ifdef	DUAL
	UINT plen;			/* Length for parameters */
#else
	UCHAR mspar = 0;		/* DosDevIOCTL parameter block */
	ULONG plen;			/* Length for parameters */
	ULONG dlen;			/* Length for data */
#endif
	PUCHAR buf;			/* Pointer to track buffer */
	BOOL res = TRUE;		/* Final function result */

	cyls = 80;			/* Always this */
	heads = 2;			/* Always this */
	switch(type) {
		case TY_DD:
			sectors = 9;
			break;

		case TY_HD:
			sectors = 18;
			break;

		case TY_ED:
			sectors = 36;
			break;

		case TY_UNKNOWN:
#ifdef	DUAL
			dpb = 0;			/* Cannot sense media */
#else
			plen = sizeof(mspar);
			dlen = sizeof(dpb);
			rc = DosDevIOCtl(
				dfd,			/* handle from DosOpen */
				IOCTL_DISK,		/* device category - logical drive */
				DSK_QUERYMEDIASENSE,	/* device function - get media type */
				&mspar,			/* parameter block */
				plen,			/* input length of parameter block */
				&plen,			/* output length of parameter block */
				&dpb,			/* data block */
				dlen,			/* input length of data block */
				&dlen);			/* output length of data block */
			if(rc != 0) {
				error("cannot get media sense information"
				", rc = %d",
				rc);
				return(FALSE);
			}
#endif
			switch(dpb) {
				default:
				case 0:			/* Media size not known */
					error("cannot determine diskette size;"
					      " use -d, -e or -h flag");
					return(FALSE);

				case 1:
					sectors = 9;
					break;

				case 2:
					sectors = 18;
					break;

				case 3:
					sectors = 36;
					break;
			}
	}
	error(
		"%d cylinders, %d heads, %d sectors per track",
		cyls, heads, sectors);

	/* We now have the file, and the diskette geometry. Create the image
	   from the diskette. */

	/* First allocate the parameter block and track table, as well
	   as the track buffer. */

	plen = sizeof(TRACKLAYOUT)+(sectors-1)*sizeof(USHORT)*2;
	parblk = (PTRACKLAYOUT) malloc(plen);
	if(parblk == (PTRACKLAYOUT) NULL) {
		error("cannot allocate memory for track table");
		return(FALSE);
	}
#ifdef	DUAL
	buf = (PUCHAR) malloc((INT) (sectors*BLKSIZE));
#else
	buf = (PUCHAR) malloc(sectors*BLKSIZE);
#endif
	if(buf == (PUCHAR) NULL) {
		error("cannot allocate memory for track buffer");
		free((PTRACKLAYOUT) parblk);
		return(FALSE);
	}

	/* Now enter the main reading loop. A whole track is done
	   at a time. */

	curcyl = 0;
	curhead = 0;

	for(;;) {
		parblk->bCommand = 1;		/* Read contiguous track */
		parblk->usHead = (USHORT) curhead;
		parblk->usCylinder = (USHORT) curcyl;
		parblk->usFirstSector = 0;
		parblk->cSectors = (USHORT) sectors;

		for(i = 1; i <= (USHORT) sectors; i++) {
			parblk->TrackTable[i-1].usSectorNumber = i;
			parblk->TrackTable[i-1].usSectorSize = BLKSIZE;
		}

		fprintf(
			stdout,
			"%s: cyl: %2d; head: %1d\r",
			progname,
			curcyl,
			curhead);
		fflush(stdout);

#ifdef	DUAL
		rc = DosDevIOCtl(
			(PVOID) buf,
			(PVOID) parblk,
			DSK_READTRACK,
			IOCTL_DISK,
			dfd);
#else
		dlen = sectors*BLKSIZE;
		rc = DosDevIOCtl(
			dfd,
			IOCTL_DISK,
			DSK_READTRACK,
			(PVOID) parblk,
			plen,
			&plen,
			(PVOID) buf,
			dlen,
			&dlen);
#endif

		if(rc != 0) {
			error(
				"\nerror reading cylinder %d, head %d; rc=%d",
				curcyl,
				curhead,
				rc);
			res = FALSE;
			break;
		}
		n = fwrite(buf, (INT) BLKSIZE, sectors, fp);/* Write image track */
		if((n != sectors) && ferror(fp)) {
			error("\nerror writing image file");
			res = FALSE;
			break;
		}

		curhead++;
		if(curhead >= heads) {
			curhead = 0;
			curcyl++;
		}
		if(curcyl >= cyls) break;
	}
	if(res == TRUE) fputc('\n', stdout);

	free((PUCHAR) buf);
	free((PTRACKLAYOUT) parblk);

	return(res);
}


/*
 * Output an error message, possibly with parameters
 *
 */

static VOID error(PUCHAR mes, ...)
{	va_list ap;

	fprintf(stderr, "%s: ", progname);

	va_start(ap, mes);
	vfprintf(stderr, mes, ap);
	va_end(ap);

	fputc('\n', stderr);
}


/*
 * Output program usage information.
 *
 */

static VOID usage(VOID)
{	PUCHAR *p = (PUCHAR *) helpinfo;
	PUCHAR q;

	for(;;) {
		q = *p++;
		if(*q == '\0') break;

		fprintf(stderr, q, progname);
		fputc('\n', stderr);
	}
	fprintf(
		stderr,
		"\nThis is version %d.%d (%s), by %s\n",
		VERSION,
		EDIT,
		MODE,
		AUTHOR);
	fprintf(
		stderr,
		"A %s version is also available.\n",
		NOTMODE);
#ifdef	DUAL
	fprintf(
		stderr,
		"Currently running in %s mode.\n",
		_osmode == DOS_MODE ? "DOS" : "OS/2");
#endif
}

/*
 * End of file: rawrite.c
 *
 */
