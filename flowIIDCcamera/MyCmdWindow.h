/*
 *  $Id: MyCmdWindow.h,v 1.3 2011-01-05 02:04:50 ueshiba Exp $
 */
#include <iterator>
#include <fstream>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/Timer.h"
#include "CaptureAndSave.h"
#include "flowcamera.h"

namespace TU
{
namespace v
{
/************************************************************************
*  class MyCmdWindow<CAMERAS>						*
************************************************************************/
template <class CAMERAS>
class MyCmdWindow : public CmdWindow
{
  public:
    typedef typename std::iterator_traits<
		typename CAMERAS::value_type>::value_type	camera_type;
    
  public:
    MyCmdWindow(App& parentApp, CAMERAS& cameras)			;

    virtual void	callback(CmdId, CmdVal)				;
    virtual void	tick()						;
    
  private:
    void		continuousShot(bool enable)			;

  private:
    CAMERAS&			_cameras;
    CaptureAndSave<camera_type>	_captureAndSave;
    CmdPane			_menuCmd;
    CmdPane			_featureCmd;
    Timer			_timer;
};

template <class CAMERAS>
MyCmdWindow<CAMERAS>::MyCmdWindow(App& parentApp, CAMERAS& cameras)
    :CmdWindow(parentApp, "Multi camera controller",
	       Colormap::RGBColor, 16, 0, 0),
     _cameras(cameras),
     _captureAndSave(cameras),
     _menuCmd(*this, createMenuCmds(*_cameras[0])),
     _featureCmd(*this, createFeatureCmds(_cameras)),
     _timer(*this, 0)
{
    _menuCmd.place(0, 0, 1, 1);
    _featureCmd.place(0, 1, 1, 1);
    show();

    _captureAndSave.saveHeaders(std::cout);	// 画像ヘッダを出力

    continuousShot(true);			// カメラからの画像出力を開始
}
    
template <class CAMERAS> void
MyCmdWindow<CAMERAS>::callback(CmdId id, CmdVal val)
{
    using namespace	std;

    try
    {
	if (setFormat(_cameras, id, val))
	{
	    _captureAndSave.setFormat(_cameras);
	    return;
	}
	else if (setSpecialFormat(_cameras, id, val, *this))
	{
	    _captureAndSave.setFormat(_cameras);
	    return;
	}
	else if (setFeatureValue(_cameras, id, val, _featureCmd))
	    return;
	
	switch (id)
	{
	  case M_Exit:
	    app().exit();
	    break;

	  case M_Open:
	  {
	    continuousShot(false);

	    ifstream	in(_cameras.configFile().c_str());
	    if (in)
		in >> _cameras;
	    refreshFeatureCmds(_cameras, _featureCmd);
	    
	    continuousShot(true);
	  }
	    break;
      
	  case M_Save:
	  {
	    continuousShot(false);

	    ofstream	out(_cameras.configFile().c_str());
	    if (out)
		out << _cameras;

	    continuousShot(true);
	  }
	    break;
      
	  case c_ContinuousShot:
	    continuousShot(val);
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what();
    }
}

template <class CAMERAS> void
MyCmdWindow<CAMERAS>::tick()
{
    if (!active)
	app().exit();
    
    static int		nframes = 0;
    static timeval	start;
    countTime(nframes, start);

    _captureAndSave(std::cout);
}

template <class CAMERAS> void
MyCmdWindow<CAMERAS>::continuousShot(bool enable)
{
    if (enable)
    {
	exec(_cameras, &camera_type::continuousShot, true);
	_timer.start(1);
    }
    else
    {
	_timer.stop();
	exec(_cameras, &camera_type::continuousShot, false);
    }
}

}
}
