/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/warn.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * warn() places an error message on the standard error output stream
 * stderr.
 */
#include <stdio.h>
#include "null.h"

extern char *PGN;			/* program name */

warn(m)
	char *m;			/* warning message */
{
	if (PGN != NULL && *PGN != '\0')
		fprintf(stderr, "%s: ", PGN);
	fprintf(stderr, "%s\n", m);
}
