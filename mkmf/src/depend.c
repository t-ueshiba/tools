/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/depend.c,v 1.1 2012-09-15 07:42:52 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/param.h>
#include "Mkmf.h"
#include "dlist.h"
#include "hash.h"
#include "macro.h"
#include "null.h"
#include "path.h"
#include "slist.h"
#include "system.h"
#include "yesno.h"
#include "macro.h"

#define USRINCLUDE		"/usr/include/"
#define CURINCLUDE		"./"
#define TOLOWER(c)		(isupper(c) ? tolower(c) : (c))
#define SKIPWHITESPACE(c, f) while ((c = getc(f))==' ' || c=='\t'); ungetc(c,f);

/*
 * Include file state
 */
#define NOTFOUND		0	/* not found anywhere */
#define EXTERNAL		1	/* not found in current directory */
#define INTERNAL		2	/* found in current directory */
#define FROMRULE		3	/* derived from tranformation rule */

/*
 * Include files are stored in hash tables by direct chaining (See
 * p. 134 in `The C Programming Language' by Kernighan and Ritchie).
 * Included include files are also added to a singly-linked list
 * attached to the hash table entry for the include file.
 */
static HASH *C_INCLUDETABLE = NULL;	/* C include file hash table */
static HASH *F_INCLUDETABLE = NULL;	/* Fortran include file hash table */
static HASH *P_INCLUDETABLE = NULL;	/* Pascal include file hash table */
/*
 * Additional include directories are specified via the -I compiler
 * command line option. These directories are stored in singly-linked lists.
 * We also assume that the last look-up directory is "/usr/include".
 */
static SLIST *C_INCDIR;			/* C include directories */
static SLIST *F_INCDIR;			/* Fortran include directories */
static SLIST *P_INCDIR;			/* Pascal include directories */

SLIST *EXTLIST;				/* external header file name list */

extern char *PGN;			/* program name */

/*
 * addincdir() adds directories containing include files to the
 * appropriate singly-linked list. The pathnames to the directories
 * are derived from makefile macro definitions.
 */
void
addincdir()
{
	extern HASH *MDEFTABLE;		/* macro definition table */
	char *slappend();		/* append to singly-linked list */
	HASHBLK *htb;			/* hash table entry block */
	HASHBLK *htlookup();		/* find hash table entry */
	int cleanup();			/* remove temporary makefile and exit */
	void getI();			/* get include directory pathnames */
	extern int dashI;		/* include directories to scan */

	/* C files */
	if ((htb = htlookup(MCFLAGS, MDEFTABLE)) != NULL)
		getI(htb->h_key, htb->h_def, C_INCDIR);
	if (slappend(USRINCLUDE, C_INCDIR) == NULL)
		cleanup();

	/* add any directories specified via the mkmf -I flag. */
	if (dashI) {
		if ((htb = htlookup(MMOREINCDIRS, MDEFTABLE)) != NULL)
			getI(htb->h_key, htb->h_def, C_INCDIR);
		if (slappend(USRINCLUDE, C_INCDIR) == NULL)
			cleanup();
	}
	
	/* Fortran files */
	if ((htb = htlookup(MFFLAGS, MDEFTABLE)) != NULL)
		getI(htb->h_key, htb->h_def, F_INCDIR);
	if (slappend(USRINCLUDE, F_INCDIR) == NULL)
		cleanup();

	/* Pascal files */
	if ((htb = htlookup(MPFLAGS, MDEFTABLE)) != NULL)
		getI(htb->h_key, htb->h_def, P_INCDIR);
	if (slappend(USRINCLUDE, P_INCDIR) == NULL)
		cleanup();
}



/*
 * findinclude() tries to find the pathname of an include file. Returns
 * integer INTERNAL if found in the current directory, EXTERNAL if found
 * somewhere else, FROMRULE if derived from a transformation rule,
 * otherwise NOTFOUND. The pathname is copied into incpath.
 */
#define LOCALDIR(f)    (index(f, _PSC) == NULL)
/* #define INCLUDETYPE(f) ((index(f, _PSC) == NULL) ? INTERNAL : EXTERNAL) */

