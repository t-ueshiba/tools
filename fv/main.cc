/*
 *  $Id$
 */
#include <cstdlib>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/CanvasPane.h"
#ifdef USE_XVDC
#  include "TU/v/XvDC.h"
#else
#  include "TU/v/ShmDC.h"
#endif
#include "TU/v/Timer.h"
#include "TU/v/FileSelection.h"
#include <fstream>
#include <sstream>

#ifndef EOF
#  define EOF	(-1)
#endif

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
restoreImages(std::istream& in, Array<GenericImage>& images)
{
    static size_t	n = 0;

    GenericImage	image;
    if (!image.restore(in))
	images.resize(n);
    else
    {
	++n;
	restoreImages(in, images);
	images[--n] = image;
    }
}

static void
restoreHeaders(std::istream& in, Array<GenericImage>& images)
{
  // 1フレームあたりの画像数を取得．
    size_t	nviews = 0;
    in >> nviews >> skipl;

  // 画像列を確保し，ヘッダ情報を読み込む．
    images.resize(nviews);
    for (size_t i = 0; i < images.size(); ++i)
	images[i].restoreHeader(in);
}
    
static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nRead a set of multi-view images or image streams from stdin and display it.\n"
	 << endl;
cerr << " Usage: " << s << " [-n #images] [-[QH24]] [-s saturation] < imageStreamFile\n"
	 << endl;
    cerr << " options.\n"
	 << "  -n #images:     #images displayed per row\n"
	 << "  -O:             display with 1/8 size\n"
	 << "  -Q:             display with quater size\n"
	 << "  -H:             display with half size\n"
	 << "  -2:             display with double size\n"
	 << "  -4:             display with quad size\n"
	 << "  -s saturation:  saturation level for displaying\n"
	 << "  -h:             print this\n"
	 << endl;
}

namespace v
{
/************************************************************************
*  static data								*
************************************************************************/
enum		{c_ContinuousShot, c_Saturation, c_Cursor};
static float	range[] = {1, 255, 1};

static MenuDef fileMenu[] =
{
    {"Save...",	M_Save,	false, noSub},
    {"-",	M_Line,	false, noSub},
    {"Quit",	M_Exit,	false, noSub},
    EndOfMenu
};

static CmdDef	Cmds[] =
{
    {C_MenuButton,   M_File,	       0, "File",	   fileMenu, CA_None,
     0, 0, 1, 1, 0},
    {C_ToggleButton, c_ContinuousShot, 1, "Continuous shot", noProp, CA_None,
     1, 0, 1, 1, 0},
    {C_Label,	     c_Cursor,	       0, "         ",	     noProp, CA_None,
     2, 0, 1, 1, 0},
    {C_Slider,	     c_Saturation,   256, "Saturation:",      range, CA_None,
     3, 0, 1, 1, 0},
    EndOfCmds
};

/************************************************************************
*  class MyCanvasPaneBase						*
************************************************************************/
class MyCanvasPaneBase : public CanvasPane
{
  public:
    MyCanvasPaneBase(Window& parentWin,
		     const GenericImage& image, float zoom)		;

    virtual std::istream&	restoreData(std::istream& in)		= 0;
    virtual std::ostream&	saveImage(std::ostream& out)	const	= 0;
};

MyCanvasPaneBase::MyCanvasPaneBase(Window& parentWin,
				   const GenericImage& image, float zoom)
    :CanvasPane(parentWin, size_t(image.width()  * zoom),
			   size_t(image.height() * zoom))
{
}

/************************************************************************
*  class MyCanvasPane<T>						*
************************************************************************/
template <class T>
class MyCanvasPane : public MyCanvasPaneBase
{
  public:
    MyCanvasPane(Window& parentWin, GenericImage& image, float zoom)	;
    
    virtual std::istream&	restoreData(std::istream& in)		;
    virtual std::ostream&	saveImage(std::ostream& out)	const	;
    virtual void		repaintUnderlay()			;
    virtual void		callback(CmdId id, CmdVal val)		;
    
