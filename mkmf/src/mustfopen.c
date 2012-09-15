/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/mustfopen.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * mustfopen() opens a file in the manner of fopen(3). However, if the file
 * cannot be accessed, exit(1) is called.
 */
#include <stdlib.h>
#include <stdio.h>

extern char *PGN;		/* program name */

FILE *
mustfopen(filename,mode)
	char *filename;
	char *mode;
{
	FILE *stream;			/* file stream */

	if ((stream = fopen(filename,mode)) == NULL)
		{
		if (*PGN != '\0')
			fprintf(stderr, "%s: ", PGN);
		fprintf(stderr, "can't open %s\n",filename);
		exit(1);
		}
	return(stream);
}