static int
INCLUDETYPE(char* f)
{
    char	**gethdrdirs();

  /* ファイル名f中の最初のディレクトリ名をtopdirにセット */
    char	topdir[MAXPATHLEN + 1], *p = topdir;
    while (*f != '\0')
    {
	if (*f == _PSC)		/* '/'が見つかれば...	*/
	    break;		/* 直ちに脱出		*/
	*p++ = *f++;
    }
    *p = '\0';

    if (*f == '\0')		/* '/'が見つからなければ...	*/
	return INTERNAL;	/* fは内部ファイル名		*/

  /* topdirが環境変数MKMF_HDRDIRS中のディレクトリ名に一致するか調べる */
    char**	hdrdir;
    for (hdrdir = gethdrdirs(); *hdrdir != NULL; ++hdrdir)
	if (EQUAL(topdir, *hdrdir))	/* どれかに一致すれば...	*/
	    return INTERNAL;		/* fは内部ファイル名		*/
    return EXTERNAL;	/* どれにも一致しなければ，fは外部ファイル名	*/
}


findinclude(incpath, incname, lastname, type)
	char *incpath;			/* pathname receiving buffer */
	register char *incname;		/* include file name */
	char *lastname;			/* file that includes incname */
	int type;			/* file type */
{
	register char *pp;		/* include file path pointer */
	char *index();			/* find occurrence of character */
	char *optpath();		/* optimize pathname */
	char *pathcat();		/* pathname concatenation */
	char *pathhead();		/* remove pathname tail */
	char *strpcpy();		/* string copy and update pointer */
	int lookuprule();		/* look up transformation rules */
	SLBLK *slb;			/* singly-linked list block */
	SLIST *slist;			/* include directory list pointer */

	/*
	 * look in /usr/include only
	 */
	if (*incname == '<')
		{
		pp = strpcpy(incpath, USRINCLUDE);
		for (incname++; *incname != '>'; incname++, pp++)
			*pp = *incname;
		*pp = '\0';
		return(FILEXIST(incpath) ? EXTERNAL : NOTFOUND);
		}

	/*
	 * look for an absolute include file name
	 */
	if (*incname == '/')
		{
		strcpy(incpath, incname);
		return(FILEXIST(incpath) ? EXTERNAL : NOTFOUND);
		}

	/*
	 * look in current include directory to see if the file exists,
	 * or can be generated by a transformation rule in the current
	 * working directory.
	 */
	if (LOCALDIR(lastname))
		{
		if (LOCALDIR(incname))
			{
			if (lookuprule(incname, incpath) == YES)
				return(FROMRULE);
			}
		strcpy(incpath, incname);
		if (FILEXIST(incpath))
			return(INCLUDETYPE(incpath));
		}
	else	{
		strcpy(incpath, lastname);
		pathcat(incpath, pathhead(incpath), incname);
		optpath(incpath);
		if (FILEXIST(incpath))
			return(INCLUDETYPE(incpath));
		}

	/*
	 * search directory list
	 */
	switch (type)
		{
		case INCLUDE_C:
			slist = C_INCDIR;
			break;
		case INCLUDE_FORTRAN:
			slist = F_INCDIR;
			break;
		case INCLUDE_PASCAL:
			slist = P_INCDIR;
			break;
		}
	for (slb = slist->head; slb != NULL; slb = slb->next)
		{
		pp = strpcpy(incpath, slb->key);
		strcpy(pp, incname);
		optpath(incpath);
		if (FILEXIST(incpath))
			return(INCLUDETYPE(incpath));
		}
	return(NOTFOUND);
}



/*
 * getI() appends include directories found via the -I compiler option to
 * a singly linked list.
 */
void
getI(mnam, mdef, slist)
	char *mnam;			/* compiler options macro name */
	char *mdef;			/* compiler options macro definition */
	SLIST *slist;			/* singly-linked list */
{
	char *gettoken();		/* get next token */
	char incpath[PATHSIZE];		/* include directory pathname buffer */
	char *slappend();		/* append to singly-linked list */
	int cleanup();			/* remove temporary makefile and exit */

	while ((mdef = gettoken(incpath, mdef)) != NULL)
		if (incpath[0] == '-' && incpath[1] == 'I')
			if (incpath[2] == '\0')	/* -I dir option */
				{
				if ((mdef = gettoken(incpath, mdef)) != NULL)
					{
					strcat(incpath, PATHSEP);
					if (slappend(incpath, slist) == NULL)
						cleanup();
					}
				else	{
					warn2("missing include directory in %s %s",
					       mnam, "macro definition");
					break;
					}
				}
			else	{		/* -Idir option */
				strcat(incpath+2, PATHSEP);
				if (slappend(incpath+2, slist) == NULL)
					cleanup();
				}
}



