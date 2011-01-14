/*
 *  $Id: main.cc,v 1.10 2011-01-14 02:35:39 ueshiba Exp $
 */
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <stdexcept>
#include "TU/Ieee1394CameraArray.h"

#define DEFAULT_CAMERA_NAME	"IEEE1394Camera"
#define DEFAULT_CONFIG_DIRS	".:/usr/local/etc/cameras"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;

    cerr << "\nPut image stream from IEEE1394 cameras to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << " [options]\n"
	 << endl;
    cerr << " Configuration options.\n"
	 << "  -c cameraName:    prefix of camera {conf|calib} file\n"
	 << "                      (default: \""
	 << DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << "  -d configDirs:    list of directories for camera {conf|calib} file\n"
	 << "                      (default: \""
	 << DEFAULT_CONFIG_DIRS
	 << "\")\n"
	 << "  -B:               IEEE1394b mode. (default: off)\n"
	 << endl;
    cerr << " Other options.\n"
	 << "  -n ncameras:      # of cameras. (default: use all cameras)\n"
	 << "  -h:               print this.\n"
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
doJob(const Ieee1394CameraArray& cameras)
{
    using namespace	std;
    
  // Set signal handler.
    signal(SIGINT,  handler);
    signal(SIGPIPE, handler);

    Array<Image<T> >	images(cameras.dim());

  // キャリブレーションデータを読み込む．
    ifstream	in(cameras.calibFile().c_str());
    if (in)
    {
	for (int i = 0; i < images.dim(); ++i)
	    in >> images[i].P >> images[i].d1 >> images[i].d2;
    }
	
  // 1フレームあたりの画像数とそのフォーマットを出力．
    cout << 'M' << images.dim() << endl;
    for (int i = 0; i < images.dim(); ++i)
    {
	images[i].resize(cameras[i]->height(), cameras[i]->width());
	images[i].saveHeader(cout);
    }

  // カメラ出力の開始．
    for (int i = 0; i < cameras.dim(); ++i)
	cameras[i]->continuousShot();

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

	for (int i = 0; i < cameras.dim(); ++i)
	    cameras[i]->snap();				// 撮影
	for (int i = 0; i < cameras.dim(); ++i)
	    *cameras[i] >> images[i];			// 画像領域への転送
	for (int i = 0; i < images.dim(); ++i)
	    if (!images[i].saveData(cout))		// stdoutへの出力
		active = false;
    }

  // カメラ出力の停止．
    for (int i = 0; i < cameras.dim(); ++i)
	cameras[i]->stopContinuousShot();
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
    
    const char*		cameraName = 0;
    const char*		configDirs = 0;
    bool		i1394b	   = false;
    int			ncameras   = -1;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:d:Bn:h")) != EOF; )
	switch (c)
	{
	  case 'c':
	    cameraName = optarg;
	    break;
	  case 'd':
	    configDirs = optarg;
	    break;
	  case 'B':
	    i1394b = true;
	    break;
	  case 'n':
	    ncameras = atoi(optarg);
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}
    
    try
    {
      // IEEE1394カメラのオープン．
	Ieee1394CameraArray	cameras(cameraName, configDirs,
					i1394b, ncameras);
	if (cameras.dim() == 0)
	    return 0;
	
	for (int i = 0; i < cameras.dim(); ++i)
	    cerr << "camera " << i << ": uniqId = "
		 << hex << setw(16) << setfill('0')
		 << cameras[i]->globalUniqueId() << dec << endl;

      // 画像のキャプチャと出力．
	switch (cameras[0]->pixelFormat())
	{
	  case Ieee1394Camera::MONO_8:
	    doJob<u_char>(cameras);
	    break;
	  case Ieee1394Camera::YUV_411:
	    doJob<YUV411>(cameras);
	    break;
	  case Ieee1394Camera::YUV_422:
	    doJob<YUV422>(cameras);
	    break;
	  case Ieee1394Camera::YUV_444:
	    doJob<YUV444>(cameras);
	    break;
	  case Ieee1394Camera::RGB_24:
	    doJob<RGBA>(cameras);
	    break;
	  default:
	    throw runtime_error("Unsupported pixel format!!");
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
    }

    return 0;
}
