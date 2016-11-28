/*
 *  $Id$
 */
#include <signal.h>
#include <cstdlib>
#include "TU/v/vV4L2++.h"
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
	 << V4L2CameraArray::DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << "  -d configDirs:    list of directories for camera {conf|calib} file\n"
	 << "                      (default: \""
	 << V4L2CameraArray::DEFAULT_CONFIG_DIRS
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
    using namespace	TU;
    
  // Parse command options.
    v::App		vapp(argc, argv);
    const char*		name = nullptr;
    const char*		dirs = nullptr;
    bool		gui  = false;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:d:Gh")) != -1; )
	switch (c)
	{
	  case 'c':
	    name = optarg;
	    break;
	  case 'd':
	    dirs = optarg;
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
	V4L2CameraArray	cameras;
	if (optind < argc)
	{
	    cameras.resize(argc - optind);
	    for (auto& camera : cameras)
		camera.initialize(argv[optind++]);
	}
	else
	    cameras.restore(name, dirs);

	if (cameras.size() == 0)
	    throw std::runtime_error("One or more cameras must be specified!!");

	BOOST_FOREACH (const auto& camera, cameras)
	{
	    BOOST_FOREACH (V4L2Camera::PixelFormat pixelFormat,
			   camera.availablePixelFormats())
		camera.put(std::cerr, pixelFormat);
	    BOOST_FOREACH (V4L2Camera::Feature feature,
			   camera.availableFeatures())
		camera.put(std::cerr, feature);
	    std::cerr << std::endl;
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
    catch (std::exception& err)
    {
	std::cerr << err.what() << std::endl;
	return 1;
    }

    return 0;
}
