/*
 *  $Id: main.cc,v 1.4 2009-09-04 05:50:33 ueshiba Exp $
 */
#include <unistd.h>
#include <exception>
#include "Rectify.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nRectify images input from stdin and output them to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << " [-s scale]\n"
	 << endl;
    cerr << " options.\n"
	 << "  -s scale:  scaling factor of output images\n"
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

    double	scale = 1.0;
    extern char	*optarg;
    extern int	optind;
    for (int c; (c =getopt(argc, argv, "s:h")) != EOF; )
	switch (c)
	{
	  case 's':
	    scale = atof(optarg);
	    break;
	    
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

    try
    {
	Image<u_char>		image[3];
	u_int			nimages = 0;
	for (nimages = 0; nimages < 3; ++nimages)
	    if (!image[nimages].restore(cin))
		break;
	cerr << nimages << " images restored!" << endl;
	
	Image<u_char>		rectifiedImage[3];
	Rectify			rectify;
	switch (nimages)
	{
	  case 1:
	  {
	    CameraWithDistortion calib(image[0].P, image[0].d1, image[0].d2);
	    Warp		 warp;
	    warp.initialize(Matrix33d::I(3), calib.intrinsic(),
			    image[0].width(), image[0].height(),
			    image[0].width(), image[0].height());
	    warp(image[0], rectifiedImage[0]);
	  }
	    break;

	  case 2:
	    rectify.initialize(image[0], image[1], scale);
	    rectify(image[0], image[1], rectifiedImage[0], rectifiedImage[1]);
	    break;

	  case 3:
	    rectify.initialize(image[0], image[1], image[2], scale);
	    rectify(image[0], image[1], image[2],
		    rectifiedImage[0], rectifiedImage[1], rectifiedImage[2]);
	    break;
	}

	if (nimages == 1)
	{
	    rectifiedImage[0].P = image[0].P;
	    rectifiedImage[0].save(cout);
	}
	else
	    for (int i = 0; i < nimages; ++i)
	    {
		rectifiedImage[i].P = rectify.H(i) * image[i].P;
		rectifiedImage[i].save(cout);
	    }
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}

