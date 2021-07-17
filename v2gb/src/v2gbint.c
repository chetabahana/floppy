/*
 * File: v2gbint.c
 *
 * Virtual device driver to fix DOS 2GB disk space problem
 *
 * Interrupt processing
 *
 * Bob Eager   February 2003
 *
 */

#include "v2gb.h"

#pragma	alloc_text(CSWAP_TEXT, V2GBInt21Proc)
#pragma	alloc_text(CSWAP_TEXT, V2GBInt21PostProcess)


/*
 * Function:	V2GBInt21Proc
 *
 * Description:	VDM INT 21H processing.
 *		This function is registered with the 8086 Manager at VDM
 *		create-time and is called by the 8086 Manager whenever a
 *		VDM attempts to call DOS via INT 21H.
 *		The basic action is to hand on all calls except the ones
 *		concerned with disk space. Those are converted to the most
 *		recently implemented version before also being handed on,
 *		but only after arming a hook for post-processing the call
 *		to DOS.
 *
 * Entry:	pcrf		pointer to client register frame
 *
 * Exit:	No chaining	returns TRUE
 *		Chain interrupt	returns FALSE
 *
 * Context:	VDM Task-time
 *
 */

BOOL HOOKENTRY V2GBInt21Proc(PCRF pcrf)
{	function = AH(pcrf);		/* DOS function requested */

	/* Filter out the disk space calls.
	   Convert them all to the 0x36 function because this gives
	   a 16 bit result for the sectors per allocation unit.
	   Then arm the return hook in readiness for post-processing. */

	if((function == 0x1b) ||
	   (function == 0x1c) ||
	   (function == 0x36)) {
		AH(pcrf) = 0x36;	/* Convert to best call */
		(VOID) VDHArmReturnHook(
				hhookInt21PostProcess,
				VDHARH_NORMAL_IRET);
	}

	return(FALSE);			/* Allow all to be handled normally */
}


/*
 * Function:	V2GBInt21PostProcess
 *
 * Description:	VDM INT 21H postprocessing.
 *		This hook function is registered with the 8086 Manager at VDM
 *		create-time and is called by the 8086 Manager on the next
 *		return after the hook has been armed. The hook is only armed
 *		when one of the disk space functions is being called.
 *		Essentially, the output values from the DOS call are modified
 *		according to the rules shown in the comments below.
 *
 * Entry:	p		pointer to reference data (none)
 *		pcrf		pointer to client register frame
 *
 * Exit:	No return values
 *
 * Context:	VDM Task-time
 *
 */

VOID HOOKENTRY V2GBInt21PostProcess(PVOID p, PCRF pcrf)
{
#if 0
	ULONG spau = AX(pcrf) & 0xffff;		/* Sectors/allocation unit */
	ULONG fau = BX(pcrf) & 0xffff;		/* Free allocation units */
	ULONG sectsize = CX(pcrf) & 0xffff;	/* Sector size (bytes) */
	ULONG nau = DX(pcrf) & 0xffff;		/* Number of allocation units */
	ULONG bpau;				/* Bytes per allocation unit */
#ifdef	DEBUG
	UINT factor = 1;			/* Scaling factor */
	UINT limflags = 0x0000;			/* AU limit flags */
#endif

	/* Check for an error (e.g. invalid drive) and if there was one,
	   return without doing any more except cleanup. */

	if((function != 0x36) && ((spau & 0xff) == 0xff))
		spau |= 0xff00;		/* Convert error to 0x36 form */

	if(spau == 0xffff) {
		/* Restore function in AH, except for function 0x36 where
		   it is defined to be overwritten. */

		if(function != 0x36) AH(pcrf) = function;

		return;
	}

	/* Calculate the number of bytes per allocation unit. If it is
	   less than MAXAUSIZE, then there is no problem, because even
	   with the maximum number of allocation units we cannot exceed
	   2 gigabytes. */

	bpau = spau*sectsize;
	if(bpau <= MAXAUSIZE) return;	/* No problem */

	/* The number of bytes per allocation unit is too large.
	   Scale down the size of the (supposed) allocation unit
	   so that it is no larger than MAXAUSIZE, at the same time
	   scaling up the number of allocation units by the same factor.
	   While we are at it, we scale both the total number of
	   allocation units, and the number of free allocation units,
	   even if both may not be needed later. */

	while(bpau > MAXAUSIZE) {
		bpau /= 2;		/* Reduce allocation unit size */
		spau /= 2;		/* Equivalent reduction at sector level */
		nau *= 2;		/* Increase total number of AUs */
		fau *= 2;		/* Increase number of free AUs */
#ifdef	DEBUG
		factor *= 2;
		SI(pcrf) = factor;	/* Put scale factor in SI for debugging */
#endif
	}

	/* The number of allocation units must not exceed MAXAU;
	   limit it to that value if necessary. Do the same for the
	   number of free allocation units. */

#ifdef	DEBUG
	if(nau > MAXAU) limflags |= 0x0001;
	if(fau > MAXAU) limflags |= 0x0100;
	DI(pcrf) = limflags;		/* Put flags in DI for debugging */
#endif
	if(nau > MAXAU) nau = MAXAU;
	if(fau > MAXAU) fau = MAXAU;

	/* We are now finished; update register contents and return. */

	AX(pcrf) = spau;		/* Possibly updated sectors per AU */
	DX(pcrf) = nau;			/* Possibly updated number of AUs */

	if(function == 0x36)
		BX(pcrf) = fau;		/* Possibly updated number of free AUs */

	/* Restore function in AH, except for function 0x36 where
	   it is defined to be overwritten. */

	if(function != 0x36) AH(pcrf) = function;
#endif
	return;
}

/*
 * End of file: v2gbint.c
 *
 */
