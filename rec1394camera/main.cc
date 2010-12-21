/*
 *  $Id: main.cc,v 1.1 2010-12-21 00:13:26 ueshiba Exp $
 */
#include <cstdlib>
#include "MyCmdWindow.h"
#include "TU/Ieee1394CameraArray.h"

#define DEFAULT_CONFIG_DIRS	".:/usr/local/etc/cameras"
#define DEFAULT_CAMERA_NAME	"IEEE1394Camera"

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
	 << "  -d configDirs:  list of directories for camera {conf|calib} file\n"
	 << "                  (default: \""
	 << DEFAULT_CONFIG_DIRS
	 << "\")\n"
	 << "  -c cameraName:  prefix of camera {conf|calib} file\n"
	 << "                  (default: \""
	 << DEFAULT_CAMERA_NAME
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

static std::string
openFile(std::ifstream& in, const std::string& dirs, const std::string& name)
{
    using namespace		std;

    string::const_iterator	p = dirs.begin();
    do
    {
	string::const_iterator	q = find(p, dirs.end(), ':');
	string			fullName = string(p, q) + '/' + name;
	in.open(fullName.c_str());
	if (in)
	    return fullName;
	p = q;
    } while (p++ != dirs.end());

    throw runtime_error("Cannot open input file \"" + name +
			"\" in \"" + dirs + "\"!!");
    return string();
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
    string	configDirs = DEFAULT_CONFIG_DIRS,
		cameraName = DEFAULT_CAMERA_NAME;
    u_int	ncol	   = 2,
		mul	   = 1,
		div	   = 1;
    bool	i1394b	   = false;

  // コマンド行の解析．
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "d:c:CBn:42HQh")) != EOF; )
	switch (c)
	{
	  case 'd':
	    configDirs = optarg;
	    break;
	  case 'c':
	    cameraName = optarg;
	    break;
	  case 'C':
	    cameraName.clear();
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
	Ieee1394CameraArray	cameras;
	string			cameraBase;
	
	if (!cameraName.empty())
	{
	  // IEEE1394カメラのオープン．
	    ifstream	in;
	    cameraBase = openFile(in, configDirs, cameraName + ".conf");
	    cameraBase.erase(cameraBase.rfind(".conf"));
	    cameras.initialize(in, i1394b);
	}

	v::MyCmdWindow	myWin(vapp, cameraBase, cameras, ncol, mul, div);
	vapp.run();
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}
