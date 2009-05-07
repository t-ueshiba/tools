/*
 *  $Id: main.cc,v 1.2 2009-05-07 04:24:14 ueshiba Exp $
 */
#include <stdlib.h>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/CanvasPaneDC.h"
#include "TU/v/Timer.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
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

namespace v
{
/************************************************************************
*  class MyCanvasPane							*
************************************************************************/
template <class T>
class MyCanvasPane : public CanvasPane
{
  public:
    MyCanvasPane(Window& parentWin, const Image<T>& image)
	:CanvasPane(parentWin, image.width(), image.height()),
	 _dc(*this), _image(image)					{}
    
    virtual void	repaintUnderlay()				;
    
  private:
    CanvasPaneDC	_dc;
    const Image<T>&	_image;
};

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
		Array<Image<T> >& images)				;

    void		tick()						;

  private:
    Array<Image<T> >&	_images;
    MyCanvasPane<T>	_canvas0;
    MyCanvasPane<T>	_canvas1;
    MyCanvasPane<T>	_canvas2;
    Timer		_timer;
};

template <class T>
MyCmdWindow<T>::MyCmdWindow(App& parentApp, const char* name,
			    Array<Image<T> >& images)
    :CmdWindow(parentApp, name, 0, Colormap::RGBColor, 16, 0, 0),
     _images(images),
     _canvas0(*this, _images[0]),
     _canvas1(*this, _images[1]),
     _canvas2(*this, _images[2]),
     _timer(*this, 10)
{
    _canvas0.place(0, 0, 1, 1);
    _canvas1.place(1, 0, 1, 1);
    _canvas2.place(2, 0, 1, 1);
    
    show();
}
 
template <class T> void
MyCmdWindow<T>::tick()
{
    using namespace	std;

    for (int i = 0; i < _images.dim(); ++i)
	_images[i].restoreData(cin, pixelType<T>());
    _canvas0.repaintUnderlay();
    _canvas1.repaintUnderlay();
    _canvas2.repaintUnderlay();
}

}

template <class T> static void
doJob(v::App& vapp, const Array<GenericImage>& headers)
{
  // ヘッダ情報に基づいて画像サイズを設定．
    Array<Image<T> >	images(std::max(headers.dim(), 3u));
    for (int i = 0; i < headers.dim(); ++i)
	images[i].resize(headers[i].height(), headers[i].width());
	
  // GUIオブジェクトを作り，イベントループを起動．
    v::MyCmdWindow<T>	myWin(vapp, "Real-time image viewer", images);
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
    
    v::App	vapp(argc, argv);

  // 1フレームあたりの画像数を取得．
    u_int	nviews = 0;
    cin >> nviews >> skipl;
    cerr << nviews << " views." << endl;

  // 画像列を確保し，ヘッダ情報を読み込む．
    Array<GenericImage>		headers(nviews);
    ImageBase::Type		type;
    for (int i = 0; i < headers.dim(); ++i)
    {
	type = headers[i].restoreHeader(cin);
	cerr << i << "-th image: "
	     << headers[i].width() << 'x' << headers[i].height() << endl;
    }

  // 画素のタイプに応じて処理を行う．
    switch (type)
    {
      case ImageBase::U_CHAR:
	doJob<u_char>(vapp, headers);
	break;
      case ImageBase::RGB_24:
	doJob<RGBA>(vapp, headers);
	break;
      case ImageBase::YUV_444:
	doJob<YUV444>(vapp, headers);
	break;
      case ImageBase::YUV_422:
	doJob<YUV422>(vapp, headers);
	break;
      case ImageBase::YUV_411:
	doJob<YUV411>(vapp, headers);
	break;
    }
    
    return 0;
}
