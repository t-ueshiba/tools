/*
 *  $Id: main.cc,v 1.2 2012-06-19 08:38:24 ueshiba Exp $
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
    cerr << " Usage: " << s << " [options]\n"
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

  // 1�ե졼�ढ����β������Ȥ��Υե����ޥåȤ���ϡ�
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

	camera.snap();				// ����
      //camera >> image;			// �����ΰ�ؤ�ž��
	camera.captureDirectly(image);		// �����ΰ�ؤ�ž��
	if (!image.saveData(cout))		// stdout�ؤν���
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
    
    const char*		deviceName = "/dev/video0";
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "d:h")) != -1; )
	switch (c)
	{
	  case 'd':
	    deviceName = optarg;
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}
    
    try
    {
      // UVC�����Υ����ץ�
	V4L2Camera	camera(deviceName);

	BOOST_FOREACH (V4L2Camera::PixelFormat pixelFormat,
		       camera.availablePixelFormats())
	    camera.put(cerr, pixelFormat);
	BOOST_FOREACH (V4L2Camera::Feature feature,
		       camera.availableFeatures())
	    camera.put(cerr, feature);

      // �����Υ���ץ���Ƚ��ϡ�
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
