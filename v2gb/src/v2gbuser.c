/*
 * File: v2gbuser.c
 *
 * Virtual device driver to fix DOS 2GB disk space problem
 *
 * User event processing
 *
 * Bob Eager   February 2003
 *
 */

#include "v2gb.h"

#pragma	alloc_text(CSWAP_TEXT, V2GBCreate)
#pragma	alloc_text(CSWAP_TEXT, V2GBValidate)


/*
 * Function:	V2GBCreate
 *
 * Description:	VDM creation notification.
 *		See VDHInstallUserHook for complete semantics.
 *
 *		This registered function is called each time a new VDM
 *		is created.
 *
 * Entry:	hvdm		handle of VDM
 *
 * Exit:	Success		returns TRUE
 *		Failure		returns FALSE
 *					unable to get properties
 *					unable to set hooks
 *					unable to allocate hooks
 *
 * Context:	VDM Task-time
 *
 */

BOOL HOOKENTRY V2GBCreate(HVDM hvdm)
{	BOOL rc;

	/* See if the driver is enabled for this VDM */

	rc = VDHQueryProperty(prop_enable);
	if(rc == FALSE) return(TRUE);		/* Don't hook anything */

	/* Hook the INT 21H interrupt to our own handler */

	rc = VDHInstallIntHook(
		hvdm,				/* Reserved */
		0x21,				/* Dos interrupt to hook */
		V2GBInt21Proc,			/* INT 21H handler */
		!VDH_ASM_HOOK | 0x02);		/* Pre-reflection hook, non-ASM */
	if(rc == FALSE) return(FALSE);

	/* Allocate a return hook for post INT 21H processing */

	hhookInt21PostProcess = VDHAllocHook(
		VDH_RETURN_HOOK,		/* Hook type */
		V2GBInt21PostProcess,		/* Hook procedure */
		0L);				/* No reference data */
	if(hhookInt21PostProcess == (HHOOK) NULL) return(FALSE);

	return(TRUE);
}


/*
 * Function:	V2GBValidate
 *
 * Description:	VDM properties validation and setting callback function.
 *		This function has to exist for a property to be registered,
 *		but it is not required to do anything here.
 *
 * Entry:	op		operation to perform (set)
 *		hvdm		target VDM
 *		cb		length of value
 *		pch		pointer to value
 *
 * Exit:	Success		returns 0
 *		Failure		returns nonzero
 *					(never happens here)
 *
 * Context:	OS/2 Task-time
 *
 */

ULONG EXPENTRY V2GBValidate(ULONG op, HVDM hvdm, ULONG cb, PSZ psz)
{    return 0;
}

/*
 * End of file: v2gbuser.c
 *
 */
