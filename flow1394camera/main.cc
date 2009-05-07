/*
 *  $Id: main.cc,v 1.3 2009-05-07 04:23:55 ueshiba Exp $
 */
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <stdexcept>
#include "TU/Ieee1394++.h"

#define DEFAULT_CONFIG_DIRS	".:/usr/local/etc/cameras"
#define DEFAULT_CAMERA_NAME	"IEEE1394Camera"

namespace TU
{
/************************************************************************
*  class CameraArray							*
************************************************************************/
class CameraArray : public Array<Ieee1394Camera*>
{
  public:
    CameraArray(std::istream& in, bool i1394b, u_int ncammax)		;
    ~CameraArray()							;
};

CameraArray::CameraArray(std::istream& in, bool i1394b, u_int ncammax)
    :Array<Ieee1394Camera*>()
{
    using namespace	std;

    u_int	delay, ncameras;
    in >> delay >> ncameras;		// Read delay value and #cameras.
    if (ncammax != 0 && ncammax < ncameras)
	ncameras = ncammax;
    resize(ncameras);
    
    for (int i = 0; i < dim(); ++i)
    {
	string	s;
	in >> s;			// Read global unique ID.
	u_int64	uniqId = strtoull(s.c_str(), 0, 0);
	(*this)[i] = new Ieee1394Camera(Ieee1394Camera::Monocular,
					i1394b, uniqId, delay);

	in >> *(*this)[i];		// Read camera parameters.
    }
}

CameraArray::~CameraArray()
{
    for (int i = 0; i < dim(); ++i)
	delete (*this)[i];
}
 
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
	 << "  -d configDirs:    list of directories for camera {conf|calib} file\n"
	 << "                      (default: \""
	 << DEFAULT_CONFIG_DIRS
	 << "\")\n"
	 << "  -c cameraName:    prefix of camera {conf|calib} file\n"
	 << "                      (default: \""
	 << DEFAULT_CAMERA_NAME
	 << "\")\n"
	 << "  -B:               IEEE1394b mode. (default: off)\n"
	 << endl;
    cerr << " Other options.\n"
	 << "  -n ncammax:       # of cameras used. (default: all cameras)\n"
	 << "  -h:               print this.\n"
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

static bool	active = true;

static void
handler(int sig)
{
    using namespace	std;
    
    cerr << "Signal [" << sig << "] caught!" << endl;
    active = false;
}

template <class T> ImageBase::Type	pixelType();

template <> inline ImageBase::Type	pixelType<u_char>()
					{
					    return ImageBase::U_CHAR;
					}
template <> inline ImageBase::Type	pixelType<RGBA>()
					{
					    return ImageBase::RGB_24;
					}
template <> inline ImageBase::Type	pixelType<YUV444>()
					{
					    return ImageBase::YUV_444;
					}
template <> inline ImageBase::Type	pixelType<YUV422>()
					{
					    return ImageBase::YUV_422;
					}
template <> inline ImageBase::Type	pixelType<YUV411>()
					{
					    return ImageBase::YUV_411;
					}

template <class T> static void
doJob(const CameraArray& cameras)
{
    using namespace	std;
    
  // Set signal handler.
    signal(SIGINT,  handler);
    signal(SIGPIPE, handler);

  // 1フレームあたりの画像数とそのフォーマットを出力．
    Array<Image<T> >	images(cameras.dim());
    cout << images.dim() << endl;
    for (int i = 0; i < images.dim(); ++i)
    {
	images[i].resize(cameras[i]->height(), cameras[i]->width());
	images[i].saveHeader(cout, pixelType<T>());
    }

  // カメラ出力の開始．
    for (int i = 0; i < cameras.dim(); ++i)
	cameras[i]->continuousShot();

    int		nframes = 0;
    timeval	start;
    while (active)
    {
	if (nframes == 10)
	{
	    timeval      end;
	    gettimeofday(&end, NULL);
	    double	interval = (end.tv_sec  - start.tv_sec) +
				   (end.tv_usec - start.tv_usec) / 1.0e6;
	    cerr << nframes / interval << " frames/sec" << endl;
	    nframes = 0;
	}
	if (nframes++ == 0)
	    gettimeofday(&start, NULL);

	for (int i = 0; i < cameras.dim(); ++i)
	    cameras[i]->snap();				// 撮影
	for (int i = 0; i < cameras.dim(); ++i)
	    *cameras[i] >> images[i];			// 画像領域への転送
	for (int i = 0; i < images.dim(); ++i)
	    if (!images[i].saveData(cout, pixelType<T>()))  // stdoutへの出力
		active = false;
    }

  // カメラ出力の停止．
    for (int i = 0; i < cameras.dim(); ++i)
	cameras[i]->stopContinuousShot();
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
    
    string		configDirs = DEFAULT_CONFIG_DIRS;
    string		cameraName = DEFAULT_CAMERA_NAME;
    bool		i1394b	   = false;
    u_int		ncammax	   = 0;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "d:c:Bn:h")) != EOF; )
	switch (c)
	{
	  case 'd':
	    configDirs = optarg;
	    break;
	  case 'c':
	    cameraName = optarg;
	    break;
	  case 'B':
	    i1394b = true;
	    break;
	  case 'n':
	    ncammax = atoi(optarg);
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}
    
    try
    {
      // IEEE1394カメラのオープン．
	ifstream	in;
	openFile(in, configDirs, cameraName + ".conf");
	CameraArray	cameras(in, i1394b, ncammax);
	if (cameras.dim() == 0)
	    return 0;
	
	for (int i = 0; i < cameras.dim(); ++i)
	    cerr << "camera " << i << ": uniqId = "
		 << hex << setw(16) << setfill('0')
		 << cameras[i]->globalUniqueId() << dec << endl;

      // 画像のキャプチャと出力．
	switch (cameras[0]->pixelFormat())
	{
	  case Ieee1394Camera::MONO_8:
	    doJob<u_char>(cameras);
	    break;
	  case Ieee1394Camera::YUV_411:
	    doJob<YUV411>(cameras);
	    break;
	  case Ieee1394Camera::YUV_422:
	    doJob<YUV422>(cameras);
	    break;
	  case Ieee1394Camera::YUV_444:
	    doJob<YUV444>(cameras);
	    break;
	  case Ieee1394Camera::RGB_24:
	    doJob<RGBA>(cameras);
	    break;
	  default:
	    throw runtime_error("Unsupported pixel format!!");
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
    }

    return 0;
}
