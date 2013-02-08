/*
 *  $Id: MyCmdWindow.h,v 1.3 2011-01-05 02:04:50 ueshiba Exp $
 */
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/Timer.h"
#include "CaptureAndSave.h"

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
    MyCmdWindow(App& parentApp, const Ieee1394CameraArray& cameras)	;

    virtual void	callback(CmdId, CmdVal)				;
    virtual void	tick()						;
    
  private:
    void		continuousShot()				;
    void		stopContinuousShot()				;

  private:
    const Ieee1394CameraArray&	_cameras;
    CaptureAndSave		_captureAndSave;
    CmdPane			_menuCmd;
    CmdPane			_featureCmd;
    Timer			_timer;
};
 
}
}
