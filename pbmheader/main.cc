/*
 *  $Id: main.cc,v 1.2 2008-09-08 08:09:38 ueshiba Exp $
 */
#include <fstream>
#include <exception>
#include <unistd.h>
#include <vector>
#include "TU/Image++.h"
#include "TU/Camera.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nRead images from stdin and show write headers to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << "\n"
	 << endl;
    cerr << " options.\n"
	 << "  -h:        print this\n"
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

    extern char	*optarg;
    extern int	optind;
    for (int c; (c =getopt(argc, argv, "h")) != EOF; )
	switch (c)
	{
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

    try
    {
	int	n = 0;
	for (GenericImage image; image.restore(cin); )
	{
	    CameraWithDistortion	calib(image.P, image.d1, image.d2);
	    cerr << "--- " << n++ << "-th camera ---" << endl;
	    cout << calib;
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}

