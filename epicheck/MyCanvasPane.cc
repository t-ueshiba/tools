/*
 *  $Id: MyCanvasPane.cc,v 1.5 2009-11-16 06:58:57 ueshiba Exp $
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
			   int				view,
			   const Array<Matrix33d>&	F,
			   const Image<u_char>&		image,
			   u_int			lineWidth,
			   u_int			mul,
			   u_int			div)
    :CanvasPane(parentWindow, (image.width()  * mul) / div,
			      (image.height() * mul) / div),
     _view(view), _F(F), _image(image),
     _dc(*this, image.width(), image.height(), mul, div)

{
    _dc << thickness(lineWidth);
#ifdef USE_OVERLAY
    _dc << overlay;
#endif
}

void
MyCanvasPane::drawEpipolarLine(const Point2d& p, u_int otherView)
{
    if (otherView != _view)
    {
	LineP2d	l = p.homogeneous() * _F[otherView];
	_dc << l;
    }
}

void
MyCanvasPane::drawSelfEpipolarLine(const Point2d& p, u_int otherView)
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
