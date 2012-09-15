/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/macro.h,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * General macro function definitions
 *
 * Author: Peter J. Nicklin
 */

/*int strcmp();*/				/* string comparison */

#undef CHDIR
#define CHDIR(d) \
	(chdir(d) == 0)			/* change directory */

#undef DOTDIR
#define DOTDIR(dp) \
	(dp->d_name[0] == '.' && dp->d_name[1] == '\0')
					/* current directory? */
#undef DOTDOTDIR
#define DOTDOTDIR(dp) \
	(dp->d_name[0] == '.' && dp->d_name[1] == '.' && dp->d_name[2] == '\0')
					/* parent directory? */
#undef EQUAL
#define EQUAL(s1,s2) \
	(strcmp(s1,s2) == 0)		/* string comparison */

#undef MIN
#define MIN(a,b) \
	(((a) < (b)) ? (a) : (b))	/* minimum of two values */

#undef MAX
#define MAX(a,b) \
	(((a) > (b)) ? (a) : (b))	/* maximum of two values */

#undef WHITESPACE
#define WHITESPACE(c) \
	(c == ' ' || c == '\t')		/* unseen space in a file */
