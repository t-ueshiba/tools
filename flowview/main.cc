/*
 *  $Id: main.cc,v 1.5 2009-08-11 00:45:53 ueshiba Exp $
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
    MyCanvasPane(Window& parentWin, const Image<T>& image)
	:CanvasPane(parentWin, image.width(), image.height()),
	 _dc(*this), _image(image)					{}
    
    virtual void	repaintUnderlay()				;
    
  private:
#ifdef USE_XVDC
    XvDC		_dc;
#else
    ShmDC		_dc;
#endif
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
    Array<Image<T> >&		_images;
    MyCanvasPane<T>		_canvas0;
    MyCanvasPane<T>		_canvas1;
    MyCanvasPane<T>		_canvas2;
    Timer			_timer;
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
	_images[i].restoreData(cin);
    _canvas0.repaintUnderlay();
    _canvas1.repaintUnderlay();
    _canvas2.repaintUnderlay();
}

}

/************************************************************************
*  static functions							*
************************************************************************/
template <class T> static void
doJob(v::App& vapp, const Array<GenericImage>& headers)
{
  // �إå�����˴�Ť��Ʋ��������������ꡥ
    Array<Image<T> >	images(std::max(headers.dim(), 3u));
    for (int i = 0; i < headers.dim(); ++i)
	images[i].resize(headers[i].height(), headers[i].width());
	
  // GUI���֥������Ȥ��ꡤ���٥�ȥ롼�פ�ư��
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

  // 1�ե졼�ढ����β������������
    char	c;
    if (!cin.get(c) || (c != 'M'))
	return 1;
    u_int	nviews = 0;
    cin >> nviews >> skipl;
    cerr << nviews << " views." << endl;
    if (nviews == 0)
	return 0;
    
  // ���������ݤ����إå�������ɤ߹��ࡥ
    Array<GenericImage>		headers(nviews);
    for (int i = 0; i < headers.dim(); ++i)
    {
	headers[i].restoreHeader(cin);
	cerr << i << "-th image: "
	     << headers[i].width() << 'x' << headers[i].height() << endl;
    }

  // ���ǤΥ����פ˱����ƽ�����Ԥ���
    switch (headers[0].type())
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
