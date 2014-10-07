/*
 *  $Id$
 */
#include <signal.h>
#include <cstdlib>
#include <iomanip>
#include "TU/Ieee1394CameraArray.h"
#include "MyCmdWindow.h"

namespace TU
{
namespace v
{
CmdDef*	createMenuCmds(const Ieee1394Camera& camera)		;
CmdDef*	createFeatureCmds(const Ieee1394CameraArray& cameras)	;
}
    
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
    const char*		cameraName = 0;
    const char*		configDirs = 0;
    Ieee1394Node::Speed	speed	   = Ieee1394Node::SPD_400M;
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
	    speed = Ieee1394Node::SPD_800M;
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
      // IEEE1394カメラをオープンする．
	Ieee1394CameraArray	cameras(cameraName, configDirs,
					speed, ncameras);
	if (cameras.size() == 0)
	    return 0;
	
	BOOST_FOREACH (const Ieee1394Camera* camera, cameras)
	    cerr << "uniqId = "
		 << hex << setw(16) << setfill('0')
		 << camera->globalUniqueId() << dec << endl;

      // トリガモードをOFFにする．
	BOOST_FOREACH (Ieee1394Camera* camera, cameras)
	    camera->turnOff(Ieee1394Camera::TRIGGER_MODE);
	
      // signal handlerを登録する．
	signal(SIGINT,  handler);
	signal(SIGPIPE, handler);

	if (gui)
	{
	    v::MyCmdWindow<Ieee1394CameraArray>
		myWin(vapp, cameras,
		      v::createMenuCmds(*cameras[0]),
		      v::createFeatureCmds(cameras));
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
