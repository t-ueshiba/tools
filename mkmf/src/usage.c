/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/usage.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * usage() places a usage error message on the standard error
 * output stream stderr.
 */
#include <stdio.h>
#include "null.h"

extern char *PGN;			/* program name */

usage(m)
	char *m;			/* usage error message */
{
	fprintf(stderr, "usage: ");
	if (PGN != NULL && *PGN != '\0')
		fprintf(stderr, "%s ", PGN);
	fprintf(stderr, "%s\n", m);
}
