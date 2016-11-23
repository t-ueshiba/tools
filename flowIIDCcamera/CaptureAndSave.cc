/*
 *  $Id$
 */
#include "TU/IIDC++.h"
#include "CaptureAndSave.h"

namespace TU
{
/************************************************************************
*  CaptureAndSave<IIDCCamera>					*
************************************************************************/
template <> void
CaptureAndSave<IIDCCamera>::setFormat(Array<IIDCCamera>& cameras)
{
    delete _kernel;
    _kernel = 0;
    
    switch (cameras[0].pixelFormat())
    {
      case IIDCCamera::MONO_8:
	_kernel = new Kernel<u_char>(cameras);
	break;
      case IIDCCamera::YUV_411:
	_kernel = new Kernel<YUV411>(cameras);
	break;
      case IIDCCamera::YUV_422:
	_kernel = new Kernel<YUV422>(cameras);
	break;
      case IIDCCamera::YUV_444:
	_kernel = new Kernel<YUV444>(cameras);
	break;
      case IIDCCamera::RGB_24:
	_kernel = new Kernel<RGBA>(cameras);
	break;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
	break;
    }
}
    
}
