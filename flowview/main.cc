/*
 *  $Id: main.cc,v 1.9 2009-08-13 23:10:57 ueshiba Exp $
 */
#include <stdlib.h>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
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
cerr << " Usage: " << s << " [-n #images] [-[QH24]] < imageStreamFile\n"
	 << endl;
    cerr << " options.\n"
	 << "  -n #images:  #images displayed per row\n"
	 << "  -Q:          display with quater size\n"
	 << "  -H:          display with half size\n"
	 << "  -2:          display with double size\n"
	 << "  -4:          display with quad size\n"
	 << "  -h:          print this\n"
	 << endl;
}

namespace v
{
/************************************************************************
*  class MyCanvasPane							*
************************************************************************/
template <class T>
class MyCanvasPane : public CanvasPane
{
  public:
    MyCanvasPane(Window& parentWin, Image<T>& image,
		 u_int mul, u_int div)					;

    Image<T>&		image()						;
    virtual void	repaintUnderlay()				;
    
  private:
#ifdef USE_XVDC
    XvDC	_dc;
#else
    ShmDC	_dc;
#endif
    Image<T>&	_image;
};

template <class T>
MyCanvasPane<T>::MyCanvasPane(Window& parentWin, Image<T>& image,
			      u_int mul, u_int div)
    :CanvasPane(parentWin, (image.width()  * mul) / div,
			   (image.height() * mul) / div),
     _dc(*this, image.width(), image.height(), mul, div), _image(image)
{
}

template <class T> Image<T>&
MyCanvasPane<T>::image()
{
    return _image;
}
        
template <class T> void
MyCanvasPane<T>::repaintUnderlay()
{
    _dc << _image;
}

/************************************************************************
*  class MyCmdWindow<T>							*
************************************************************************/
template <class T>
class MyCmdWindow : public CmdWindow
{
  public:
    MyCmdWindow(App& parentApp, const char* name,
		Array<Image<T> >& images,
		u_int ncol, u_int mul, u_int div)			;
    ~MyCmdWindow()							;
    
    void	tick()							;

  private:
    Array<MyCanvasPane<T>*>	_canvases;
    Timer			_timer;
};

template <class T>
MyCmdWindow<T>::MyCmdWindow(App& parentApp, const char* name,
			    Array<Image<T> >& images,
			    u_int ncol, u_int mul, u_int div)
    :CmdWindow(parentApp, name, 0, Colormap::RGBColor, 16, 0, 0),
     _canvases(images.dim()),
     _timer(*this, 10)
{
    for (u_int i = 0; i < _canvases.dim(); ++i)
    {
	_canvases[i] = new MyCanvasPane<T>(*this, images[i], mul, div);
	_canvases[i]->place(i % ncol, i / ncol, 1, 1);
    }
    
    show();
}

template <class T>
MyCmdWindow<T>::~MyCmdWindow()
{
    for (u_int i = 0; i < _canvases.dim(); ++i)
	delete _canvases[i];
}
 
template <class T> void
MyCmdWindow<T>::tick()
{
    using namespace	std;

    for (int i = 0; i < _canvases.dim(); ++i)
    {
	_canvases[i]->image().restoreData(cin);
	_canvases[i]->repaintUnderlay();
    }
}

}

/************************************************************************
*  static functions							*
************************************************************************/
template <class T> static void
doJob(v::App& vapp, const Array<GenericImage>& headers,
      u_int ncol, u_int mul, u_int div)
{
  // ヘッダ情報に基づいて画像サイズを設定．
    Array<Image<T> >	images(headers.dim());
    for (int i = 0; i < images.dim(); ++i)
	images[i].resize(headers[i].height(), headers[i].width());
	
  // GUIオブジェクトを作り，イベントループを起動．
    v::MyCmdWindow<T>	myWin(vapp, "Real-time image viewer", images,
			      ncol, mul, div);
    vapp.run();
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
    u_int		ncol = 2, mul = 1, div = 1;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "n:42HQh")) !=EOF; )
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
	for (int i = 0; i < headers.dim(); ++i)
	{
	    headers[i].restoreHeader(cin);
	    cerr << i << "-th image: "
		 << headers[i].width() << 'x' << headers[i].height() << endl;
	}

      // 画素のタイプに応じて処理を行う．
	switch (headers[0].type())
	{
	  case ImageBase::U_CHAR:
	    doJob<u_char>(vapp, headers, ncol, mul, div);
	    break;
	  case ImageBase::RGB_24:
	    doJob<RGBA>(vapp, headers, ncol, mul, div);
	    break;
	  case ImageBase::YUV_444:
	    doJob<YUV444>(vapp, headers, ncol, mul, div);
	    break;
	  case ImageBase::YUV_422:
	    doJob<YUV422>(vapp, headers, ncol, mul, div);
	    break;
	  case ImageBase::YUV_411:
	    doJob<YUV411>(vapp, headers, ncol, mul, div);
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }
    
    return 0;
}