/*
 * getinclude() fetchs an include file name from a line of source code.
 * /usr/include '<' and '>' delimiters remain with the filename to
 * distinguish it from an include file in a local directory. Returns
 * NO if syntax error, otherwise YES.
 */
getinclude(incname, curname, lineno, ifp)
	char *curname;			/* current file name */
	char *incname;			/* include file name receiving buffer */
	int lineno;			/* current line number */
	register FILE *ifp;		/* input stream */
{
	register char *ip;		/* include file name buffer pointer */
	register int c;			/* current character */

	SKIPWHITESPACE(c, ifp);
	for (ip = incname; (c = getc(ifp)) != EOF; ip++)
		{
		*ip = c;
		if (c == '\n' || c == '\t' || c == ' ' || c == ';'  || c == ',')
			{
			ungetc(c, ifp);
			break;
			}
		}
	*ip = '\0';

#ifndef ORIGINAL
	if (*incname == '<' && ip[-1] == '>')
		return (NO);
#endif !ORIGINAL
	if ((*incname == '<'  && ip[-1] != '>')  ||
	    (*incname == '\"' && ip[-1] != '\"') ||
	    (*incname == '\'' && ip[-1] != '\'') ||
	    (*incname == '('  && ip[-1] != ')'))
		{
		fprintf(stderr,
			"%s: \"%s\", line %d: bad include syntax for %s\n",
			PGN, curname, lineno, incname);
		return(NO);
		}
	if (*incname == '\"' || *incname == '\'' || *incname == '(')
		{
		ip[-1] = '\0';
		ip = incname + 1;
		while (*incname++ = *ip++)
			continue;
		}
	return(YES);
}



/*
 * inclink() stores a pointer to a hash table block (which contains
 * include file information) somewhere. Returns a pointer to the somewhere,
 * or calls cleanup() if out of memory.
 */
INCBLK *
inclink(htb)
	HASHBLK *htb;			/* hash table block pointer to save */
{
	INCBLK *iblk;			/* pointer to new include chain block */
	int cleanup();			/* remove temporary makefile and exit */

	if ((iblk = (INCBLK *) malloc(sizeof(INCBLK))) == NULL)
		{
		warn("out of memory");
		cleanup();
		}
	iblk->i_loop = NO;
	iblk->i_hblk = htb;
	iblk->i_next = NULL;
	return(iblk);
}



/*
 * instalinclude() adds an include file name to the appropriate include
 * file hash table. Returns a pointer to the hash table block, or calls
 * cleanup() if out of memory.
 */
HASHBLK *
instalinclude(incname, incpath, type)
	char *incname;			/* name of include file */
	char *incpath;			/* path to include file */
	int type;			/* type of source file */
{
	HASH *htinit();			/* initialize hash table */
	HASHBLK *htb = NULL;		/* hash table block */
	HASHBLK *htinstall();		/* install hash table entry */
	int cleanup();			/* remove temporary makefile and exit */
	int ilen;			/* include path length */

	ilen = strlen(incpath);
	switch (type)
		{
		case INCLUDE_C:
			if (C_INCLUDETABLE == NULL)
				{
				C_INCLUDETABLE = htinit(INCLUDETABLESIZE);
				}
			htb = htinstall(incname, incpath, ilen, C_INCLUDETABLE);
			break;
		case INCLUDE_FORTRAN:
			if (F_INCLUDETABLE == NULL)
				{
				F_INCLUDETABLE = htinit(INCLUDETABLESIZE);
				}
			htb = htinstall(incname, incpath, ilen, F_INCLUDETABLE);
			break;
		case INCLUDE_PASCAL:
			if (P_INCLUDETABLE == NULL)
				{
				P_INCLUDETABLE = htinit(INCLUDETABLESIZE);
				}
			htb = htinstall(incname, incpath, ilen, P_INCLUDETABLE);
			break;
		}
	if (htb == NULL)
		cleanup();
	return(htb);
}



/*
 * lookupinclude() returns a pointer to an include hash table block
 * corresponding to incname and type. Returns null if not found.
 */
HASHBLK *
lookupinclude(incname, type)
	char *incname;			/* name of include file */
	int type;			/* type of source file */
{
	HASH *includetable = NULL;	/* include file hash table */
	HASHBLK *htlookup();		/* find hash table entry */

	switch (type)
		{
		case INCLUDE_C:
			includetable = C_INCLUDETABLE;
			break;
		case INCLUDE_FORTRAN:
			includetable = F_INCLUDETABLE;
			break;
		case INCLUDE_PASCAL:
			includetable = P_INCLUDETABLE;
			break;
		}
	return((includetable == NULL) ? NULL : htlookup(incname, includetable));
}



