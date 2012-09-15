/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/getcwp.c,v 1.1 2012-09-15 07:42:52 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * getcwp() returns the pathname of the current working project. If the
 * PROJECT environment variable is undefined or a null string, null is
 * returned.
 */
#include "null.h"

char *
getcwp()
{
	extern char *_PROJECT;		/* project root directory pathname */
	void getproject();		/* get PROJECT environment variable */

	if (_PROJECT == NULL)
		getproject();
	return(_PROJECT);
}
