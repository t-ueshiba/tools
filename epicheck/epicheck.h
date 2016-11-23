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
		 size_t				view,
		 const Array<Matrix33d>&	F,
		 const Image<u_char>&		image,
		 size_t				lineWidth,
		 float				zoom)			;

    CanvasPaneDC&	dc()						;

    void	drawEpipolarLine(const Point2d&	p, size_t otherView)	;
    void	drawSelfEpipolarLine(const Point2d& p, size_t otherView);

    virtual void	callback(CmdId, CmdVal)				;
    virtual void	repaintUnderlay()				;
    
  private:
    const size_t		_view;
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
		size_t				lineWidth,
		size_t				ncol,
		float				zoom)			;
    ~MyCmdWindow()							;

    virtual void	callback(CmdId, CmdVal)				;
    
  private:
    size_t		nviews()				const	;
    size_t		npairs()				const	;

    CmdPane				_cmd;
    Array<MyCanvasPane*>		_canvases;
    Point2d				_q;	// previous cursor location.
    const Array2<Array<Point2d> >&	_pairs;
    BGR					_bgr[8];
};

inline size_t	MyCmdWindow::nviews() const	{return _canvases.dim();}
inline size_t	MyCmdWindow::npairs() const	{return _pairs.dim();}
 
}
}
