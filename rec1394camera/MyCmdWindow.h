/*
 *  $Id: MyCmdWindow.h,v 1.2 2010-12-22 01:47:14 ueshiba Exp $
 */
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/FileSelection.h"
#include "TU/v/Timer.h"
#include "MyCanvasPane.h"
#include "TU/Ieee1394++.h"
#include "TU/Movie.h"
#include <string>

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
class MyCmdWindow : public CmdWindow
{
  public:
    MyCmdWindow(App& parentApp, const std::string& cameraBase,
		const Array<Ieee1394Camera*>& cameras,
		u_int ncol, u_int mul, u_int div)		;
    ~MyCmdWindow()						;
    
    virtual void	callback(CmdId, CmdVal)			;
    virtual void	tick()					;
    
  private:
    void		initializeMovie()			;
    void		setCanvases()				;
    void		setNFrames()				;
    void		repaintCanvases()			;
    void		setFrame()				;
    void		stopContinuousShotIfRunning()		;
    void		syncronizedSnap()			;

    const std::string&			_cameraBase;
    const Array<Ieee1394Camera*>&	_cameras;
    const u_int				_ncol;
    const u_int				_mul;
    const u_int				_div;
    bool				_headIsActive;
    Movie<PixelType>			_movie;
    Array<MyCanvasPane*>		_canvases;
    CmdPane				_menuCmd;
    CmdPane				_captureCmd;
    CmdPane				_featureCmd;
    FileSelection			_fileSelection;
    Timer				_timer;
};
 
}
}
