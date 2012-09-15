/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/slinit.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * slinit() returns a pointer to the head block of a new list, or null
 * pointer if out of memory.
 */
#include <stdlib.h>
#include <stdio.h>
#include "null.h"
#include "slist.h"

extern char *PGN;			/* program name */

SLIST *
slinit()
{
	SLIST *slist;			/* pointer to list head block */

	if ((slist = (SLIST *) malloc(sizeof(SLIST))) == NULL)
		{
		if (*PGN != '\0')
			fprintf(stderr, "%s: ", PGN);
		fprintf(stderr, "out of memory\n");
		return(NULL);
		}
	slist->nk = 0;
	slist->maxkey = 0;
	slist->head = slist->curblk = slist->tail = NULL;
	return(slist);
}
