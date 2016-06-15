/*
 *  $Id$
 */
#include <signal.h>
#include <cstdlib>
#include <iomanip>
#include "TU/v/vIIDC++.h"
#include "TU/IIDCCameraArray.h"
#include "MyCmdWindow.h"

namespace TU
{
/************************************************************************
*  global variables							*
************************************************************************/
bool	active = true;

/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;

    cerr << "\nPut image stream from IIDC cameras to stdout.\n"
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
    
    v::App		vapp(argc, argv);
    const char*		cameraName = nullptr;
    const char*		configDirs = nullptr;
    IIDCCamera::Speed	speed	   = IIDCCamera::SPD_400M;
    int			ncameras   = -1;
    bool		gui	   = false;
    
  // Parse command options.
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:d:Bn:Gh")) != -1; )
	switch (c)
	{
	  case 'c':
	    cameraName = optarg;
	    break;
	  case 'd':
	    configDirs = optarg;
	    break;
	  case 'B':
	    speed = IIDCCamera::SPD_800M;
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
      // IIDCカメラをオープンする．
	IIDCCameraArray	cameras(cameraName, configDirs, speed, ncameras);
	if (cameras.size() == 0)
	    return 0;
	
	for (const auto camera : cameras)
	    cerr << "uniqId = "
		 << hex << setw(16) << setfill('0')
		 << camera->globalUniqueId() << dec << endl;

      // signal handlerを登録する．
	signal(SIGINT,  handler);
	signal(SIGPIPE, handler);

	if (gui)
	{
	    v::MyCmdWindow<IIDCCameraArray>	myWin(vapp, cameras);
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
