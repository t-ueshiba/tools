/*
 *  $Id$
 */
#include <cstdlib>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/XvDC.h"
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
    static u_int	n = 0;

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
    u_int	nviews = 0;
    in >> nviews >> skipl;

  // 画像列を確保し，ヘッダ情報を読み込む．
    images.resize(nviews);
    for (u_int i = 0; i < images.dim(); ++i)
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
enum		{c_ContinuousShot, c_Slider, c_Cursor};
static int	range[] = {1, 255, 1};

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
    {C_Slider,	     c_Slider,	     256, "Saturation:",      range, CA_None,
     3, 0, 1, 1, 0},
    EndOfCmds
};

/************************************************************************
*  class MyCanvasPaneBase						*
************************************************************************/
class MyCanvasPaneBase : public CanvasPane
{
  public:
    MyCanvasPaneBase(Window& parentWin, const GenericImage& image,
		     u_int mul, u_int div)				;

    virtual std::istream&	restoreData(std::istream& in)		= 0;
    virtual std::ostream&	saveImage(std::ostream& out)	const	= 0;
};

MyCanvasPaneBase::MyCanvasPaneBase(Window& parentWin,
				   const GenericImage& image,
				   u_int mul, u_int div)
    :CanvasPane(parentWin, (image.width()  * mul) / div,
			   (image.height() * mul) / div)
{
}

/************************************************************************
*  class MyCanvasPane<T>						*
************************************************************************/
template <class T>
class MyCanvasPane : public MyCanvasPaneBase
{
  public:
    MyCanvasPane(Window& parentWin, GenericImage& image,
		 u_int mul, u_int div)					;
    
    virtual std::istream&	restoreData(std::istream& in)		;
    virtual std::ostream&	saveImage(std::ostream& out)	const	;
    virtual void		repaintUnderlay()			;
    virtual void		callback(CmdId id, CmdVal val)		;
    
  private:
#ifdef USE_XVDC
    XvDC			_dc;
#else
#  ifdef USE_SHMDC
    ShmDC			_dc;
#  else
    CanvasPaneDC		_dc;
#  endif
#endif
    const ImageBase::TypeInfo&	_typeInfo;
    Image<T>			_image;
};

template <class T>
MyCanvasPane<T>::MyCanvasPane(Window& parentWin, GenericImage& image,
			      u_int mul, u_int div)
    :MyCanvasPaneBase(parentWin, image, mul, div),
     _dc(*this, image.width(), image.height(), mul, div),
     _typeInfo(image.typeInfo()),
     _image((T*)image.data(), image.width(), image.height())
{
    _image.P  = image.P;
    _image.d1 = image.d1;
    _image.d2 = image.d2;
}

template <class T> std::istream&
MyCanvasPane<T>::restoreData(std::istream& in)
{
    return _image.restoreData(in, _typeInfo);
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
	parent().callback(id,
			  CmdVal(_dc.dev2logU(val.u), _dc.dev2logV(val.v)));
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
		u_int ncol, u_int mul, u_int div, u_int saturation)	;
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
			 u_int ncol, u_int mul, u_int div, u_int saturation)
    :CmdWindow(parentApp, name, 0, Colormap::RGBColor, 16, 0, 0),
     _movie(movie),
     _cmd(*this, Cmds),
     _canvases(images.dim()),
     _timer(*this, 0)
{
    using namespace	std;
    
    _cmd.place(0, 0, ncol, 1);

    for (u_int i = 0; i < _canvases.dim(); ++i)
    {
	switch (images[i].typeInfo().type)
	{
	  case ImageBase::U_CHAR:
	    _canvases[i] = new MyCanvasPane<u_char>(*this,
						    images[i], mul, div);
	    break;
	  case ImageBase::RGB_24:
	    _canvases[i] = new MyCanvasPane<RGB>(*this,
						 images[i], mul, div);
	    break;
	  case ImageBase::SHORT:
	    _canvases[i] = new MyCanvasPane<short>(*this,
						   images[i], mul, div);
	    break;
	  case ImageBase::FLOAT:
	    _canvases[i] = new MyCanvasPane<float>(*this,
						   images[i], mul, div);
	    break;
	  case ImageBase::YUV_444:
	    _canvases[i] = new MyCanvasPane<YUV444>(*this,
						    images[i], mul, div);
	    break;
	  case ImageBase::YUV_422:
	    _canvases[i] = new MyCanvasPane<YUV422>(*this,
						    images[i], mul, div);
	    break;
	  case ImageBase::YUYV_422:
	    _canvases[i] = new MyCanvasPane<YUYV422>(*this,
						     images[i], mul, div);
	    break;
	  case ImageBase::YUV_411:
	    _canvases[i] = new MyCanvasPane<YUV411>(*this,
						    images[i], mul, div);
	    break;
	  case ImageBase::BMP_8:
	    _canvases[i] = new MyCanvasPane<u_char>(*this,
						    images[i], mul, div);
	    break;
	  case ImageBase::BMP_24:
	    _canvases[i] = new MyCanvasPane<BGR>(*this,
						 images[i], mul, div);
	    break;
	  case ImageBase::BMP_32:
	    _canvases[i] = new MyCanvasPane<BGRA>(*this,
						  images[i], mul, div);
	    break;
	  default:
	    throw runtime_error("Unknown image type!!");
	    break;
	}
	
	_canvases[i]->place(i % ncol, 1 + i / ncol, 1, 1);
    }
    
    show();

    _cmd.setValue(c_Slider, int(saturation));
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
    for (u_int i = 0; i < _canvases.dim(); ++i)
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
	    for (u_int i = 0; i < _canvases.dim(); ++i)
		_canvases[i]->saveImage(out);

	if (_cmd.getValue(c_ContinuousShot))
	    _timer.start(5);
      }
	break;

      case c_Slider:
	colormap().setSaturation(val);
	colormap().setSaturationF(val.f());
	for (u_int i = 0; i < _canvases.dim(); ++i)
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
	s << '(' << val.u << ',' << val.v << ')';
	_cmd.setString(c_Cursor, s.str().c_str());
      }
        break;
    }
}

void
MyCmdWindow::tick()
{
    using namespace	std;
    
    for (u_int i = 0; i < _canvases.dim(); ++i)
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
    u_int		ncol = 2, mul = 1, div = 1, saturation = 256;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "n:42HQs:h")) !=EOF; )
	switch (c)
	{
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
	for (u_int i = 0; i < images.dim(); ++i)
	{
	    cerr << i << "-th view: "
		 << images[i].width() << 'x' << images[i].height() << " (";
	    switch (images[i].typeInfo().type)
	    {
	      case ImageBase::U_CHAR:
		cerr << "U_CHAR";
		break;
	      case ImageBase::RGB_24:
		cerr << "RGB_24";
		break;
	      case ImageBase::SHORT:
		cerr << "SHORT";
		break;
	      case ImageBase::FLOAT:
		cerr << "FLOAT";
		break;
	      case ImageBase::YUV_444:
		cerr << "YUV_444";
		break;
	      case ImageBase::YUV_422:
		cerr << "YUV_422";
		break;
	      case ImageBase::YUYV_422:
		cerr << "YUYV_422";
		break;
	      case ImageBase::YUV_411:
		cerr << "YUV_411";
		break;
	      case ImageBase::BMP_8:
		cerr << "BMP_8";
		break;
	      case ImageBase::BMP_24:
		cerr << "BMP_24";
		break;
	      case ImageBase::BMP_32:
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
			      (c == 'M'), ncol, mul, div, saturation);
	vapp.run();
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }
    
    return 0;
}
