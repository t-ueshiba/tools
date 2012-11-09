/*
 *  $Id$
 */
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/CanvasPane.h"
#include "TU/v/ShmDC.h"

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
		 u_int				lineWidth,
		 u_int				mul,
		 u_int				div)			;

    CanvasPaneDC&	dc()						;

    void	drawEpipolarLine(const Point2d&	p, u_int otherView)	;
    void	drawSelfEpipolarLine(const Point2d& p, u_int otherView)	;

    virtual void	callback(CmdId, CmdVal)				;
    virtual void	repaintUnderlay()				;
    
  private:
    const int			_view;
    const Array<Matrix33d>	_F;
    const Image<u_char>&	_image;
#ifdef USE_SHMDC
    ShmDC			_dc;
#else
    CanvasPaneDC		_dc;
#endif
};

inline CanvasPaneDC&		MyCanvasPane::dc()	{return _dc;}

/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
class MyCmdWindow : public CmdWindow
{
  public:
    MyCmdWindow(App&				parentApp,
		const Array<Image<u_char> >&	images,
		const Array2<Array<Point2d> >&	pairs,
		u_int				lineWidth,
		u_int				ncol,
		u_int				mul,
		u_int				div)			;
    ~MyCmdWindow()							;

    virtual void	callback(CmdId, CmdVal)				;
    
  private:
    u_int		nviews()				const	;
    u_int		npairs()				const	;

    CmdPane				_cmd;
    Array<MyCanvasPane*>		_canvases;
    Point2d				_q;	// previous cursor location.
    const Array2<Array<Point2d> >&	_pairs;
    BGR					_bgr[8];
};

inline u_int	MyCmdWindow::nviews() const	{return _canvases.dim();}
inline u_int	MyCmdWindow::npairs() const	{return _pairs.dim();}
 
}
}
