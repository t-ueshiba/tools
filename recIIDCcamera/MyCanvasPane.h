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
    MyCanvasPane(Window& parentWin, size_t width, size_t height,
		 const Image<PIXEL>& image)
	:CanvasPane(parentWin, width, height),
	 _dc(*this, image.width(), image.height()), _image(image)	{}
    MyCanvasPane(Window& parentWin, const Image<PIXEL>& image,
		 size_t width, size_t height, float zoom)
	:CanvasPane(parentWin,
		    size_t(width * zoom), size_t(height * zoom)),
	 _dc(*this, width, height, zoom), _image(image)			{}
    
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
    _dc.setSize(_image.width(), _image.height(), _dc.zoom());
}

template <class PIXEL> void
MyCanvasPane<PIXEL>::repaintUnderlay()
{
    _dc << _image;
}

}
}
