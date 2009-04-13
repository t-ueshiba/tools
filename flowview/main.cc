/*
 *  $Id: main.cc,v 1.1 2009-04-13 04:15:04 ueshiba Exp $
 */
#include <stdlib.h>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/CanvasPaneDC.h"
#include "TU/v/Timer.h"

#define PIXEL_TYPE		ImageBase::U_CHAR

namespace TU
{
typedef u_char	PixelType;
    
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
	_images[i].restoreData(cin, PIXEL_TYPE);
    _canvas0.repaintUnderlay();
    _canvas1.repaintUnderlay();
    _canvas2.repaintUnderlay();
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
    
    v::App	vapp(argc, argv);

  // 1フレームあたりの画像数を取得．
    u_int	nviews = 0;
    cin >> nviews >> skipl;
    cerr << nviews << " views." << endl;

  // 画像列を確保し，ヘッダ情報を読み込む．
    Array<Image<PixelType> >	images(std::max(nviews, 3u));
    for (int i = 0; i < nviews; ++i)
    {
	images[i].restoreHeader(cin);
	cerr << i << "-th image: "
	     << images[i].width() << 'x' << images[i].height() << endl;
    }

  // GUIオブジェクトを作り，イベントループを起動．
    v::MyCmdWindow<PixelType>	myWin(vapp, "Real-time image viewer", images);
    vapp.run();
    
    return 0;
}
