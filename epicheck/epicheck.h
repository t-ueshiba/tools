/*
 *  $Id: epicheck.h,v 1.2 2008-09-09 05:50:06 ueshiba Exp $
 */
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/CanvasPaneDC.h"

namespace TU
{
namespace v
{
/************************************************************************
*  MyCanvasPane								*
************************************************************************/
class MyCanvasPane : public CanvasPane
{
  public:
    enum	{NEVENTS = 5};
    
    MyCanvasPane(Window&			parentWindow,
		 int				view,
		 const Array<Matrix33d>&	F,
		 const Image<u_char>&		image,
		 u_int				lineWidth)		;

    CanvasPaneDC&	dc()						;

    void	drawEpipolarLine(const Point2d&	p, u_int otherView)	;
    void	drawSelfEpipolarLine(const Point2d& p, u_int otherView)	;

    virtual void	callback(CmdId, CmdVal)				;
    virtual void	repaintUnderlay()				;
    
  private:
    const int			_view;
    const Array<Matrix33d>	_F;
    const Image<u_char>&	_image;
    CanvasPaneDC		_dc;
};

inline CanvasPaneDC&		MyCanvasPane::dc()	{return _dc;}

/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
class MyCmdWindow : public CmdWindow
{
  public:
    MyCmdWindow(App&			parentApp,
		const Image<u_char>	image[],
		const Matrix<double>&	pair,
		const u_int		index[],
		u_int			nframes,
		u_int			lineWidth)			;
    ~MyCmdWindow()							;

    virtual void	callback(CmdId, CmdVal)				;
    
  private:
    u_int		nframes()				const	;
    u_int		npairs()				const	;

    CmdPane			_cmd;
    Array<MyCanvasPane*>	_canvas;
    Point2d			_q;		// previous cursor location.
    Matrix<double>		_pair;
    BGR				_bgr[8];
};

inline u_int	MyCmdWindow::nframes() const	{return _canvas.dim();}
inline u_int	MyCmdWindow::npairs() const	{return _pair.dim();}
 
}
}
