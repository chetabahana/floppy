/*
 * File: v2gbinit.c
 *
 * Virtual device driver to fix DOS 2GB disk space problem
 *
 * Driver initialisation code
 *
 * Bob Eager   February 2003
 *
 */

#include "v2gb.h"

#pragma	alloc_text(CINIT_TEXT, V2GBInit)

#pragma	entry(V2GBInit)


/*
 * Function:	V2GBInit
 *
 * Description:	V2GB initialisation code. This is called during
 *		system initialisation.
 *
 * Entry:	initstring	pointer to configuration strings (not used)
 *
 * Exit:	Success		returns TRUE
 *		Failure		returns FALSE
 *					unable to install hooks
 *					unable to register properties
 *
 * Context:	Init-time
 *
 */

BOOL EXPENTRY V2GBInit(PSZ initstring)
{	BOOL rc;

	/* Register a VDM creation handler entry point */

	rc = VDHInstallUserHook(
			VDM_CREATE,
			(PUSERHOOK) V2GBCreate);
	if(rc == FALSE)	return(FALSE);		/* Failed to install hook */

	/* Register properties */

	/*** Property to enable/disable the driver ***/

	rc = VDHRegisterProperty(
			prop_enable,		/* Property name */
			(PSZ) NULL,		/* Reserved */
			0,			/* Reserved */
			(VPTYPE) VDMP_BOOL,	/* Property type */
			(VPORD) VDMP_ORD_OTHER,	/* Ordinal */
			VDMP_CREATE,		/* Property flags */
			(PVOID) TRUE,		/* Default value */
			NULL,			/* Validation data */
			V2GBValidate);		/* Validation function */
	if(rc == FALSE) return(FALSE);		/* Failed to register property */

	return(TRUE);			/* Initialised OK */
}

/*
 * End of file: v2gbinit.c
 *
 */
