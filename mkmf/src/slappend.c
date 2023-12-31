/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/slappend.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * slappend() saves a null-terminated key string somewhere and inserts a
 * pointer to the key at the tail of list slist. Returns a pointer to
 * the somewhere, or a null pointer if out of memory.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "macro.h"
#include "null.h"
#include "slist.h"

extern char *PGN;			/* program name */

char *
slappend(key, slist)
	char *key;			/* key string */
	SLIST *slist;			/* pointer to list head block */
{
	SLBLK *slbptr;			/* pointer to list block */
	unsigned int klen;		/* key length */

	if (slist == NULL)
		return(NULL);
	klen = strlen(key);
	slist->maxkey = MAX(slist->maxkey, klen);
	if ((slbptr = (SLBLK *) malloc(sizeof(SLBLK))) == NULL ||
	    (slbptr->key = malloc(klen+1)) == NULL)
		{
		if (*PGN != '\0')
			fprintf(stderr, "%s: ", PGN);
		fprintf(stderr, "out of memory\n");
		return(NULL);
		}
	strcpy(slbptr->key, key);
	slbptr->next = NULL;
	if (slist->tail == NULL)
		slist->head = slist->tail = slbptr;
	else
		slist->tail = slist->tail->next = slbptr;
	slist->nk++;
	return(slbptr->key);
}
