/*
 *  $Id: MyCmdWindow.cc,v 1.7 2012-06-30 20:00:51 ueshiba Exp $
 */
#include "flow1394camera.h"
#include "MyCmdWindow.h"
#include "MyModalDialog.h"
#include <iomanip>

namespace TU
{
namespace v
{
void			refreshFeatureCmds(const Ieee1394Camera& camera,
					   CmdPane& cmdPane)		;
Ieee1394Camera::Feature	id2feature(v::CmdId id)				;
    
/************************************************************************
*  class MyCmdWindow<Ieee1394CameraArray>				*
************************************************************************/
template <> void
MyCmdWindow<Ieee1394CameraArray>::callback(CmdId id, CmdVal val)
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
	    stopContinuousShot();

	    ofstream	out(_cameras.configFile().c_str());
	    if (out)
	    {
		out << _cameras[0]->delay() << ' ' << _cameras.size() << endl;
		for (size_t i = 0; i < _cameras.size(); ++i)
		    out << "0x" << setw(16) << setfill('0')
			<< hex << _cameras[i]->globalUniqueId() << ' '
			<< dec << *_cameras[i];
	    }

	    continuousShot();
	  }
	    break;
      
	  case c_YUV444_160x120:
	  case c_YUV422_320x240:
	  case c_YUV411_640x480:
	  case c_YUV422_640x480:
	  case c_RGB24_640x480:
	  case c_MONO8_640x480:
	  case c_MONO16_640x480:
	  case c_YUV422_800x600:
	  case c_RGB24_800x600:
	  case c_MONO8_800x600:
	  case c_YUV422_1024x768:
	  case c_RGB24_1024x768:
	  case c_MONO8_1024x768:
	  case c_MONO16_800x600:
	  case c_MONO16_1024x768:
	  case c_YUV422_1280x960:
	  case c_RGB24_1280x960:
	  case c_MONO8_1280x960:
	  case c_YUV422_1600x1200:
	  case c_RGB24_1600x1200:
	  case c_MONO8_1600x1200:
	  case c_MONO16_1280x960:
	  case c_MONO16_1600x1200:
	    _cameras.exec(&Ieee1394Camera::setFormatAndFrameRate,
			  Ieee1394Camera::uintToFormat(id),
			  Ieee1394Camera::uintToFrameRate(val));
	    _captureAndSave.setFormat(_cameras);
	    break;

	  case c_Format_7_0:
	  case c_Format_7_1:
	  case c_Format_7_2:
	  case c_Format_7_3:
	  case c_Format_7_4:
	  case c_Format_7_5:
	  case c_Format_7_6:
	  case c_Format_7_7:
	  {
	    Ieee1394Camera::Format
			format7 = Ieee1394Camera::uintToFormat(id);
	    Ieee1394Camera::Format_7_Info
			fmt7info = _cameras[0]->getFormat_7_Info(format7);
	    MyModalDialog	modalDialog(*this, fmt7info);
	    u_int		u0, v0, width, height;
	    Ieee1394Camera::PixelFormat
		pixelFormat = modalDialog.getROI(u0, v0, width, height);
	    for (size_t i = 0; i < _cameras.size(); ++i)
		_cameras[i]->setFormat_7_ROI(format7, u0, v0, width, height)
		  .setFormat_7_PixelFormat(format7, pixelFormat)
		  .setFormatAndFrameRate(format7,
					 Ieee1394Camera::uintToFrameRate(val));
	    _captureAndSave.setFormat(_cameras);
	  }
	    break;
	
	  case c_Brightness:
	  case c_AutoExposure:
	  case c_Sharpness:
	  case c_Hue:
	  case c_Saturation:
	  case c_Gamma:
	  case c_Shutter:
	  case c_Gain:
	  case c_Iris:
	  case c_Focus:
	  case c_Zoom:
	  {
	    const size_t	n = _featureCmd.getValue(c_CameraChoice);
	    if (n < _cameras.size())  
		_cameras[n]->setValue(id2feature(id), val);
	    else
		_cameras.exec(&Ieee1394Camera::setValue,
			      id2feature(id), u_int(val));
	  }
	    break;
      
	  case c_WhiteBalance_UB:
	  {
	    const size_t	n = _featureCmd.getValue(c_CameraChoice);
	    if (n < _cameras.size())  
		_cameras[n]->setWhiteBalance(
		    val, _featureCmd.getValue(c_WhiteBalance_VR));
	    else
		_cameras.exec(&Ieee1394Camera::setWhiteBalance, 
			      u_int(val),
			      u_int(_featureCmd.getValue(c_WhiteBalance_VR)));
	  }
	    break;
	    
