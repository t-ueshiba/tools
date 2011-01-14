/*
 *  $Id: main.cc,v 1.3 2011-01-14 02:35:56 ueshiba Exp $
 */
#include <cstdlib>
#include "MyCmdWindow.h"

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
    
    cerr << "\nRecord image stream from multiple IEEE1394 cameras.\n"
	 << endl;
    cerr << " Usage: " << s << " [options]\n"
	 << endl;
    cerr << " configuration options.\n"
	 << "  -c cameraName:  prefix of camera {conf|calib} file\n"
	 << "                  (default: \""
	 << DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << "  -d configDirs:  list of directories for camera {conf|calib} file\n"
	 << "                  (default: \""
	 << DEFAULT_CONFIG_DIRS
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
    
    v::App	vapp(argc, argv);
    const char*	cameraName = DEFAULT_CAMERA_NAME;
    const char*	configDirs = 0;
    u_int	ncol	   = 2,
		mul	   = 1,
		div	   = 1;
    bool	i1394b	   = false;

  // コマンド行の解析．
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:d:CBn:42HQh")) != EOF; )
	switch (c)
	{
	  case 'c':
	    cameraName = optarg;
	    break;
	  case 'd':
	    configDirs = optarg;
	    break;
	  case 'C':
	    cameraName = 0;
	    break;
	  case 'B':
	    i1394b = true;
	    break;
	  case 'n':
	    ncol = atoi(optarg);
	    break;
	  case '4':
	    mul = 4;
	    div = 1;
	    break;
	  case '2':
	    mul = 2;
	    div = 1;
	    break;
	  case 'H':
	    mul = 1;
	    div = 2;
	    break;
	  case 'Q':
	    mul = 1;
	    div = 4;
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}
    
    try
    {
      // IEEE1394カメラのオープン．
	Ieee1394CameraArray	cameras;
	if (cameraName != 0)
	    cameras.initialize(cameraName, configDirs, i1394b);

	v::MyCmdWindow	myWin(vapp, cameras, ncol, mul, div);
	vapp.run();
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}