  private:
#ifdef USE_XVDC
    XvDC		_dc;
#else
#  ifdef USE_SHMDC
    ShmDC		_dc;
#  else
    CanvasPaneDC	_dc;
#  endif
#endif
    const ImageFormat&	_format;
    Image<T>		_image;
};

template <class T>
MyCanvasPane<T>::MyCanvasPane(Window& parentWin, GenericImage& image, float zoom)
    :MyCanvasPaneBase(parentWin, image, zoom),
     _dc(*this, image.width(), image.height(), zoom),
     _format(image.format()),
     _image((T*)image.data(), image.width(), image.height())
{
    _image.P  = image.P;
    _image.d1 = image.d1;
    _image.d2 = image.d2;
}

template <class T> std::istream&
MyCanvasPane<T>::restoreData(std::istream& in)
{
    return _image.restoreData(in, _format);
}
        
template <class T> std::ostream&
MyCanvasPane<T>::saveImage(std::ostream& out) const
{
    return _image.save(out);
}
        
template <class T> void
MyCanvasPane<T>::repaintUnderlay()
{
    _dc << _image;
}

template <class T> void
MyCanvasPane<T>::callback(CmdId id, CmdVal val)
{
    switch (id)
    {
      case Id_MouseButton1Press:
      case Id_MouseButton1Drag:
      case Id_MouseButton1Release:
      case Id_MouseMove:
	parent().callback(id, CmdVal(_dc.dev2logU(val.u()), _dc.dev2logV(val.v())));
	return;
    }

    parent().callback(id, val);
}
    
/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
class MyCmdWindow : public CmdWindow
{
  public:
    MyCmdWindow(App& parentApp, const char* name,
		Array<GenericImage>& images, bool movie,
		size_t ncol, float zoom, size_t saturation)		;
    ~MyCmdWindow()							;
    
    virtual void	callback(CmdId id, CmdVal val)			;
    void		tick()						;

