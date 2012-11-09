/*
 *  $Id$
 */
#include <signal.h>
#include <sys/time.h>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <boost/foreach.hpp>
#include "TU/V4L2++.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;

    cerr << "\nPut image stream from UVC cameras to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << " [-d deviceName]\n"
	 << endl;
}

static bool	active = true;

static void
handler(int sig)
{
    using namespace	std;
    
    cerr << "Signal [" << sig << "] caught!" << endl;
    active = false;
}

template <class T> static void
doJob(V4L2Camera& camera)
{
    using namespace	std;

  // Set signal handler.
    signal(SIGINT,  handler);
    signal(SIGPIPE, handler);

    Image<T>	image(camera.width(), camera.height());

  // 1フレームあたりの画像数とそのフォーマットを出力．
    cout << 'M' << 1 << endl;
    image.saveHeader(cout);

    camera.continuousShot();
    
    int		nframes = 0;
    timeval	start;
    while (active)
    {
	if (nframes == 10)
	{
	    timeval      end;
	    gettimeofday(&end, NULL);
	    double	interval = (end.tv_sec  - start.tv_sec) +
				   (end.tv_usec - start.tv_usec) / 1.0e6;
	    cerr << nframes / interval << " frames/sec" << endl;
	    nframes = 0;
	}
	if (nframes++ == 0)
	    gettimeofday(&start, NULL);

	camera.snap();				// 撮影
	camera >> image;			// 画像領域への転送
      //camera.captureDirectly(image);		// 画像領域への転送
	if (!image.saveData(cout))		// stdoutへの出力
	    active = false;
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
    
    const char*		dev = "/dev/video0";
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "d:h")) != -1; )
	switch (c)
	{
	  case 'd':
	    dev = optarg;
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}
    
    try
    {
      // UVCカメラのオープン．
	V4L2Camera	camera(dev);

	BOOST_FOREACH (V4L2Camera::PixelFormat pixelFormat,
		       camera.availablePixelFormats())
	    camera.put(cerr, pixelFormat);
	BOOST_FOREACH (V4L2Camera::Feature feature,
		       camera.availableFeatures())
	    camera.put(cerr, feature);

      // 画像のキャプチャと出力．
	switch (camera.pixelFormat())
	{
	  case V4L2Camera::GREY:
	    doJob<u_char>(camera);
	    break;
	  case V4L2Camera::YUYV:
	    doJob<YUYV422>(camera);
	    break;
	  case V4L2Camera::UYVY:
	    doJob<YUV422>(camera);
	    break;
	  case V4L2Camera::RGB24:
	    doJob<RGB>(camera);
	    break;
	  case V4L2Camera::BGR24:
	    doJob<BGR>(camera);
	    break;
	  case V4L2Camera::RGB32:
	    doJob<RGBA>(camera);
	    break;
	  case V4L2Camera::BGR32:
	    doJob<ABGR>(camera);
	    break;
	  default:
	    throw runtime_error("Unsupported pixel format!!");
	    break;
	}

	cerr << camera;
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
    }
    
    return 0;
}
