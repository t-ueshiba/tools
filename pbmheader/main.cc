/*
 *  $Id: main.cc,v 1.4 2009-09-04 05:50:33 ueshiba Exp $
 */
#include <unistd.h>
#include <exception>
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

