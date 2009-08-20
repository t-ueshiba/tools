/*
 *  $Id: main.cc,v 1.2 2009-08-20 00:55:46 ueshiba Exp $
 */
#include <fstream>
#include <unistd.h>
#include "TU/Image++.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
restoreImages(std::istream& in, Array<GenericImage>& images)
{
    static int	n = 0;

    GenericImage	image;
    if (!image.restore(in))
	images.resize(n);
    else
    {
	++n;
	restoreImages(in, images);
	images[--n] = image;
    }
}

static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nRead pbm images from stdin, then select and output to stdout several of them.\n"
	 << endl;
    cerr << " Usage: " << s << " <image #> <image #> ...\n"
	 << endl;
    cerr << " options.\n"
	 << "  -h:            print this\n"
	 << endl;
}

}

/************************************************************************
*  global functions							*
************************************************************************/
int
main(int argc, char* argv[])
{
    using namespace	std;
    using namespace	TU;

    for (int c; (c =getopt(argc, argv, "h")) != EOF; )
	switch (c)
	{
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

    try
    {
	Array<GenericImage>	images;
	restoreImages(cin, images);

	extern int	optind;
	for (int i = optind; i < argc; ++i)
	{
	    int	n = atoi(argv[i]);
	    if (n < images.dim())
	    {
		cerr << "Extracting " << n << "-th image..." << endl;
		images[n].save(cout);
	    }
	    else
		cerr << "Cannot find " << n << "-th image!" << endl;
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}

