/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/rule.h,v 1.1 2012-09-15 07:42:53 ueshiba Exp $ */

/*
 * Rule definitions
 *
 * Author: Peter J. Nicklin
 */

/*
 * Rule table block struct
 */
typedef struct _ruleblk
	{
	char *r_rule;			/* pointer to rule string */
	struct _ruleblk *r_next;	/* ptr to next rule list block */
	} RULEBLK;
