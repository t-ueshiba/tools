/*
 *  $Id$
 */
#include <unistd.h>
#ifdef WIN32
#  include <io.h>
#  include <fcntl.h>
#endif
#include "TU/Rectify.h"

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
    
    cerr << "\nRectify images input from stdin and output them to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << " [-s scale]\n"
	 << endl;
    cerr << " options.\n"
	 << "  -s scale:  scaling factor of output images\n"
	 << "  -h:        print this\n"
	 << endl;
}

template <class T> static void
doJob(const GenericImage& header, double scale)
{
    using namespace	std;
    
    typedef Camera<IntrinsicWithDistortion<IntrinsicBase<double> > >
								camera_type;
    
    Image<T>	image[3];
    u_int	nimages = 0;

  // 最初の画像のデータ部分を読み込む．
    image[0].resize(header.height(), header.width());
    image[0].P = header.P;
    image[0].d1 = header.d1;
    image[0].d2 = header.d2;
    if (!image[0].restoreData(cin, header.typeInfo()))
	throw runtime_error("Cannot restore data of the first image!!");

  // 2番目以降の画像を読み込む．
    for (nimages = 1; nimages < 3; ++nimages)
	if (!image[nimages].restore(cin))
	    break;
    cerr << nimages << " images restored!" << endl;
	
    Image<T>	rectifiedImage[3];
    Rectify	rectify;
    switch (nimages)
    {
      case 1:
      {
	  camera_type	calib;
	  calib.setProjection(image[0].P);
	  calib.setDistortion(image[0].d1, image[0].d2);
	  Warp	warp;
	  warp.initialize(Matrix33d::I(3), calib,
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
	for (u_int i = 0; i < nimages; ++i)
	{
	    rectifiedImage[i].P = rectify.H(i) * image[i].P;
	    rectifiedImage[i].save(cout);
	}
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
#ifdef WIN32
	if (_setmode(_fileno(stdin), _O_BINARY) == -1)
	    throw runtime_error("Cannot set stdin to binary mode!!"); 
	if (_setmode(_fileno(stdout), _O_BINARY) == -1)
	    throw runtime_error("Cannot set stdout to binary mode!!"); 
#endif
	GenericImage	header;
	switch (header.restoreHeader(cin).type)
	{
	  case ImageBase::U_CHAR:
	  case ImageBase::SHORT:
	  case ImageBase::FLOAT:
	  case ImageBase::BMP_8:
	    doJob<u_char>(header, scale);
	    break;
	  case ImageBase::RGB_24:
	  case ImageBase::YUV_444:
	  case ImageBase::YUV_422:
	  case ImageBase::YUV_411:
	  case ImageBase::BMP_24:
	  case ImageBase::BMP_32:
	    doJob<RGBA>(header, scale);
	    break;
	  default:
	    throw runtime_error("Unknown image format!!");
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}

