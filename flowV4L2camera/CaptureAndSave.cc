/*
 *  $Id$
 */
#include "TU/V4L2++.h"
#include "CaptureAndSave.h"

namespace TU
{
/************************************************************************
*  CaptureAndSave<V4L2Camera>						*
************************************************************************/
template <> void
CaptureAndSave<V4L2Camera>::setFormat(const Array<V4L2Camera*>& cameras)
{
    delete _kernel;
    _kernel = 0;
    
    switch (cameras[0]->pixelFormat())
    {
      case V4L2Camera::GREY:
	_kernel = new Kernel<u_char>(cameras);
	break;
      case V4L2Camera::YUYV:
	_kernel = new Kernel<YUYV422>(cameras);
	break;
      case V4L2Camera::UYVY:
	_kernel = new Kernel<YUV422>(cameras);
	break;
      case V4L2Camera::RGB24:
	_kernel = new Kernel<RGB>(cameras);
	break;
      case V4L2Camera::BGR24:
	_kernel = new Kernel<BGR>(cameras);
	break;
      case V4L2Camera::RGB32:
	_kernel = new Kernel<RGBA>(cameras);
	break;
      case V4L2Camera::BGR32:
	_kernel = new Kernel<ABGR>(cameras);
	break;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
	break;
    }
}
    
}
