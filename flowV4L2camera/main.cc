/*
 *  $Id$
 */
#include <signal.h>
#include <sys/time.h>
#include <cstdlib>
#include <iomanip>
#include <boost/foreach.hpp>
#include "MyCmdWindow.h"
#include "TU/io.h"

#define DEFAULT_CAMERA_NAME	"V4L2Camera"
#define DEFAULT_CONFIG_DIRS	".:/usr/local/etc/cameras"

namespace TU
{
/************************************************************************
*  global variables and functions					*
************************************************************************/
bool	active = true;

void
countTime(int& nframes, timeval& start)
{
    using namespace	std;

    if (nframes == 10)
    {
	timeval	end;
	gettimeofday(&end, NULL);
	double	interval = (end.tv_sec  - start.tv_sec) +
	    (end.tv_usec - start.tv_usec) / 1.0e6;
	cerr << nframes / interval << " frames/sec" << endl;
	nframes = 0;
    }
    if (nframes++ == 0)
	gettimeofday(&start, NULL);
}

/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;

    cerr << "\nPut image stream from a UVC camera to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << " [options]\n"
	 << endl;
    cerr << "  -c cameraName:    prefix of camera {conf|calib} file\n"
	 << "                      (default: \""
	 << DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << endl;
    cerr << " Other options.\n"
	 << "  -G:               GUI mode. (default: off)\n"
	 << "  -h:               print this.\n"
	 << endl;
}

static void
handler(int sig)
{
    using namespace	std;
    
    cerr << "Signal [" << sig << "] caught!" << endl;
    active = false;
}

static void
run(V4L2Camera& camera)
{
    CaptureAndSave	captureAndSave(camera);
    captureAndSave.saveHeader(std::cout);	// 画像数とそのヘッダを出力

    camera.continuousShot();			// カメラ出力開始

    int		nframes = 0;
    timeval	start;
    while (active)
    {
	countTime(nframes, start);

	if (!captureAndSave(std::cout))
	    active = false;
    }

    camera.stopContinuousShot();		// カメラ出力停止
}

}
/************************************************************************
*  main function							*
************************************************************************/
int
main(int argc, char* argv[])
{
    using namespace	std;
    using namespace	TU;
    
  // Parse command options.
    v::App		vapp(argc, argv);
    const char*		cameraName = DEFAULT_CAMERA_NAME;
    bool		gui	   = false;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "d:Gh")) != -1; )
	switch (c)
	{
	  case 'c':
	    cameraName = optarg;
	    break;
	  case 'G':
	    gui = true;
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}
    
  // Main job.
    try
    {
      // UVCカメラのオープン．
	ifstream	in;
	string		baseName = openFile(in,
					    string(cameraName),
					    string(DEFAULT_CONFIG_DIRS),
					    ".conf");
	string		dev;
	in >> dev;
	V4L2Camera	camera(dev.c_str());
	in >> camera;
	
	BOOST_FOREACH (V4L2Camera::PixelFormat pixelFormat,
		       camera.availablePixelFormats())
	    camera.put(cerr, pixelFormat);
	BOOST_FOREACH (V4L2Camera::Feature feature,
		       camera.availableFeatures())
	    camera.put(cerr, feature);

      // signal handlerを登録する．
	signal(SIGINT,  handler);
	signal(SIGPIPE, handler);

	if (gui)
	{
	    v::MyCmdWindow	myWin(vapp, baseName, dev, camera);
	    vapp.run();
	}
	else
	    run(camera);
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}
