/*
 *  $Id: MyCmdWindow.h,v 1.3 2011-01-14 02:13:40 ueshiba Exp $
 */
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/FileSelection.h"
#include "TU/v/Timer.h"
#include "MyCanvasPane.h"
#include "TU/Ieee1394CameraArray.h"
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
    MyCmdWindow(App& parentApp,
		const Ieee1394CameraArray& cameras,
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

    const Ieee1394CameraArray&	_cameras;
    const u_int			_ncol;
    const u_int			_mul;
    const u_int			_div;
    bool			_headIsActive;
    Movie<PixelType>		_movie;
    Array<MyCanvasPane*>	_canvases;
    CmdPane			_menuCmd;
    CmdPane			_captureCmd;
    CmdPane			_featureCmd;
    FileSelection		_fileSelection;
    Timer			_timer;
};
 
}
}
