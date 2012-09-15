/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/slsort.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * slsort() sorts list slist according to comparison function compar().
 * compar() is to be called with two arguments and must return an integer
 * greater than, equal to, or less than 0, depending on the lexicographic
 * relationship between the two arguments. Returns integer YES if
 * successful, otherwise NO if out of memory.
 */
#include <stdlib.h>
#include <stdio.h>
#include "null.h"
#include "slist.h"
#include "yesno.h"

static int comparb();			/* compare 2 list blocks */

extern char *PGN;			/* program name */

static int (*sscmp)();			/* string compare function */

slsort(compar, slist)
	int (*compar)();		/* compare two strings */
	SLIST *slist;			/* pointer to list head block */
{
	char **kp;			/* pointer to key pointer array */
	char **skp;			/* ptr to start of key ptr array */
	SLBLK *curblk;			/* current list block */

	if (slist->nk <= 0)
		return(YES);
	else if ((skp = (char **) malloc((unsigned)slist->nk*sizeof(char *))) == NULL)
		{
		if (*PGN != '\0')
			fprintf(stderr, "%s: ", PGN);
		fprintf(stderr, "out of memory\n");
		return(NO);
		}
	for (kp = skp, curblk = slist->head; curblk != NULL; kp++, curblk = curblk->next)
		*kp = curblk->key;

	sscmp = compar;
	qsort((char *) skp, slist->nk, sizeof(char *), comparb);

	for (kp = skp, curblk = slist->head; curblk != NULL; kp++, curblk = curblk->next)
		curblk->key = *kp;
	
	free((char *) skp);
	return(YES);
}



/*
 * comparb() compares key strings in 2 list blocks. Returns whatever
 * sscmp() returns. sscmp() is a string compare function.
 */
static int
comparb(s1, s2)
	char **s1;			/* string pointer */
	char **s2;			/* string pointer */
{
	return(sscmp(*s1, *s2));
}
