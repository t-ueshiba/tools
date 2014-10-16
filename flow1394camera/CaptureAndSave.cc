/*
 *  $Id$
 */
#include "TU/Ieee1394++.h"
#include "CaptureAndSave.h"

namespace TU
{
/************************************************************************
*  CaptureAndSave<Ieee1394Camera>					*
************************************************************************/
template <> void
CaptureAndSave<Ieee1394Camera>::setFormat(const Array<Ieee1394Camera*>& cameras)
{
    delete _kernel;
    _kernel = 0;
    
    switch (cameras[0]->pixelFormat())
    {
      case Ieee1394Camera::MONO_8:
	_kernel = new Kernel<u_char>(cameras);
	break;
      case Ieee1394Camera::YUV_411:
	_kernel = new Kernel<YUV411>(cameras);
	break;
      case Ieee1394Camera::YUV_422:
	_kernel = new Kernel<YUV422>(cameras);
	break;
      case Ieee1394Camera::YUV_444:
	_kernel = new Kernel<YUV444>(cameras);
	break;
      case Ieee1394Camera::RGB_24:
	_kernel = new Kernel<RGBA>(cameras);
	break;
      default:
	throw std::runtime_error("Unsupported pixel format!!");
	break;
    }
}
    
}
