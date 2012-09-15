/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/warns.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * warns() places an error message and a string argument on the
 * standard error output stream stderr.
 */
#include <stdio.h>
#include "null.h"

extern char *PGN;			/* program name */

warns(m, s)
	char *m;			/* warning message */
	char *s;			/* string argument */
{
	if (PGN != NULL && *PGN != '\0')
		fprintf(stderr, "%s: ", PGN);
	fprintf(stderr, m, s);
	fprintf(stderr, "\n");
}