  private:
    const bool			_movie;
    CmdPane			_cmd;
    Array<MyCanvasPaneBase*>	_canvases;
    Timer			_timer;
};

MyCmdWindow::MyCmdWindow(App& parentApp, const char* name,
			 Array<GenericImage>& images, bool movie,
			 size_t ncol, float zoom, size_t saturation)
    :CmdWindow(parentApp, name, 0, Colormap::RGBColor, 16, 0, 0),
     _movie(movie),
     _cmd(*this, Cmds),
     _canvases(images.size()),
     _timer(*this, 0)
{
    using namespace	std;
    
    _cmd.place(0, 0, ncol, 1);

    for (size_t i = 0; i < _canvases.size(); ++i)
    {
	switch (images[i].format().type())
	{
	  case ImageFormat::U_CHAR:
	    _canvases[i] = new MyCanvasPane<u_char>(*this, images[i], zoom);
	    break;
	  case ImageFormat::RGB_24:
	    _canvases[i] = new MyCanvasPane<RGB>(*this, images[i], zoom);
	    break;
	  case ImageFormat::SHORT:
	    _canvases[i] = new MyCanvasPane<short>(*this, images[i], zoom);
	    break;
	  case ImageFormat::FLOAT:
	    _canvases[i] = new MyCanvasPane<float>(*this, images[i], zoom);
	    break;
	  case ImageFormat::YUV_444:
	    _canvases[i] = new MyCanvasPane<YUV444>(*this, images[i], zoom);
	    break;
	  case ImageFormat::YUV_422:
	    _canvases[i] = new MyCanvasPane<YUV422>(*this, images[i], zoom);
	    break;
	  case ImageFormat::YUYV_422:
	    _canvases[i] = new MyCanvasPane<YUYV422>(*this, images[i], zoom);
	    break;
	  case ImageFormat::YUV_411:
	    _canvases[i] = new MyCanvasPane<YUV411>(*this, images[i], zoom);
	    break;
	  case ImageFormat::BMP_8:
	    _canvases[i] = new MyCanvasPane<u_char>(*this, images[i], zoom);
	    break;
	  case ImageFormat::BMP_24:
	    _canvases[i] = new MyCanvasPane<BGR>(*this, images[i], zoom);
	    break;
	  case ImageFormat::BMP_32:
	    _canvases[i] = new MyCanvasPane<BGRA>(*this, images[i], zoom);
	    break;
	  default:
	    throw runtime_error("Unknown image type!!");
	    break;
	}
	
	_canvases[i]->place(i % ncol, 1 + i / ncol, 1, 1);
    }
    
    show();

    _cmd.setValue(c_Saturation, int(saturation));
    colormap().setSaturation(saturation);
    colormap().setSaturationF(saturation);

    if (_movie)
    {
	if (_cmd.getValue(c_ContinuousShot))
	    _timer.start(5);
    }
    else
	_cmd.setValue(c_ContinuousShot, false);
}

MyCmdWindow::~MyCmdWindow()
{
    for (size_t i = 0; i < _canvases.size(); ++i)
	delete _canvases[i];
}
 
void
MyCmdWindow::callback(CmdId id, CmdVal val)
{
    using namespace	std;
    
    switch (id)
    {
      case M_Exit:
	app().exit();
	break;

      case M_Save:
      {
	if (_cmd.getValue(c_ContinuousShot))
	    _timer.stop();

	FileSelection	fileSelection(*this);
	ofstream	out;
	if (fileSelection.open(out))
	    for (size_t i = 0; i < _canvases.size(); ++i)
		_canvases[i]->saveImage(out);

	if (_cmd.getValue(c_ContinuousShot))
	    _timer.start(5);
      }
	break;

      case c_Saturation:
	colormap().setSaturation(val);
	colormap().setSaturationF(val.f());
	for (size_t i = 0; i < _canvases.size(); ++i)
	    _canvases[i]->repaintUnderlay();
	break;

      case c_ContinuousShot:
	if (_movie)
	    if (val)
		_timer.start(5);
	    else
		_timer.stop();
	else
	    _cmd.setValue(c_ContinuousShot, false);
	break;

      case Id_MouseMove:
      {
	ostringstream	s;
	s << '(' << val.u() << ',' << val.v() << ')';
	_cmd.setString(c_Cursor, s.str().c_str());
      }
        break;
    }
}

void
MyCmdWindow::tick()
{
    using namespace	std;
    
    for (size_t i = 0; i < _canvases.size(); ++i)
    {
	_canvases[i]->restoreData(cin);
	_canvases[i]->repaintUnderlay();
    }
}

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
    size_t		ncol = 2, saturation = 256;
    float		zoom = 1;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "n:42HQOs:h")) !=EOF; )
	switch (c)
	{
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
	  case 'O':
	    zoom = 0.125;
	    break;
	  case 's':
	    saturation = atoi(optarg);
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

    try
    {
	char	c;
	if (!cin.get(c))
	    throw runtime_error("Failed to read from stdin!!");

      // 画像／ムービーを判定して，画像データまたはヘッダを読み込む．
	Array<GenericImage>	images;
	switch (c)
	{
	  case 'P':	// 多視点画像
	  case 'B':
	    cin.putback(c);
	    restoreImages(cin, images);
	    break;
	  case 'M':	// 多視点ムービー
	    restoreHeaders(cin, images);
	    break;
	  default:
	    throw runtime_error("Neither image nor movie file!!");
	    break;
	}

      // 各視点の画像サイズを表示．
	for (size_t i = 0; i < images.size(); ++i)
	{
	    cerr << i << "-th view: "
		 << images[i].width() << 'x' << images[i].height() << " (";
	    switch (images[i].format().type())
	    {
	      case ImageFormat::U_CHAR:
		cerr << "U_CHAR";
		break;
	      case ImageFormat::RGB_24:
		cerr << "RGB_24";
		break;
	      case ImageFormat::SHORT:
		cerr << "SHORT";
		break;
	      case ImageFormat::FLOAT:
		cerr << "FLOAT";
		break;
	      case ImageFormat::YUV_444:
		cerr << "YUV_444";
		break;
	      case ImageFormat::YUV_422:
		cerr << "YUV_422";
		break;
	      case ImageFormat::YUYV_422:
		cerr << "YUYV_422";
		break;
	      case ImageFormat::YUV_411:
		cerr << "YUV_411";
		break;
	      case ImageFormat::BMP_8:
		cerr << "BMP_8";
		break;
	      case ImageFormat::BMP_24:
		cerr << "BMP_24";
		break;
	      case ImageFormat::BMP_32:
		cerr << "BMP_32";
		break;
	      default:
		cerr << "unknown...";
		break;
	    }
	    cerr << ')' << endl;
	}

      // GUIオブジェクトを作り，イベントループを起動．
	v::MyCmdWindow	myWin(vapp, "Real-time image viewer", images,
			      (c == 'M'), ncol, zoom, saturation);
	vapp.run();
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }
    
    return 0;
}
