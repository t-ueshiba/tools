/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/strsav.c,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * strsav() saves a string somewhere and returns a pointer to the somewhere.
 * Returns NULL on error.
 */
#include <stdlib.h>
#include <string.h>
#include "null.h"

char *
strsav(s)
	char *s;
{
	char *sptr;			/* somewhere string pointer */

	if ((sptr = malloc((unsigned)(strlen(s)+1))) == NULL)
		return(NULL);
	return(strcpy(sptr, s));
}
