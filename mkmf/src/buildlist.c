/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/buildlist.c,v 1.1 2012-09-15 07:42:52 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "Mkmf.h"
#include "hash.h"
#include "null.h"
#include "path.h"
#include "slist.h"
#include "suffix.h"
#include "system.h"
#include "yesno.h"
#include "macro.h"

/*
 * buftolist() copies the items from a buffer to a singly-linked list.
 * Returns integer YES if successful, otherwise NO.
 */
buftolist(buf, list)
	char *buf;			/* item buffer */
	SLIST *list;			/* receiving list */
{
	char *gettoken();		/* get next token */
	char *slappend();		/* append file name to list */
	char token[MAXNAMLEN];		/* item buffer */

	while ((buf = gettoken(token, buf)) != NULL)
		{
		if (slappend(token, list) == NULL)
			return(NO);
		}
	return(YES);
}



/*
 * buildliblist() reads library pathnames from the LIBLIST macro
 * definition, and adds them to the library pathname list. Libraries
 * may be specified as `-lx'. Returns integer YES if successful,
 * otherwise NO.
 */
buildliblist()
{
	extern SLIST *LIBLIST;		/* library pathname list */
	extern HASH *MDEFTABLE;		/* macro definition table */
	HASHBLK *htb;			/* hash table block */
	HASHBLK *htlookup();		/* find hash table entry */
	int libbuftolist();		/* load library pathnames into list */
	SLIST *slinit();		/* initialize singly-linked list */
	void htrm();			/* remove hash table entry */

	LIBLIST = NULL;
	if ((htb = htlookup(MLIBLIST, MDEFTABLE)) != NULL)
		{
		LIBLIST = slinit();
		if (libbuftolist(htb->h_def, LIBLIST) == NO)
			return(NO);
		}
	htrm(MLIBLIST, MDEFTABLE);
	return(YES);
}



/*
 * buildsrclist() takes source and header file names from command line
 * macro definitions or the current directory and appends them to source
 * or header file name lists as appropriate. Returns integer YES if
 * if successful, otherwise NO.
 */
buildsrclist()
{
	extern HASH *MDEFTABLE;		/* macro definition table */
	extern SLIST *HEADLIST;		/* header file name list */
	extern SLIST *SRCLIST;		/* source file name list */
	char *slappend();		/* append file name to list */
	HASHBLK *headhtb;		/* HEADERS macro hash table block */
	HASHBLK *htlookup();		/* find hash table entry */
	HASHBLK *srchtb;		/* SOURCE macro hash table block */
	int buftolist();		/* copy items from buffer to list */
	int needheaders = 1;		/* need header file names */
	int needsource = 1;		/* need source file names */
	int read_dir();			/* read dir for source and headers */
	int slsort();			/* sort singly-linked list */
	int strcmp();			/* string comparison */
	SLIST *slinit();		/* initialize singly-linked list */

	HEADLIST = slinit();
	SRCLIST = slinit();

	/* build lists from command line macro definitions */
	if ((headhtb = htlookup(MHEADERS, MDEFTABLE)) != NULL)
		{
		if (buftolist(headhtb->h_def, HEADLIST) == NO)
			return(NO);
		needheaders--;
		}
	if ((srchtb = htlookup(MSOURCE, MDEFTABLE)) != NULL)
		{
		if (buftolist(srchtb->h_def, SRCLIST) == NO)
			return(NO);
		needsource--;
		}
	
	/* read the current directory to get source and header file names */
	if (needheaders || needsource)
		if (read_dir(needheaders, needsource, CURDIR) == NO)
			return(NO);

	if (slsort(strcmp, SRCLIST) == NO)
		return(NO);
	if (slsort(strcmp, HEADLIST) == NO)
		return(NO);
	return(YES);
}



/*
 * expandlibpath() converts a library file specified by `-lx' into a full
 * pathname. /lib and /usr/lib are searched for the library in the form
 * libx.a. An integer YES is returned if the library was found, otherwise NO.
 * A library file which doesn't begin with `-' is left unchanged.
 */
