/*
 *  $Id$
 */
#include <iostream>
#include "TU/V4L2CameraArray.h"

namespace TU
{
/************************************************************************
*  CaptureAndSave							*
************************************************************************/
class CaptureAndSave
{
  private:
    class KernelBase
    {
      public:
	virtual std::ostream&	saveHeaders(std::ostream& out)	const	= 0;
	virtual std::ostream&	operator ()(std::ostream& out)		= 0;
    };

    template <class T>
    class Kernel : public KernelBase
    {
      public:
	Kernel(const V4L2CameraArray& cameras)				;
	~Kernel()							{}
	
	virtual std::ostream&	saveHeaders(std::ostream& out)	const	;
	virtual std::ostream&	operator ()(std::ostream& out)		;
	
      private:
	const V4L2CameraArray&	_cameras;
	Array<Image<T> >	_images;
    };
    
  public:
    CaptureAndSave(const V4L2CameraArray& cameras)
	:_kernel(0)				{ setFormat(cameras); }
    ~CaptureAndSave()				{ delete _kernel; }
    
    void		setFormat(const V4L2CameraArray& cameras)	;
    std::ostream&	saveHeaders(std::ostream& out) const
			{
			    return _kernel->saveHeaders(out);
			}
    std::ostream&	operator ()(std::ostream& out) const
			{
			    return (*_kernel)(out);
			}
    
  private:
    mutable KernelBase*		_kernel;
};

template <class T>
CaptureAndSave::Kernel<T>::Kernel(const V4L2CameraArray& cameras)
    :_cameras(cameras), _images(_cameras.size())
{
    for (size_t i = 0; i < _images.size(); ++i)
	_images[i].resize(_cameras[i]->height(), _cameras[i]->width());
}
    
template <class T> std::ostream&
CaptureAndSave::Kernel<T>::saveHeaders(std::ostream& out) const
{
    out << 'M' << _images.size() << std::endl;
    for (size_t i = 0; i < _images.size(); ++i)
	_images[i].saveHeader(out);

    return out;
}
    
template <class T> std::ostream&
CaptureAndSave::Kernel<T>::operator ()(std::ostream& out)
{
    _cameras.exec(&V4L2Camera::snap);
    for (size_t i = 0; i < _cameras.size(); ++i)
	*_cameras[i] >> _images[i];
    for (size_t i = 0; i < _cameras.size(); ++i)
	_images[i].saveData(out);

    return out;
}
    
}
