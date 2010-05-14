/*
 *  $Id: main.cc,v 1.6 2010-05-14 02:23:09 ueshiba Exp $
 */
#include <unistd.h>
#ifdef WIN32
#  include <io.h>
#  include <fcntl.h>
#endif
#include "TU/Image++.h"
#include "TU/Camera.h"

#ifndef EOF
#  define EOF	(-1)
#endif

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nRead images from stdin and write headers to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << "\n"
	 << endl;
    cerr << " options.\n"
	 << "  -m:        write in a matrix form\n"
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

    bool	matrixForm = false;
    extern char	*optarg;
    extern int	optind;
    for (int c; (c =getopt(argc, argv, "mh")) != EOF; )
	switch (c)
	{
	  case 'm':
	    matrixForm = true;
	    break;
	    
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

    try
    {
#ifdef WIN32
	if (_setmode(_fileno(stdin), _O_BINARY) == -1)
	    throw runtime_error("Cannot set stdin to binary mode!!"); 
	if (_setmode(_fileno(stdout), _O_BINARY) == -1)
	    throw runtime_error("Cannot set stdout to binary mode!!"); 
#endif
	int	n = 0;
	for (GenericImage image; image.restore(cin); )
	{
	    CameraWithDistortion	calib(image.P, image.d1, image.d2);
	    cerr << "--- " << n++ << "-th camera ---" << endl;
	    if (matrixForm)
	    {
		cerr << "[Position: t]" << endl;
		cout << calib.t() << endl;
		cerr << "[Orientation: Rt]" << endl;
		cout << calib.Rt();
		cerr << "[Intrinsic: K]" << endl;
		cout << calib.K();
		cerr << "[Distortion: d1, d2]" << endl;
		cout << calib.d1() << ' ' << calib.d2() << '\n' << endl;
	    }
	    else
	    {
		cout << calib;
	    }
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}

