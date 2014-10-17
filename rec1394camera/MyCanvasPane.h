/*
 *  $Id$
 */
#include "TU/v/CanvasPane.h"
#include "TU/v/CanvasPaneDC.h"
#if defined(USE_SHMDC)
#  include "TU/v/ShmDC.h"
#elif defined(USE_XVDC)
#  include "TU/v/XvDC.h"
#endif

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCanvasPane<PIXEL>						*
************************************************************************/
template <class PIXEL>
class MyCanvasPane : public CanvasPane
{
  public:
    MyCanvasPane(Window& parentWin, u_int width, u_int height,
		 const Image<PIXEL>& image)
	:CanvasPane(parentWin, width, height),
	 _dc(*this, image.width(), image.height()), _image(image)	{}
    MyCanvasPane(Window& parentWin, const Image<PIXEL>& image,
		 u_int w, u_int h, u_int mul, u_int div)
	:CanvasPane(parentWin, (w * mul)/div, (h * mul)/div),
	 _dc(*this, w, h, mul, div), _image(image)			{}
    
    void		resize()					;
    virtual void	repaintUnderlay()				;
    
  private:
#if defined(USE_SHMDC)
    ShmDC		_dc;
#elif defined(USE_XVDC)
    XvDC		_dc;
#else
    CanvasPaneDC	_dc;
#endif
    const Image<PIXEL>&	_image;
};

template <class PIXEL> inline void
MyCanvasPane<PIXEL>::resize()
{
    _dc.setSize(_image.width(), _image.height(), _dc.mul(), _dc.div());
}

template <class PIXEL> void
MyCanvasPane<PIXEL>::repaintUnderlay()
{
    _dc << _image;
}

}
}
