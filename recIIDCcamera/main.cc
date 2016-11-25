/*
 *  $Id$
 */
#include <cstdlib>
#include "TU/v/vIIDC++.h"
#include "MyCmdWindow.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nRecord image stream from multiple IIDC cameras.\n"
	 << endl;
    cerr << " Usage: " << s << " [options]\n"
	 << endl;
    cerr << " configuration options.\n"
	 << "  -c cameraName:  prefix of camera {conf|calib} file\n"
	 << "                  (default: \""
	 << IIDCCameraArray::DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << "  -d configDirs:  list of directories for camera {conf|calib} file\n"
	 << "                  (default: \""
	 << IIDCCameraArray::DEFAULT_CONFIG_DIRS
	 << "\")\n"
	 << "  -B:             IEEE1394b mode (default: off)\n"
	 << "  -C:             no cameras used (movie editing only)\n"
	 << endl;
    cerr << " display options.\n"
	 << "  -n ncols:       display <ncols> images per row (default: 2)\n"
	 << "  -4:             display images with quad size\n"
	 << "  -2:             display images with double size\n"
	 << "  -H:             display images with half size\n"
	 << "  -Q:             display images with quarter size\n"
	 << endl;
    cerr << " other options.\n"
	 << "  -h:             print this\n"
	 << endl;
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
    
    v::App		vapp(argc, argv);
    const char*		name  = IIDCCameraArray::DEFAULT_CAMERA_NAME;
    const char*		dirs  = nullptr;
    u_int		ncol  = 2;
    float		zoom  = 1;
    IIDCCamera::Speed	speed = IIDCCamera::SPD_400M;

  // コマンド行の解析．
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:d:Cbn:42HQh")) != -1; )
	switch (c)
	{
	  case 'c':
	    name = optarg;
	    break;
	  case 'd':
	    dirs = optarg;
	    break;
	  case 'C':
	    name = nullptr;
	    break;
	  case 'b':
	    speed = IIDCCamera::SPD_800M;
	    break;
	  case 'n':
	    ncol = atoi(optarg);
	    break;
	  case '4':
	    zoom = 4;
	    break;
	  case '2':
	    zoom = 2;
	    break;
	  case 'H':
	    zoom = 0.5;
	    break;
	  case 'Q':
	    zoom = 0.25;
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}
    
    try
    {
	IIDCCameraArray	cameras;
	if (optind < argc)
	{
	    cameras.resize(argc - optind);
	    for (auto& camera : cameras)
	    {
		camera.initialize(strtoull(argv[optind], 0, 0));
		camera.setSpeed(speed);
	    }
	}
	else if (!name)
	    cameras.restore(name, dirs, speed);

	v::MyCmdWindow<IIDCCameraArray, u_char>	myWin(vapp, cameras, ncol, zoom);
	vapp.run();
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}