/*
 * mkdepend() creates include file dependencies for object files and installs
 * them to dependency list dlp. Returns a pointer to the dependency list.
 */
DLIST *
mkdepend()
{
	extern SLIST *SRCLIST;		/* source file name list */
	char *rindex();			/* find last occurrence of character */
	char *suffix;			/* suffix pointer */
	DLBLK *dlappend();		/* append dependency list */
	DLIST *dlinit();		/* initialize dependency list */
	DLIST *dlist;			/* dependency list */
	INCBLK *ibp;			/* pointer to chain of include files */
	INCBLK *readC();		/* read C include-style files */
	INCBLK *readF();		/* read Fortran include-style files */
	INCBLK *readP();		/* read Pascal include-style files */
	int cleanup();			/* remove temporary makefile and exit */
	int lookuptypeofinclude();	/* look up the brand of include */
	int slsort();			/* sort singly-linked list */
	int strcmp();			/* string comparison */
	int type;			/* source file type */
	SLBLK *lbp;			/* list block pointer */
	SLIST *slinit();		/* initialize singly-linked list */
	void addincdir();		/* add to list of include directories */
	void rmprinttag();		/* remove "already printed" tags */

	/* initialize include file look-up lists */
	C_INCDIR = slinit();
	F_INCDIR = slinit();
	P_INCDIR = slinit();

	/* add additional include directories */
	addincdir();

	/* initialize external header file name list */
	EXTLIST = slinit();

	/* initialize dependency list */
	dlist = dlinit();
	 
	for (lbp = SRCLIST->head; lbp != NULL; lbp = lbp->next)
		{
		suffix = rindex(lbp->key, '.');
		type = lookuptypeofinclude(++suffix);
		switch (type)
			{
			case INCLUDE_C:
				ibp = readC(lbp->key, 0, lbp->key);
				break;
			case INCLUDE_FORTRAN:
				ibp = readF(lbp->key, 0, lbp->key);
				break;
			case INCLUDE_PASCAL:
				ibp = readP(lbp->key, 0, lbp->key);
				break;
			case INCLUDE_NONE:
				ibp = NULL;
				break;
			}
		if (ibp != NULL)
			{
			if (dlappend(type, lbp, ibp, dlist) == NULL)
				cleanup();
			}
		}
	if (slsort(strcmp, EXTLIST) == NO)
		cleanup();
	return(dlist);
}



/*
 * notfound() prints a "can't find" filename error message.
 */
void
notfound(curname, lineno, incname)
	char *curname;			/* current file name */
	char *incname;			/* name of include file */
	int lineno;			/* current line number */
{
	if (PGN != NULL && *PGN != '\0')
		{
		fprintf(stderr, "%s: ", PGN);
		}
	if (*incname == '<')
		{
		fprintf(stderr, "\"%s\", line %d: can't find %s\n",
			curname, lineno, incname);
		}
	else	{
		fprintf(stderr, "\"%s\", line %d: can't find \"%s\"\n",
			curname, lineno, incname);
		}
}



/*
 * readC() searches C files for included files. Returns a pointer to
 * the chain of include files installed or found in the include file
 * hash table, or null if no include files found.
 */
