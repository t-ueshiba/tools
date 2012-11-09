/*
 *  $Id$
 */
#include "TU/v/CanvasPane.h"
#if defined(USE_SHMDC)
#  include "TU/v/ShmDC.h"
#elif defined(USE_XVDC)
#  include "TU/v/XvDC.h"
#else
#  include "TU/v/CanvasPaneDC.h"
#endif

namespace TU
{
#ifdef MONO_IMAGE
typedef u_char		PixelType;
#else
typedef RGBA		PixelType;
#endif

namespace v
{
/************************************************************************
*  class MyCanvasPane							*
************************************************************************/
class MyCanvasPane : public CanvasPane
{
  public:
    MyCanvasPane(Window& parentWin, const Image<PixelType>& image,
		 u_int w, u_int h, u_int mul, u_int div)
	:CanvasPane(parentWin, (w *  mul) / div, (h * mul) / div),
	 _dc(*this, w, h, mul, div),
	 _image(image)							{}
    
    void		resize()					;
    virtual void	repaintUnderlay()				;
    
  private:
#if defined(USE_XVDC)
    XvDC			_dc;
#elif defined(USE_SHMDC)
    ShmDC			_dc;
#else
    CanvasPaneDC		_dc;
#endif
    const Image<PixelType>&	_image;
};

inline void
MyCanvasPane::resize()
{
    _dc.setSize(_image.width(), _image.height(), _dc.mul(), _dc.div());
}

}
}
