/*
 * File: v2gbdata.c
 *
 * Virtual device driver to fix DOS 2GB disk space problem
 *
 * Global and instance data definitions
 *
 * Bob Eager   February 2003
 *
 */

#include "v2gb.h"

/* Swappable global data */

#pragma	data_seg(CSWAP_DATA)

UCHAR	prop_enable[] = PROP_ENABLE;

/* Swappable instance data */

#pragma	data_seg(SWAPINSTDATA)

HHOOK	hhookInt21PostProcess;		/* Hook for post INT 21H processing */
UINT	function;			/* INT 21H function being processed */

/*
 * End of file: v2gbdata.c
 *
 */
