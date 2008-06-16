/*
 *  $Id: MyCanvasPane.cc,v 1.1.1.1 2008-06-16 03:15:08 ueshiba Exp $
 */
#include "epicheck.h"

namespace TU
{
namespace v
{
/************************************************************************
*  static data/functions						*
************************************************************************/
static const u_int	MaxCanvasWidth  = 512;
static const u_int	MaxCanvasHeight = 480;

inline u_int	min(u_int x, u_int y)	{return (x < y ? x : y);}

/************************************************************************
*  MyCanvasPane								*
************************************************************************/
MyCanvasPane::MyCanvasPane(Window&			 parentWindow,
			   int				 view,
			   const Array<Matrix<double> >& F,
			   const Image<u_char>&		 image,
			   u_int			 lineWidth)
    :CanvasPane(parentWindow,
		min(image.width(),  MaxCanvasWidth),
		min(image.height(), MaxCanvasHeight)),
     _view(view), _F(F), _image(image),
     _dc(*this, image.width(), image.height())

{
    _dc << thickness(lineWidth);
#ifdef USE_OVERLAY
    _dc << overlay;
#endif
}

void
MyCanvasPane::drawEpipolarLine(const Point2<double>& p, u_int otherView)
{
    if (otherView != _view)
    {
	LineP2d	l = p.hom() * _F[otherView];
	_dc << l;
    }
}

void
MyCanvasPane::drawSelfEpipolarLine(const Point2<double>& p, u_int otherView)
{
    if (otherView != _view)
    {
	LineP2d	l = (_F[otherView][0] ^ _F[otherView][1]) ^ p.hom();
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
