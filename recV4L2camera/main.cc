/*
 *  $Id: main.cc 1215 2012-11-09 01:36:56Z ueshiba $
 */
#include <cstdlib>
#include "TU/v/vV4L2++.h"
#include "TU/V4L2CameraArray.h"
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
    
    cerr << "\nRecord image stream from multiple V4L2 cameras.\n"
	 << endl;
    cerr << " Usage: " << s << " [options]\n"
	 << endl;
    cerr << " configuration options.\n"
	 << "  -c cameraName:  prefix of camera {conf|calib} file\n"
	 << "                  (default: \""
	 << TU_V4L2_DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << "  -d configDirs:  list of directories for camera {conf|calib} file\n"
	 << "                  (default: \""
	 << TU_V4L2_DEFAULT_CONFIG_DIRS
	 << "\")\n"
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
    const char*		cameraName = TU_V4L2_DEFAULT_CAMERA_NAME;
    const char*		configDirs = TU_V4L2_DEFAULT_CONFIG_DIRS;
    u_int		ncol	   = 2,
			mul	   = 1,
			div	   = 1;

  // コマンド行の解析．
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:d:Cn:42HQh")) != -1; )
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
	V4L2CameraArray		cameras;
	if (cameraName != 0)
	    cameras.initialize(cameraName, configDirs);

	v::MyCmdWindow<V4L2CameraArray, u_char>
				myWin(vapp, cameras, ncol, mul, div);
	vapp.run();
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}
