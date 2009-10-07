/*
 *  $Id: main.cc,v 1.10 2009-10-07 04:17:27 ueshiba Exp $
 */
#include <stdlib.h>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/XvDC.h"
#include "TU/v/Timer.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nRead an image stream with multiple views from stdin and display it.\n"
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
enum	{c_Slider};

static int	range[] = {1, 255, 1};
static CmdDef	Cmds[] =
{
    {C_Slider, c_Slider, 256, "Saturation:", range, CA_None, 0, 0, 1, 1, 0},
    EndOfCmds
};

/************************************************************************
*  class MyCanvasPaneBase						*
************************************************************************/
class MyCanvasPaneBase : public CanvasPane
{
  public:
    MyCanvasPaneBase(Window& parentWin, const GenericImage& header,
		     u_int mul, u_int div)				;

    virtual std::istream&	restoreData(std::istream& in)		= 0;
};

MyCanvasPaneBase::MyCanvasPaneBase(Window& parentWin,
				   const GenericImage& header,
				   u_int mul, u_int div)
    :CanvasPane(parentWin, (header.width()  * mul) / div,
			   (header.height() * mul) / div)
{
}

/************************************************************************
*  class MyCanvasPane<T>						*
************************************************************************/
template <class T>
class MyCanvasPane : public MyCanvasPaneBase
{
  public:
    MyCanvasPane(Window& parentWin, const GenericImage& header,
		 u_int mul, u_int div)					;
    
    virtual std::istream&	restoreData(std::istream& in)		;
    virtual void		repaintUnderlay()			;
    
  private:
#ifdef USE_XVDC
    XvDC	_dc;
#else
    ShmDC	_dc;
#endif
    Image<T>	_image;
};

template <class T>
MyCanvasPane<T>::MyCanvasPane(Window& parentWin, const GenericImage& header,
			      u_int mul, u_int div)
    :MyCanvasPaneBase(parentWin, header, mul, div),
     _dc(*this, header.width(), header.height(), mul, div),
     _image(header.width(), header.height())
{
}

template <class T> std::istream&
MyCanvasPane<T>::restoreData(std::istream& in)
{
    return _image.restoreData(in);
}
        
template <class T> void
MyCanvasPane<T>::repaintUnderlay()
{
    _dc << _image;
}

/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
class MyCmdWindow : public CmdWindow
{
  public:
    MyCmdWindow(App& parentApp, const char* name,
		const Array<GenericImage>& headers,
		u_int ncol, u_int mul, u_int div, u_int saturation)	;
    ~MyCmdWindow()							;
    
    virtual void	callback(CmdId id, CmdVal val)			;
    void		tick()						;

  private:
    CmdPane			_cmd;
    Array<MyCanvasPaneBase*>	_canvases;
    Timer			_timer;
};

MyCmdWindow::MyCmdWindow(App& parentApp, const char* name,
			 const Array<GenericImage>& headers,
			 u_int ncol, u_int mul, u_int div, u_int saturation)
    :CmdWindow(parentApp, name, 0, Colormap::RGBColor, 16, 0, 0),
     _cmd(*this, Cmds),
     _canvases(headers.dim()),
     _timer(*this, 10)
{
    _cmd.place(0, 0, ncol, 1);

    for (u_int i = 0; i < _canvases.dim(); ++i)
    {
	switch (headers[i].type())
	{
	  case ImageBase::RGB_24:
	    _canvases[i] = new MyCanvasPane<RGBA>(*this,
						  headers[i], mul, div);
	    break;
	  case ImageBase::SHORT:
	    _canvases[i] = new MyCanvasPane<short>(*this,
						   headers[i], mul, div);
	    break;
	  case ImageBase::FLOAT:
	    _canvases[i] = new MyCanvasPane<float>(*this,
						   headers[i], mul, div);
	    break;
	  case ImageBase::YUV_444:
	    _canvases[i] = new MyCanvasPane<YUV444>(*this,
						    headers[i], mul, div);
	    break;
	  case ImageBase::YUV_422:
	    _canvases[i] = new MyCanvasPane<YUV422>(*this,
						    headers[i], mul, div);
	    break;
	  case ImageBase::YUV_411:
	    _canvases[i] = new MyCanvasPane<YUV411>(*this,
						    headers[i], mul, div);
	    break;
	  default:
	    _canvases[i] = new MyCanvasPane<u_char>(*this,
						    headers[i], mul, div);
	    break;
	}
	
	_canvases[i]->place(i % ncol, 1 + i / ncol, 1, 1);
    }
    
    show();

    _cmd.setValue(c_Slider, int(saturation));
    colormap().setSaturation(saturation);
    colormap().setSaturationF(saturation);
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

      case c_Slider:
	colormap().setSaturation(val);
	colormap().setSaturationF(val.f());
	for (u_int i = 0; i < _canvases.dim(); ++i)
	    _canvases[i]->repaintUnderlay();
	break;
    }
}
 
void
MyCmdWindow::tick()
{
    for (u_int i = 0; i < _canvases.dim(); ++i)
    {
	_canvases[i]->restoreData(std::cin);
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
      // 1フレームあたりの画像数を取得．
	char	c;
	if (!cin.get(c) || (c != 'M'))
	    throw runtime_error("Not an image stream!");
	u_int	nviews = 0;
	cin >> nviews >> skipl;
	cerr << nviews << " views." << endl;
	if (nviews == 0)
	    throw runtime_error("No views found in the input image stream!");
    
      // 画像列を確保し，ヘッダ情報を読み込む．
	Array<GenericImage>	headers(nviews);
	for (u_int i = 0; i < headers.dim(); ++i)
	{
	    headers[i].restoreHeader(cin);
	    cerr << i << "-th image: "
		 << headers[i].width() << 'x' << headers[i].height() << endl;
	}

      // GUIオブジェクトを作り，イベントループを起動．
	v::MyCmdWindow	myWin(vapp, "Real-time image viewer", headers,
			      ncol, mul, div, saturation);
	vapp.run();
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }
    
    return 0;
}
