/*
 *  $Id$
 */
#include "epicheck.h"

namespace TU
{
namespace v
{
/************************************************************************
*  MyCanvasPane								*
************************************************************************/
MyCanvasPane::MyCanvasPane(Window&			parentWindow,
			   size_t			view,
			   const Array<Matrix33d>&	F,
			   const Image<u_char>&		image,
			   size_t			lineWidth,
			   float			zoom)
    :CanvasPane(parentWindow,
		size_t(image.width()  * zoom), size_t(image.height() * zoom)),
     _view(view), _F(F), _image(image),
     _dc(*this, image.width(), image.height(), zoom)

{
    _dc << thickness(lineWidth);
#ifdef USE_OVERLAY
    _dc << overlay;
#endif
}

void
MyCanvasPane::drawEpipolarLine(const Point2d& p, size_t otherView)
{
    if (otherView != _view)
    {
	LineP2d	l = p.homogeneous() * _F[otherView];
	_dc << l;
    }
}

void
MyCanvasPane::drawSelfEpipolarLine(const Point2d& p, size_t otherView)
{
    if (otherView != _view)
    {
	LineP2d	l = (_F[otherView][0] ^ _F[otherView][1]) ^ p.homogeneous();
	_dc << l;
    }
}

void
MyCanvasPane::callback(CmdId id, CmdVal val)
{
    switch (id)
    {
      case Id_MouseButton1Press:
	parent().callback(NEVENTS * _view, val);
	break;
      case Id_MouseButton1Drag:
	parent().callback(NEVENTS * _view + 1, val);
	break;
      case Id_MouseButton1Release:
	parent().callback(NEVENTS * _view + 2, val);
	break;
      case Id_MouseMove:
	parent().callback(NEVENTS * _view + 3, val);
	break;
    }
}

void
MyCanvasPane::repaintUnderlay()
{
    _dc << _image;
    parent().callback(NEVENTS * _view + 4, 0);
}
 
}
}
