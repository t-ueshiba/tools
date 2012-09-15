/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/getarg.h,v 1.1 2012-09-15 07:42:52 ueshiba Exp $ */

/*
 * Get command line argument
 *
 * Author: Peter J. Nicklin
 */

/*
 * Argument syntax: `-xargument' or `-x argument'
 */
#define GETARG(p) ((p[1] != '\0') ? ++p : (--argc, *++argv))

/*
 * Argument syntax: `-xargument'
 *
 * #define GETARG(p) (++p)
 */
