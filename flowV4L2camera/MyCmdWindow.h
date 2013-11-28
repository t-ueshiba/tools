/*
 *  $Id: MyCmdWindow.h,v 1.3 2011-01-05 02:04:50 ueshiba Exp $
 */
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/Timer.h"
#include "CaptureAndSave.h"
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
    MyCmdWindow(App& parentApp, const std::string& baseName,
		const std::string& dev, V4L2Camera& camera)		;

    virtual void	callback(CmdId, CmdVal)				;
    virtual void	tick()						;
    
  private:
    void		continuousShot()				;
    void		stopContinuousShot()				;

  private:
    const std::string&	_baseName;
    const std::string&	_dev;
    V4L2Camera&		_camera;
    CaptureAndSave	_captureAndSave;
    CmdPane		_menuCmd;
    CmdPane		_featureCmd;
    Timer		_timer;
};
 
}
}