INCBLK *
readC(lastfile, lastline, curname)
	char *lastfile;			/* parent file name */
	int lastline;			/* current line in parent file */
	char *curname;			/* current file name */
{
	register char *p;		/* include string pointer */
	register FILE *ifp;		/* input file stream */
	register int c;			/* current character */
	char incname[PATHSIZE];		/* name of include file */
	char incpath[PATHSIZE];		/* path to include file */
	char *slappend();		/* append pathname to list */
	HASHBLK *ftb;			/* fromrule hash table entry block */
	HASHBLK *htb = NULL;		/* hash table entry block */
	HASHBLK *instalinclude();	/* install include name in hash table */
	HASHBLK *lookupinclude();	/* look up include in hash table */
	INCBLK *i_head = NULL;		/* head of include chain */
	INCBLK *i_tail = NULL;		/* tail of include chain */
	INCBLK *inclink();		/* link include file hash blocks */
	int cleanup();			/* remove temporary makefile and exit */
	int findinclude();		/* locate include file */
	int getinclude();		/* get include name from input line */
	int inctype;			/* origin of include file */
	int lineno = 1;			/* current line number */
	int type;			/* file type */
	void notfound();		/* print "can't find" filename msg */

	type = INCLUDE_C;

	if ((ifp = fopen(curname, "r")) == NULL)
		{
		if (lastline > 0)
			fprintf(stderr, "%s: \"%s\", line %d: ", PGN,
				lastfile, lastline);
		else
			fprintf(stderr, "%s: ", PGN);
		perror(curname);
		return(NULL);
		}
	while ((c = getc(ifp)) != EOF)
		{
		if (c != '#')
			goto nextline;
		SKIPWHITESPACE(c, ifp);
		for (p = "include"; (c = getc(ifp)) == *p && *p != '\0' ; p++)
			continue;
		if (*p != '\0')
			goto nextline;
		if (getinclude(incname, curname, lineno, ifp) == NO)
			goto nextline;
		if ((htb = lookupinclude(incname, type)) == NULL)
			{
			inctype = findinclude(incpath, incname, curname, type);
			if (inctype == INTERNAL)
				{
				htb = instalinclude(incname, incpath, type);
				}
			else if (inctype == EXTERNAL)
				{
				htb = instalinclude(incname, incpath, type);
				if (slappend(incpath, EXTLIST) == NULL)
					cleanup();
				}
			else if (inctype == FROMRULE)
				{
				htb = instalinclude(incname, incname, type);
				ftb = instalinclude(incpath, incpath, type);
				}
			else	{
				notfound(curname, lineno, incname);
				goto nextline;
				}

			/* look for nested include files */
			htb->h_sub = readC(curname, lineno, incpath);

			if (inctype == FROMRULE) 
				ftb->h_sub = htb->h_sub;
			}
		if (i_tail == NULL)
			{
			i_head = i_tail = inclink(htb);
			}
		else	{
			i_tail = i_tail->i_next = inclink(htb);
			}
nextline:	while (c != '\n' && c != EOF)
			c = getc(ifp);
		lineno++;
		}
	fclose(ifp);
	return(i_head);
}



/*
 * readF() searches Fortran files for included files. Returns a pointer
 * to the chain of include files installed or found in the include file
 * hash table, or null if no include files found.
 */
INCBLK *
readF(lastfile, lastline, curname)
	char *lastfile;			/* parent file name */
	int lastline;			/* current line in parent file */
	char *curname;			/* current file name */
{
	register char *p;		/* include string pointer */
	register FILE *ifp;		/* input file stream */
	register int c;			/* current character */
	char incname[PATHSIZE];		/* name of include file */
	char incpath[PATHSIZE];		/* path to include file */
	char *slappend();		/* append pathname to list */
	HASHBLK *ftb;			/* fromrule hash table entry block */
	HASHBLK *htb = NULL;		/* hash table entry block */
	HASHBLK *instalinclude();	/* install include name in hash table */
	HASHBLK *lookupinclude();	/* look up include in hash table */
	INCBLK *i_head = NULL;		/* head of include chain */
	INCBLK *i_tail = NULL;		/* tail of include chain */
	INCBLK *inclink();		/* link include file hash blocks */
	int cleanup();			/* remove temporary makefile and exit */
	int findinclude();		/* locate include file */
	int getinclude();		/* get include name from input line */
	int inctype;			/* origin of include file */
	int lineno = 1;			/* current line number */
	int type;			/* file type */
	void notfound();		/* print "can't find" filename msg */

	type = INCLUDE_FORTRAN;

	if ((ifp = fopen(curname, "r")) == NULL)
		{
		if (lastline > 0)
			fprintf(stderr, "%s: \"%s\", line %d: ", PGN,
				lastfile, lastline);
		else
			fprintf(stderr, "%s: ", PGN);
		perror(curname);
		return(NULL);
		}
	while ((c = getc(ifp)) != EOF)
		{
		if (c == 'c' || c == 'C' || c == '*' || c == '\n')
			goto nextline;
		while ((c = getc(ifp)) == ' ' || c == '\t')
			continue;
		for (p = "include"; *p == TOLOWER(c) && *p != '\0'; p++)
			c = getc(ifp);
		if (*p != '\0')
			goto nextline;
		if (getinclude(incname, curname, lineno, ifp) == NO)
			goto nextline;
		if ((htb = lookupinclude(incname, type)) == NULL)
			{
			inctype = findinclude(incpath, incname, curname, type);
			if (inctype == INTERNAL)
				{
				htb = instalinclude(incname, incpath, type);
				}
			else if (inctype == EXTERNAL)
				{
				htb = instalinclude(incname, incpath, type);
				if (slappend(incpath, EXTLIST) == NULL)
					cleanup();
				}
			else if (inctype == FROMRULE)
				{
				htb = instalinclude(incname, incname, type);
				ftb = instalinclude(incpath, incpath, type);
				}
			else	{
				notfound(curname, lineno, incname);
				goto nextline;
				}

			/* look for nested include files */
			htb->h_sub = readF(curname, lineno, incpath);

			if (inctype == FROMRULE) 
				ftb->h_sub = htb->h_sub;
			}
		if (i_tail == NULL)
			{
			i_head = i_tail = inclink(htb);
			}
		else	{
			i_tail = i_tail->i_next = inclink(htb);
			}
nextline:	while (c != '\n' && c != EOF)
			c = getc(ifp);
		lineno++;
		}
	fclose(ifp);
	return(i_head);
}



