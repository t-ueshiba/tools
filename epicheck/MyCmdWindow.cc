/*
 *  $Id: MyCmdWindow.cc,v 1.2 2008-09-09 05:50:05 ueshiba Exp $
 */
#include "TU/v/App.h"
#include "epicheck.h"

namespace TU
{
namespace v
{
/************************************************************************
*  static data								*
************************************************************************/
enum	{c_Position};

static CmdDef cmds[] =
{
    {C_Label, c_Position, 0, "            ", noProp, CA_None, 0, 0, 1, 1, 12},
    EndOfCmds
};

enum DrawColor
{
    Color_BG   = 0, Color_WHITE = 1, Color_RED     = 2, Color_GREEN  = 3,
    Color_BLUE = 4, Color_CYAN  = 5, Color_MAGENDA = 6, Color_YELLOW = 7
};

/************************************************************************
*  static functions							*
************************************************************************/
inline u_int
view2color(u_int view)
{
    return Color_RED + (view % 6);
}

/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
MyCmdWindow::MyCmdWindow(App&			parentApp,
			 const Image<u_char>	image[],
			 const Matrix<double>&	pair,
			 const u_int		index[],
			 u_int			nfrms,
			 u_int			lineWidth)
    :CmdWindow(parentApp, "epicheck",
#ifdef USE_OVERLAY
	       Colormap::IndexedColor, 16, 8, 3
#else
	       Colormap::RGBColor, 16, 0, 0
#endif
	      ),
     _cmd(*this, cmds), _canvas(nfrms), _q(0, 0), _pair(pair.nrow(), 3*nfrms)
{
    _cmd.place(0, 0, 2, 1);
    
    for (int i = 0; i < nframes(); ++i)
    {
      // Compute F-matrices and create child canvases.
	const Matrix34d&	P0 = image[index[i]].P;
	SVDecomposition<double>	svd(P0);
	Vector<double>		c = svd.Ut()[3];
	Matrix<double>		P0inv = svd.Ut()[0]%svd.Vt()[0]/svd[0]
					      + svd.Ut()[1]%svd.Vt()[1]/svd[1]
					      + svd.Ut()[2]%svd.Vt()[2]/svd[2];
	Array<Matrix33d>	F(nframes());
	for (int j = 0; j < nframes(); ++j)
	{
	    const Matrix34d&	P1 = image[index[j]].P;
	    F[j] = (P1 * c).skew() * P1 * P0inv;
	}
	_canvas[i] = new MyCanvasPane(*this, i, F, image[index[i]], lineWidth);
	_canvas[i]->place(i%2, 1 + i/2, 1, 1);

      // Extract pairs.
	for (int n = 0; n < npairs(); ++n)
	{
	    _pair[n](3*i, 3) = pair[n](3*index[i], 3);
	}
    }

    _bgr[Color_BG]	= BGR(  0,   0,   0);	// black
    _bgr[Color_WHITE]	= BGR(255, 255, 255);	// white
    _bgr[Color_RED]	= BGR(255,   0,   0);	// red
    _bgr[Color_GREEN]	= BGR(  0, 255,   0);	// green
    _bgr[Color_BLUE]	= BGR(  0,   0, 255);	// blue
    _bgr[Color_CYAN]	= BGR(  0, 255, 255);	// cyan
    _bgr[Color_MAGENDA]	= BGR(255,   0, 255);	// magenda
    _bgr[Color_YELLOW]	= BGR(255, 255,   0);	// yellow
#ifdef USE_OVERLAY
    for (int j = 1; j < 8; ++j)
    {
	colormap().setUnderlayValue(j, _bgr[j]);
	colormap().setOverlayValue(j, _bgr[j]);
    }
#endif
    show();
}

MyCmdWindow::~MyCmdWindow()
{
    for (int i = 0; i < _canvas.dim(); ++i)
	delete _canvas[i];
}

void
MyCmdWindow::callback(CmdId id, CmdVal val)
{
    u_int	view  = id / MyCanvasPane::NEVENTS,
		event = id % MyCanvasPane::NEVENTS;
    Point2d	p(_canvas[view]->dc().dev2logU(val.u),
		  _canvas[view]->dc().dev2logU(val.v));

    switch (event)
    {
#ifdef USE_OVERLAY
      case 1:		// MouseButton1Drag.
      {			// Erase previously drawn epipolar lines.
	for (int i = 0; i < nframes(); ++i)
	{
	    _canvas[i]->dc() << foreground(0);
	    _canvas[i]->drawEpipolarLine(_q, view);
	    _canvas[view]->dc() << foreground(0);
	    _canvas[view]->drawSelfEpipolarLine(_q, i);
	}
      }
    // Fall down to the next "case" block.
      
      case 0:		// MouseButton1Press.
      {			// Draw epipolar lines.
	for (int i = 0; i < nframes(); ++i)
	{
	    _canvas[i]->dc() << foreground(view2color(i));
	    _canvas[i]->drawEpipolarLine(p, view);
	    _canvas[view]->dc() << foreground(view2color(i));
	    _canvas[view]->drawSelfEpipolarLine(p, i);
	}
	_q = p;
      }
    // Fall down to the next "case" block.

      case 3:		// mouseMove.
      {
	char	s[32];
	sprintf(s, "(%4d, %4d)", p[0], p[1]);
	_cmd.setString(c_Position, s);
      }
	break;

      case 2:		// MouseButton1Release.
      {
	  for (int i = 0; i < nframes(); ++i)
	      _canvas[i]->dc() << clear;
      }
        break;

      case 4:		// repaintUnderlay.
      {
	  _canvas[view]->dc() << foreground(Color_RED) << cross;
	  for (int j = 0; j < npairs(); ++j)
	  {
	      _canvas[view]->dc() << Point2d(_pair[j](3*view, 3));
	      char	s[32];
	      sprintf(s, "%d", j);
	      _canvas[view]->dc().draw(s, _pair[j][3*view] + 5,
				       _pair[j][3*view+1]);
	  }
      }
        break;
    }
#else
      case 2:		// MouseButton1Release.
      case 1:		// MouseButton1Drag.
      {
	for (int i = 0; i < nframes(); ++i)
	    _canvas[i]->repaintUnderlay();
      }
        if (event == 2)
	    break;
      
    // Fall down to the next "case" block.
      
      case 0:		// MouseButton1Press.
      {			// Draw epipolar lines.
	for (int i = 0; i < nframes(); ++i)
	{
	    _canvas[i]->dc() << foreground(_bgr[view2color(i)]);
	    _canvas[i]->drawEpipolarLine(p, view);
	    _canvas[view]->dc() << foreground(_bgr[view2color(i)]);
	    _canvas[view]->drawSelfEpipolarLine(p, i);
	}
	_q = p;
      }
    // Fall down to the next "case" block.

      case 3:		// mouseMove.
      {
	char	s[32];
	sprintf(s, "(%4d, %4d)", int(p[0]), int(p[1]));
	_cmd.setString(c_Position, s);
      }
	break;
    
      case 4:		// repaintUnderlay.
      {
	  for (int i = 0; i < nframes(); ++i)
	  {
	      _canvas[view]->dc() << foreground(_bgr[Color_WHITE]);
	      for (int j = 0; j < npairs(); ++j)
		  _canvas[view]->drawEpipolarLine(_pair[j](3*i, 3), i);
	  }
	  _canvas[view]->dc() << foreground(_bgr[Color_GREEN]) << cross;
	  for (int j = 0; j < npairs(); ++j)
	      _canvas[view]->dc() << Point2d(_pair[j](3*view, 3));
      }
        break;
    }
#endif // !USE_OVERLAY
}
 
}
}
