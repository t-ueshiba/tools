/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/target.h,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Target definitions
 *
 * Author: Peter J. Nicklin
 */

/*
 * Target struct
 */
typedef struct _target
	{
	int type;			/* prog, lib, or other target type */
	int dest;			/* target destination flag */
	} TARGET;
