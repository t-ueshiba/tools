/*
 *  $Id: rfctrl.h,v 1.1.1.1 2012-09-15 08:03:09 ueshiba Exp $
 */
#include "TU/v/TUv++.h"
#include "TU/V4L2++.h"

/************************************************************************
*  global data and definitions						*
************************************************************************/
namespace TU
{
namespace v
{
enum
{
  // File menu
    c_RestoreConfig,
    c_SaveConfig,

  // Camera video format.
    c_PixelFormat,
    c_BGR24			= V4L2Camera::BGR24,
    c_RGB24			= V4L2Camera::RGB24,
    c_BGR32			= V4L2Camera::BGR32,
    c_RGB32			= V4L2Camera::RGB32,
    c_GREY			= V4L2Camera::GREY,
    c_Y16			= V4L2Camera::Y16,
    c_YUYV			= V4L2Camera::YUYV,
    c_UYVY			= V4L2Camera::UYVY,
    c_SBGGR8			= V4L2Camera::SBGGR8,
    c_SGBRG8			= V4L2Camera::SGBRG8,
    c_SGRBG8			= V4L2Camera::SGRBG8,

  // Camera control.
    c_ContinuousShot,

  // Camera features.
    c_Brightness		= V4L2Camera::BRIGHTNESS,
    c_Brightness_Auto		= V4L2Camera::BRIGHTNESS_AUTO,
    c_Contrast			= V4L2Camera::CONTRAST,
    c_Gain			= V4L2Camera::GAIN,
    c_Saturation		= V4L2Camera::SATURATION,
    c_Hue			= V4L2Camera::HUE,
    c_Hue_Auto			= V4L2Camera::HUE_AUTO,
    c_Gamma			= V4L2Camera::GAMMA,
    c_Sharpness			= V4L2Camera::SHARPNESS,
    c_White_Balance_Temperature	= V4L2Camera::WHITE_BALANCE_TEMPERATURE,
    c_White_Balance_Auto	= V4L2Camera::WHITE_BALANCE_AUTO,
    c_Backlight_Compensation	= V4L2Camera::BACKLIGHT_COMPENSATION,
    c_Power_Frequency		= V4L2Camera::POWER_LINE_FREQUENCY,
    c_Exposure_Auto		= V4L2Camera::EXPOSURE_AUTO,
    c_Exposure_Auto_Priority	= V4L2Camera::EXPOSURE_AUTO_PRIORITY,
    c_Exposure_Absolute		= V4L2Camera::EXPOSURE_ABSOLUTE,
    c_Focus_Absolute		= V4L2Camera::FOCUS_ABSOLUTE,
    c_Focus_Relative		= V4L2Camera::FOCUS_RELATIVE,
    c_Focus_Auto		= V4L2Camera::FOCUS_AUTO,
    c_Zomm_Absolute		= V4L2Camera::ZOOM_ABSOLUTE,
    c_Zoom_Relative		= V4L2Camera::ZOOM_RELATIVE,
    c_Zoom_Continuous		= V4L2Camera::ZOOM_CONTINUOUS,
#ifdef V4L2_CID_IRIS_ABSOLUTE
    c_Iris_Absolute		= V4L2Camera::IRIS_ABSOLUTE,
#endif
#ifdef V4L2_CID_IRIS_RELATIVE
    c_Iris_Relative		= V4L2Camera::IRIS_RELATIVE,
#endif
    c_Pan_Absolute		= V4L2Camera::PAN_ABSOLUTE,
    c_Pan_Relative		= V4L2Camera::PAN_RELATIVE,
    c_Pan_Reset			= V4L2Camera::PAN_RESET,
    c_Tilt_Absolute		= V4L2Camera::TILT_ABSOLUTE,
    c_Tilt_Relative		= V4L2Camera::TILT_RELATIVE,
    c_Tilt_Reset		= V4L2Camera::TILT_RESET,
};
}
}