expandlibpath(libpath)
	char *libpath;			/* library pathname buffer */
{
	char *lib;			/* /lib library pathname template */
	char *usrlib;			/* /usr/lib library pathname template */
	int i;				/* library pathname index */

#ifndef ORIGINAL
	return (YES);
#endif !ORIGINAL

	lib = "/lib/libxxxxxxxxxxxxxxxxxxxxxxxxx";
	usrlib = "/usr/lib/libxxxxxxxxxxxxxxxxxxxxxxxxx";

	if (libpath[0] == '-' && libpath[1] == 'l')
		{
		for (i = 0; libpath[i+2] != '\0' && i < 22; i++)
			{
			lib[i+8] = libpath[i+2];
			usrlib[i+12] = libpath[i+2];
			}
		lib[i+8] = usrlib[i+12] = '.';
		lib[i+9] = usrlib[i+13] = 'a';
		lib[i+10] = usrlib[i+14] = '\0';
		if (FILEXIST(lib))
			{
			strcpy(libpath, lib);
			return(YES);
			}
		else if (FILEXIST(usrlib))
			{
			strcpy(libpath, usrlib);
			return(YES);
			}
		else
			return(NO);
		}
	return(YES);
}



/*
 * libbuftolist() appends each library pathname specified in libbuf to
 * the liblist library pathname list.
 */
libbuftolist(libbuf, liblist)
	char *libbuf;			/* library pathname buffer */
	SLIST *liblist;			/* library pathname list */
{
	char *gettoken();		/* get next token */
	char libpath[PATHSIZE];		/* library file pathname */
	char *slappend();		/* append file name to list */
	int expandlibpath();		/* -lx -> full library pathname */

	while ((libbuf = gettoken(libpath, libbuf)) != NULL)
		{
		if (expandlibpath(libpath) == NO)
			{
			warns("can't find library %s", libpath);
			return(NO);
			}
		if (slappend(libpath, liblist) == NULL)
			return(NO);
		}
	return(YES);
}



/*
 * read_dir() reads filenames from the current directory and adds them
 * to the source or header file name lists as appropriate. Returns
 * integer YES if successful, otherwise NO.
 */
read_dir(needheaders, needsource, dirname)
	int needheaders;		/* need header file names */
	int needsource;			/* need source file names */
	char *dirname;
{
    extern int AFLAG;		/* accept src files w/ leading dots? */
    extern SLIST *HEADLIST;	/* header file name list */
    extern SLIST *SRCLIST;	/* source file name list */
    char *rindex();		/* find last occurrence of character */
    char *slappend();		/* append file name to list */
    char *suffix;		/* pointer to file name suffix */
    char *p;			/* beginning of file name component */
    DIR *dirp;			/* directory stream */
    DIR *opendir();		/* open directory stream */
    int lookupsfx();		/* get suffix type */
    int sfxtyp;			/* type of suffix */
    struct direct *dp;		/* directory entry pointer */
    struct direct *readdir();	/* read a directory entry */
    char **gethdrdirs();
    
    if ((dirp = opendir(dirname[0] == '\0' ? CURDIR : dirname)) == NULL)
    {
	warn("can't open directory");
	return(NO);
    }
    for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
    {
	char		filename[MAXPATHLEN + 1];
	int		n = 0;
	struct stat	statbuf;
	if (strcmp(dirname, CURDIR))
	{
	    n = strlen(dirname);
	    strcpy(filename, dirname);
	    filename[n] = '/';
	    ++n;
	}
	strcpy(filename + n, dp->d_name);

	if (stat(filename, &statbuf))
	{
	    perror(filename);
	    continue;
	}

	if (statbuf.st_mode & S_IFDIR)
	{
	    if (!EQUAL(dp->d_name, CURDIR) && !EQUAL(dp->d_name, PARENTDIR))
	    {
		if (EQUAL(dirname, CURDIR))
		{
		    char	**p;
		    for (p = gethdrdirs(); *p != NULL; ++p)
			if (!strcmp(dp->d_name, *p))
			    read_dir(needheaders, needsource, filename);
		}
		else
		{
		    read_dir(needheaders, needsource, filename);
		}
	    }
	}
	else if ((suffix = rindex(filename, '.')) != NULL)
	{
	    if ((p = rindex(filename, '/')) != NULL)
		p++;
	    else
		p = filename;
	    if (*p == '.' && (AFLAG == 0))
		continue;
	    suffix++;
	    sfxtyp = lookupsfx(suffix);
	    if (sfxtyp == SFXSRC)
	    {
		if (needsource)
		    if (slappend(filename, SRCLIST) == NULL)
			return(NO);
	    }
	    else if (sfxtyp == SFXHEAD)
	    {
		if (needheaders)
		    if (slappend(filename, HEADLIST) == NULL)
			return(NO);
	    }
	}
    }
    
    closedir(dirp);
    return(YES);
}
