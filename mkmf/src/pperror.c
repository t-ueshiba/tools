/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/pperror.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * pperror() writes a system error message to standard error output,
 * preceded by the name of the program and message.
 */
#include <stdio.h>

pperror(message)
	char *message;			/* error message */
{
	extern char *PGN;		/* program name */

	fprintf(stderr, "%s: ", PGN);
	perror(message);
}
