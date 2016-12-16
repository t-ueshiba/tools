/*
 *  $Id$
 */
#include <signal.h>
#include <cstdlib>
#include <iomanip>
#include "TU/v/vIIDC++.h"
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
	 << IIDCCameraArray::DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << "  -b:               IEEE1394b mode. (default: off)\n"
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
    const char*		name  = IIDCCameraArray::DEFAULT_CAMERA_NAME;
    IIDCCamera::Speed	speed = IIDCCamera::SPD_400M;
    bool		gui   = false;
    
  // Parse command options.
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:bGh")) != -1; )
	switch (c)
	{
	  case 'c':
	    name = optarg;
	    break;
	  case 'b':
	    speed = IIDCCamera::SPD_800M;
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
	IIDCCameraArray	cameras;
	if (optind < argc)
	{
	    cameras.resize(argc - optind);
	    for (auto& camera : cameras)
	    {
		camera.initialize(strtoull(argv[optind++], 0, 0));
		camera.setSpeed(speed);
	    }
	}
	else
	    cameras.restore(name, speed);
	
	if (cameras.size() == 0)
	    throw std::runtime_error("One or more cameras must be specified!!");

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

	std::cerr << cameras;
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}
