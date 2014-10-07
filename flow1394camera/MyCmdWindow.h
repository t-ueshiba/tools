/*
 *  $Id: MyCmdWindow.h,v 1.3 2011-01-05 02:04:50 ueshiba Exp $
 */
#include <sys/time.h>
#include "TU/v/App.h"
#include "TU/v/CmdWindow.h"
#include "TU/v/CmdPane.h"
#include "TU/v/Timer.h"
#include "CaptureAndSave.h"

namespace TU
{
extern bool	active;

/************************************************************************
*  global functions							*
************************************************************************/
inline void
countTime(int& nframes, timeval& start)
{
    using namespace	std;

    if (nframes == 10)
    {
	timeval	end;
	gettimeofday(&end, NULL);
	double	interval = (end.tv_sec  - start.tv_sec) +
	    (end.tv_usec - start.tv_usec) / 1.0e6;
	cerr << nframes / interval << " frames/sec" << endl;
	nframes = 0;
    }
    if (nframes++ == 0)
	gettimeofday(&start, NULL);
}

template <class CAMERAS> void
run(const CAMERAS& cameras)
{
    typedef typename CaptureAndSave<CAMERAS>::camera_type	camera_type;
    
    CaptureAndSave<CAMERAS>	captureAndSave(cameras);
    captureAndSave.saveHeaders(std::cout);		// 画像数とヘッダを出力
    cameras.exec(&camera_type::continuousShot);		// カメラ出力開始

    int		nframes = 0;
    timeval	start;
    while (active)
    {
	countTime(nframes, start);

	if (!captureAndSave(std::cout))
	    active = false;
    }

    cameras.exec(&camera_type::stopContinuousShot);	// カメラ出力停止
}

namespace v
{
/************************************************************************
*  class MyCmdWindow<CAMERAS>						*
************************************************************************/
template <class CAMERAS> class MyCmdWindow : public CmdWindow
{
  public:
    typedef typename CaptureAndSave<CAMERAS>::camera_type	camera_type;
    
  public:
    MyCmdWindow(App& parentApp, const CAMERAS& cameras,
		CmdDef* menuCmds, CmdDef* featureCmds)			;

    virtual void	callback(CmdId, CmdVal)				;
    virtual void	tick()						;
    
  private:
    void		continuousShot()				;
    void		stopContinuousShot()				;

  private:
    const CAMERAS&		_cameras;
    CaptureAndSave<CAMERAS>	_captureAndSave;
    CmdPane			_menuCmd;
    CmdPane			_featureCmd;
    Timer			_timer;
};

template <class CAMERAS>
MyCmdWindow<CAMERAS>::MyCmdWindow(App& parentApp, const CAMERAS& cameras,
				  CmdDef* menuCmds, CmdDef* featureCmds)
    :CmdWindow(parentApp, "Multi camera controller",
	       0, Colormap::RGBColor, 16, 0, 0),
     _cameras(cameras),
     _captureAndSave(cameras),
     _menuCmd(*this, menuCmds),
     _featureCmd(*this, featureCmds),
     _timer(*this, 0)
{
    _menuCmd.place(0, 0, 1, 1);
    _featureCmd.place(0, 1, 1, 1);
    show();

    _captureAndSave.saveHeaders(std::cout);	// 画像ヘッダを出力

    continuousShot();				// カメラからの画像出力を開始
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
MyCmdWindow<CAMERAS>::continuousShot()
{
    _cameras.exec(&camera_type::continuousShot);
    _timer.start(1);
}

template <class CAMERAS> void
MyCmdWindow<CAMERAS>::stopContinuousShot()
{
    _timer.stop();
    _cameras.exec(&camera_type::stopContinuousShot);
}

}
}
