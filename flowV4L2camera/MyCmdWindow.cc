/*
 *  $Id: MyCmdWindow.cc,v 1.7 2012-06-30 20:00:51 ueshiba Exp $
 */
#include "flowV4L2camera.h"
#include "MyCmdWindow.h"
#include <sys/time.h>
#include <fstream>

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
extern bool	active;
void		countTime(int& nframes, timeval& start)			;

namespace v
{
CmdDef*		createMenuCmds(const V4L2Camera& camera)		;
CmdDef*		createFeatureCmds(const V4L2CameraArray& cameras)	;
void		refreshFeatureCmds(const V4L2Camera& camera,
				   CmdPane& cmdPane)			;
				   
/************************************************************************
*  class MyCmdWindow							*
************************************************************************/
MyCmdWindow::MyCmdWindow(App& parentApp, const V4L2CameraArray& cameras)
    :CmdWindow(parentApp, "V4L2 camera controller",
	       0, Colormap::RGBColor, 16, 0, 0),
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

    if (_menuCmd.getValue(c_ContinuousShot))	// ボタンの初期値がtrueならば...
	continuousShot();			// カメラからの画像出力を開始
}

void
MyCmdWindow::callback(CmdId id, CmdVal val)
{
    using namespace	std;

    try
    {
	switch (id)
	{
	  case M_Exit:
	    app().exit();
	    break;

	  case c_SaveConfig:
	  {
	    if (_menuCmd.getValue(c_ContinuousShot))
		stopContinuousShot();

	    ofstream	out(_cameras.configFile().c_str());
	    if (out)
	    {
		out << _cameras.size() << endl;
		for (size_t i = 0; i < _cameras.size(); ++i)
		    out << _cameras[i]->dev() << ' ' << *_cameras[i];
	    }

	    if (_menuCmd.getValue(c_ContinuousShot))
		continuousShot();
	  }
	    break;
	  
	  case c_BGR24:
	  case c_RGB24:
	  case c_BGR32:
	  case c_RGB32:
	  case c_GREY:
	  case c_Y16:
	  case c_YUYV:
	  case c_UYVY:
	  case c_SBGGR8:
	  case c_SGBRG8:
	  case c_SGRBG8:
	  {
	    V4L2Camera::PixelFormat
		pixelFormat = V4L2Camera::uintToPixelFormat(id);
	    const V4L2Camera::FrameSize&
		frameSize = _cameras[0]->availableFrameSizes(pixelFormat)
					.first[val];
	    size_t	w = frameSize.width.max, h = frameSize.height.max;
	    V4L2Camera::FrameRateRange
			frameRates = frameSize.availableFrameRates();
	    const V4L2Camera::FrameRate&	frameRate = *frameRates.first;
	    u_int	fps_n = frameRate.fps_n.min,
			fps_d = frameRate.fps_d.max;
	    for (size_t i = 0; i < _cameras.size(); ++i)
		_cameras[i]->setFormat(pixelFormat, w, h, fps_n, fps_d);
	    _captureAndSave.setFormat(_cameras);
	  }
	    break;

	  case c_Brightness:
	  case c_Brightness_Auto:
	  case c_Contrast:
	  case c_Gain:
	  case c_Saturation:
	  case c_Hue:
	  case c_Hue_Auto:
	  case c_Gamma:
	  case c_Sharpness:
	  case c_White_Balance_Temperature:
	  case c_White_Balance_Auto:
	  case c_Backlight_Compensation:
	  case c_Power_Frequency:
	  case c_Exposure_Auto:
	  case c_Exposure_Auto_Priority:
	  case c_Exposure_Absolute:
	  case c_Focus_Absolute:
	  case c_Focus_Relative:
	  case c_Focus_Auto:
	  case c_Zomm_Absolute:
	  case c_Zoom_Relative:
	  case c_Zoom_Continuous:
#ifdef V4L2_CID_IRIS_ABSOLUTE
	  case c_Iris_Absolute:
#endif
#ifdef V4L2_CID_IRIS_RELATIVE
	  case c_Iris_Relative:
#endif
	  case c_Pan_Absolute:
	  case c_Pan_Relative:
	  case c_Pan_Reset:
	  case c_Tilt_Absolute:
	  case c_Tilt_Relative:
	  case c_Tilt_Reset:
	  case c_Cid_Private0:
	  case c_Cid_Private1:
	  case c_Cid_Private2:
	  case c_Cid_Private3:
	  case c_Cid_Private4:
	  case c_Cid_Private5:
	  case c_Cid_Private6:
	  case c_Cid_Private7:
	  case c_Cid_Private8:
	  case c_Cid_Private9:
	  case c_Cid_Private10:
	  case c_Cid_Private11:
	  case c_Cid_Private12:
	  case c_Cid_Private13:
	  case c_Cid_Private14:
	  case c_Cid_Private15:
	  case c_Cid_Private16:
	  case c_Cid_Private17:
	  case c_Cid_Private18:
	  case c_Cid_Private19:
	  case c_Cid_Private20:
	  case c_Cid_Private21:
	  case c_Cid_Private22:
	  case c_Cid_Private23:
	  case c_Cid_Private24:
	  case c_Cid_Private25:
	  case c_Cid_Private26:
	  case c_Cid_Private27:
	  case c_Cid_Private28:
	  case c_Cid_Private29:
	  {
	    const size_t	n = _featureCmd.getValue(c_CameraChoice);
	    if (n < _cameras.size())  
		_cameras[n]->setValue(V4L2Camera::uintToFeature(id), val);
	    else
		for (size_t i = 0; i < _cameras.size(); ++i)
		    _cameras[i]->setValue(V4L2Camera::uintToFeature(id), val);
	  }
	    break;

	  case c_CameraChoice:
	  {
	    const size_t	n = (val < _cameras.size() ? size_t(val) : 0);
	    refreshFeatureCmds(*_cameras[n], _featureCmd);
	  }
	    break;
	  
	  case c_ContinuousShot:
	    if (val)
		continuousShot();
	    else
		stopContinuousShot();
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what();
    }
}

void
MyCmdWindow::tick()
{
    if (!active)
	app().exit();
    
    static int			nframes = 0;
    static struct timeval	start;
    countTime(nframes, start);

    _captureAndSave(std::cout);
}

void
MyCmdWindow::continuousShot()
{
    for (size_t i = 0; i < _cameras.size(); ++i)
	_cameras[i]->continuousShot();
    _timer.start(1);
}

void
MyCmdWindow::stopContinuousShot()
{
    _timer.stop();
    for (size_t i = 0; i < _cameras.size(); ++i)
	_cameras[i]->stopContinuousShot();
}

}
}
