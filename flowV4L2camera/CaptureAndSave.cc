/*
 *  $Id$
 */
#include "CaptureAndSave.h"

namespace TU
{
/************************************************************************
*  CaptureAndSave							*
************************************************************************/
void
CaptureAndSave::setFormat(V4L2Camera& camera)
{
    delete _kernel;
    _kernel = 0;
    
    switch (camera.pixelFormat())
    {
      case V4L2Camera::GREY:
	_kernel = new Kernel<u_char>(camera);
	break;
      case V4L2Camera::YUYV:
	_kernel = new Kernel<YUYV422>(camera);
	break;
      case V4L2Camera::UYVY:
	_kernel = new Kernel<YUV422>(camera);
	break;
      case V4L2Camera::RGB24:
	_kernel = new Kernel<RGB>(camera);
	break;
      case V4L2Camera::BGR24:
	_kernel = new Kernel<BGR>(camera);
	break;
      case V4L2Camera::RGB32:
	_kernel = new Kernel<RGBA>(camera);
	break;
      case V4L2Camera::BGR32:
	_kernel = new Kernel<ABGR>(camera);
	break;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
	break;
    }
}
    
}