/*
 * readP() searches Pascal files for included files. Returns a pointer
 * to the chain of include files installed or found in the include file
 * hash table, or null if no include files found.
 */
INCBLK *
readP(lastfile, lastline, curname)
	char *lastfile;			/* parent file name */
	int lastline;			/* current line in parent file */
	char *curname;			/* current file name */
{
	register char *p;		/* include string pointer */
	register FILE *ifp;		/* input file stream */
	register int c;			/* current character */
	char incname[PATHSIZE];		/* name of include file */
	char incpath[PATHSIZE];		/* path to include file */
	char *slappend();		/* append pathname to list */
	HASHBLK *ftb;			/* fromrule hash table entry block */
	HASHBLK *htb = NULL;		/* hash table entry block */
	HASHBLK *instalinclude();	/* install include name in hash table */
	HASHBLK *lookupinclude();	/* look up include in hash table */
	INCBLK *i_head = NULL;		/* head of include chain */
	INCBLK *i_tail = NULL;		/* tail of include chain */
	INCBLK *inclink();		/* link include file hash blocks */
	int cleanup();			/* remove temporary makefile and exit */
	int findinclude();		/* locate include file */
	int getinclude();		/* get include name from input line */
	int inctype;			/* origin of include file */
	int lineno = 1;			/* current line number */
	int type;			/* file type */
	void notfound();		/* print "can't find" filename msg */

	type = INCLUDE_PASCAL;

	if ((ifp = fopen(curname, "r")) == NULL)
		{
		if (lastline > 0)
			fprintf(stderr, "%s: \"%s\", line %d: ", PGN,
				lastfile, lastline);
		else
			fprintf(stderr, "%s: ", PGN);
		perror(curname);
		return(NULL);
		}
	while ((c = getc(ifp)) != EOF)
		{
		if (c != '#')
			goto nextline;
		while ((c = getc(ifp)) == ' ' || c == '\t')
			continue;
		for (p = "include"; *p == TOLOWER(c) && *p != '\0'; p++)
			c = getc(ifp);
		if (*p != '\0')
			goto nextline;
		if (getinclude(incname, curname, lineno, ifp) == NO)
			goto nextline;
		if ((htb = lookupinclude(incname, type)) == NULL)
			{
			inctype = findinclude(incpath, incname, curname, type);
			if (inctype == INTERNAL)
				{
				htb = instalinclude(incname, incpath, type);
				}
			else if (inctype == EXTERNAL)
				{
				htb = instalinclude(incname, incpath, type);
				if (slappend(incpath, EXTLIST) == NULL)
					cleanup();
				}
			else if (inctype == FROMRULE)
				{
				htb = instalinclude(incname, incname, type);
				ftb = instalinclude(incpath, incpath, type);
				}
			else	{
				notfound(curname, lineno, incname);
				goto nextline;
				}

			/* look for nested include files */
			htb->h_sub = readP(curname, lineno, incpath);

			if (inctype == FROMRULE) 
				ftb->h_sub = htb->h_sub;
			}
		if (i_tail == NULL)
			{
			i_head = i_tail = inclink(htb);
			}
		else	{
			i_tail = i_tail->i_next = inclink(htb);
			}
nextline:	while (c != '\n' && c != EOF)
			c = getc(ifp);
		lineno++;
		}
	fclose(ifp);
	return(i_head);
}
