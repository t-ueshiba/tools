/* $Header: /home/ueshiba/cvsroot/tools/mkmf/src/gethdrdirs.c,v 1.1 2012-09-15 07:42:52 ueshiba Exp $ */

/*
 * Author: Peter J. Nicklin
 */

/*
 * gethdrdirs() returns the list of internal header directory pathnames
 * specified by the MKMF_HDRDIRS environment variable. If the MKMF_HDRDIRS
 * variable is undefined or a null string, null is returned.
 */
#include "null.h"
#include <stdio.h>
#include <stdlib.h>

char**
gethdrdirs()
{
    static char**	_HDRDIRS = NULL;
    static char*	nulllist = NULL;
    char*		hv;		/* ptr to start of HDRDIRS variable */
    char*		getenv();	/* get environment variable */
    char*		strsav();	/* save a string somewhere */
    char*		index();	/* find first occurrence of character */

    if (_HDRDIRS != NULL)
	return _HDRDIRS;
    
    if ((hv = getenv("MKMF_HDRDIRS")) != NULL && *hv != '\0')
    {
	char*	p;
	int	n, i;

      /* 環境変数からヘッダディレクトリのリストを取得 */
	char*	hdrdirs = strsav(hv);
	if (hdrdirs == NULL)
	{
	    warn("out of memory");
	    return NULL;
	}

      /* リスト中の':'を終端文字に置き換え，ディレクトリ数を求める */
	n = 1;
	for (p = hdrdirs; (p = index(p, ':')) != NULL; ++p)
	{
	    *p = '\0';
	    ++n;
	}

      /* ディレクトリ名を納める配列を確保 */
	_HDRDIRS = (char**)malloc((n + 1)*sizeof(char*));
	if (_HDRDIRS == NULL)
	{
	    warn("out of memory");
	    return NULL;
	}

      /* 確保した配列にディレクトリ名を納める */
	p = hdrdirs;
	for (i = 0; i < n; ++i)
	{
	    _HDRDIRS[i] = p;
	    p = index(p, '\0') + 1;
	}
	_HDRDIRS[n] = NULL;
    }
    else
	_HDRDIRS = &nulllist;
    
    return _HDRDIRS;
}
