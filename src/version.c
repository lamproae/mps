/* impl.c.version: Version Inspection
 *
 * $HopeName: MMsrc!version.c(MM_epcore_brisling.16) $
 * Copyright (C) 1998 Harlequin Limited.  All rights reserved.
 *
 * PURPOSE
 *
 * .purpose: The purpose of this module is to provide a means by which
 * the version of the MM library being used can be determined.
 *
 * DESIGN
 *
 * .design: See design.mps.version-library, but to let you in on a
 * secret, it works by declaring a string with all the necessary info
 * in.
 */

#include "mpm.h"


/* .release: When making a new release, change the expansion of
 * MPS_RELEASE to be a string of the form "release.dylan.crow.2"
 * or whatever. */
#define MPS_RELEASE "release.epcore.brisling.7+ $HopeName: MMsrc!version.c(MMdevel_configura.1) $ *** DEVELOPMENT ONLY ***"

/* Version String
 *
 * MPSVersion is a declared object comprising the concatenation of
 * various other strings. */
char MPSVersionString[] =
  "@(#)HQNMPS, "
  "product." MPS_PROD_STRING ", " MPS_RELEASE ", platform." MPS_PF_STRING
  ", variety." MPS_VARIETY_STRING ", compiled on " __DATE__ " " __TIME__;

char *MPSVersion(void)
{
  return MPSVersionString;
}
