/*
 *  $Id$
 */
#include <signal.h>
#include <cstdlib>
#include "TU/v/vV4L2++.h"
#include "TU/V4L2CameraArray.h"
#include "MyCmdWindow.h"
#include <boost/foreach.hpp>

namespace TU
{
/************************************************************************
*  global variables and functions					*
************************************************************************/
bool	active = true;

/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;

    cerr << "\nPut image stream from Video for Linux v.2 cameras to stdout.\n"
	 << endl;
    cerr << " Usage: " << s << " [options]\n"
	 << endl;
    cerr << "  -c cameraName:    prefix of camera {conf|calib} file\n"
	 << "                      (default: \""
	 << TU_V4L2_DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << "  -d configDirs:    list of directories for camera {conf|calib} file\n"
	 << "                      (default: \""
	 << TU_V4L2_DEFAULT_CONFIG_DIRS
	 << "\")"
	 << endl;
    cerr << " Other options.\n"
	 << "  -n ncameras:      # of cameras. (default: use all cameras)\n"
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
    const char*		cameraName = 0;
    const char*		configDirs = 0;
    int			ncameras   = -1;
    bool		gui	   = false;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:d:n:Gh")) != -1; )
	switch (c)
	{
	  case 'c':
	    cameraName = optarg;
	    break;
	  case 'd':
	    configDirs = optarg;
	    break;
	  case 'n':
	    ncameras = atoi(optarg);
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
      // V4L2カメラをオープンする．
	V4L2CameraArray	cameras(cameraName, configDirs, ncameras);

	if (cameras.size() == 0)
	    return 0;

	BOOST_FOREACH (const V4L2Camera* camera, cameras)
	{
	    BOOST_FOREACH (V4L2Camera::PixelFormat pixelFormat,
			   camera->availablePixelFormats())
		camera->put(cerr, pixelFormat);
	    BOOST_FOREACH (V4L2Camera::Feature feature,
			   camera->availableFeatures())
		camera->put(cerr, feature);
	    cerr << endl;
	}

      // signal handlerを登録する．
	signal(SIGINT,  handler);
	signal(SIGPIPE, handler);

	if (gui)
	{
	    v::MyCmdWindow<V4L2CameraArray>	myWin(vapp, cameras);
	    vapp.run();
	}
	else
	    run(cameras);
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}