	  case c_WhiteBalance_VR:
	  {
	    const size_t	n = _featureCmd.getValue(c_CameraChoice);
	    if (n < _cameras.size())  
		_cameras[n]->setWhiteBalance(
		    _featureCmd.getValue(c_WhiteBalance_UB), val);
	    else
		_cameras.exec(&Ieee1394Camera::setWhiteBalance, 
			      u_int(_featureCmd.getValue(c_WhiteBalance_UB)),
			      u_int(val));
	  }
	    break;
      
	  case c_Brightness	 + OFFSET_ONOFF:
	  case c_AutoExposure    + OFFSET_ONOFF:
	  case c_Sharpness	 + OFFSET_ONOFF:
	  case c_WhiteBalance_UB + OFFSET_ONOFF:
	  case c_WhiteBalance_VR + OFFSET_ONOFF:
	  case c_Hue		 + OFFSET_ONOFF:
	  case c_Saturation	 + OFFSET_ONOFF:
	  case c_Gamma		 + OFFSET_ONOFF:
	  case c_Shutter	 + OFFSET_ONOFF:
	  case c_Gain		 + OFFSET_ONOFF:
	  case c_Iris		 + OFFSET_ONOFF:
	  case c_Focus		 + OFFSET_ONOFF:
	  case c_Zoom		 + OFFSET_ONOFF:
	  {
	    Ieee1394Camera::Feature feature = id2feature(id - OFFSET_ONOFF);
	    const size_t	    n = _featureCmd.getValue(c_CameraChoice);
	    if (val)
	    {
		if (n < _cameras.size())  
		    _cameras[n]->turnOn(feature);
		else
		    _cameras.exec(&Ieee1394Camera::turnOn, feature);
	    }
	    else
	    {
		if (n < _cameras.size())  
		    _cameras[n]->turnOff(feature);
		else
		    _cameras.exec(&Ieee1394Camera::turnOff, feature);
	    }
	  }
	    break;
      
	  case c_Brightness	 + OFFSET_AUTO:
	  case c_AutoExposure    + OFFSET_AUTO:
	  case c_Sharpness	 + OFFSET_AUTO:
	  case c_WhiteBalance_UB + OFFSET_AUTO:
	  case c_WhiteBalance_VR + OFFSET_AUTO:
	  case c_Hue		 + OFFSET_AUTO:
	  case c_Saturation	 + OFFSET_AUTO:
	  case c_Gamma		 + OFFSET_AUTO:
	  case c_Shutter	 + OFFSET_AUTO:
	  case c_Gain		 + OFFSET_AUTO:
	  case c_Iris		 + OFFSET_AUTO:
	  case c_Focus		 + OFFSET_AUTO:
	  case c_Zoom		 + OFFSET_AUTO:
	  {
	    Ieee1394Camera::Feature feature = id2feature(id - OFFSET_AUTO);
	    const size_t	    n = _featureCmd.getValue(c_CameraChoice);
	    if (val)
	    {
		if (n < _cameras.size())
		    _cameras[n]->setAutoMode(feature);
		else
		    _cameras.exec(&Ieee1394Camera::setAutoMode, feature);
	    }
	    else
	    {
		if (n < _cameras.size())
		{
		    _cameras[n]->setManualMode(feature);
		    if (feature == Ieee1394Camera::WHITE_BALANCE)
			_cameras[n]->setWhiteBalance(
			    _featureCmd.getValue(c_WhiteBalance_UB),
			    _featureCmd.getValue(c_WhiteBalance_VR));
		    else
			_cameras[n]->setValue(
			    feature, _featureCmd.getValue(id - OFFSET_AUTO));
		}
		else
		{
		    _cameras.exec(&Ieee1394Camera::setManualMode, feature);
		    if (feature == Ieee1394Camera::WHITE_BALANCE)
			_cameras.exec(&Ieee1394Camera::setWhiteBalance,
				      u_int(_featureCmd
					    .getValue(c_WhiteBalance_UB)),
				      u_int(_featureCmd
					    .getValue(c_WhiteBalance_VR)));
		    else
			_cameras.exec(&Ieee1394Camera::setValue,
				      feature,
				      u_int(_featureCmd
					    .getValue(id - OFFSET_AUTO)));
		}
	    }
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
	
	  case c_OneShot:
	    if (_menuCmd.getValue(c_ContinuousShot))
	    {
		stopContinuousShot();
		_menuCmd.setValue(c_ContinuousShot, false);
	    }
	    _cameras.exec(&Ieee1394Camera::oneShot);
	    tick();
	    break;
	}
    }
    catch (exception& err)
    {
	cerr << err.what();
    }
}

}
}
