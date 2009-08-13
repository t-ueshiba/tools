/*
 *  $Id: main.cc,v 1.8 2009-08-13 00:06:50 ueshiba Exp $
 */
#include <stdlib.h>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/XvDC.h"
#include "TU/v/Timer.h"

namespace TU
{
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
		 double scale)
	:CanvasPane(parentWin, u_int(scale * image.width()),
			       u_int(scale * image.height())),
	 _dc(*this, image.width(), image.height()), _image(image)	{}

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
		Array<Image<T> >& images, u_int ncol, double scale)	;
    ~MyCmdWindow()							;
    
    void	tick()							;

  private:
    Array<MyCanvasPane<T>*>	_canvases;
    Timer			_timer;
};

template <class T>
MyCmdWindow<T>::MyCmdWindow(App& parentApp, const char* name,
			    Array<Image<T> >& images, u_int ncol, double scale)
    :CmdWindow(parentApp, name, 0, Colormap::RGBColor, 16, 0, 0),
     _canvases(images.dim()),
     _timer(*this, 10)
{
    for (u_int i = 0; i < _canvases.dim(); ++i)
    {
	_canvases[i] = new MyCanvasPane<T>(*this, images[i], scale);
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
      u_int ncol, double scale)
{
  // ヘッダ情報に基づいて画像サイズを設定．
    Array<Image<T> >	images(headers.dim());
    for (int i = 0; i < images.dim(); ++i)
	images[i].resize(headers[i].height(), headers[i].width());
	
  // GUIオブジェクトを作り，イベントループを起動．
    v::MyCmdWindow<T>	myWin(vapp, "Real-time image viewer", images,
			      ncol, scale);
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
    u_int		ncol = 2;
    double		scale = 1.0;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "n:s:")) !=EOF; )
	switch (c)
	{
	  case 'n':
	    ncol = atoi(optarg);
	    break;
	  case 's':
	    scale = atof(optarg);
	    break;
	}
    
  // 1フレームあたりの画像数を取得．
    char	c;
    if (!cin.get(c) || (c != 'M'))
	return 1;
    u_int	nviews = 0;
    cin >> nviews >> skipl;
    cerr << nviews << " views." << endl;
    if (nviews == 0)
	return 0;
    
  // 画像列を確保し，ヘッダ情報を読み込む．
    Array<GenericImage>		headers(nviews);
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
	doJob<u_char>(vapp, headers, ncol, scale);
	break;
      case ImageBase::RGB_24:
	doJob<RGBA>(vapp, headers, ncol, scale);
	break;
      case ImageBase::YUV_444:
	doJob<YUV444>(vapp, headers, ncol, scale);
	break;
      case ImageBase::YUV_422:
	doJob<YUV422>(vapp, headers, ncol, scale);
	break;
      case ImageBase::YUV_411:
	doJob<YUV411>(vapp, headers, ncol, scale);
	break;
    }
    
    return 0;
}
