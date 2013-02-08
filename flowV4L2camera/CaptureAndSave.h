/*
 *  $Id$
 */
#include <iostream>
#include "TU/V4L2++.h"

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
	virtual std::ostream&	saveHeader(std::ostream& out)	const	= 0;
	virtual std::ostream&	operator ()(std::ostream& out)		= 0;
    };

    template <class T>
    class Kernel : public KernelBase
    {
      public:
	Kernel(V4L2Camera& camera)					;
	~Kernel()							{}
	
	virtual std::ostream&	saveHeader(std::ostream& out)	const	;
	virtual std::ostream&	operator ()(std::ostream& out)		;
	
      private:
	V4L2Camera&	_camera;
	Image<T>	_image;
    };
    
  public:
    CaptureAndSave(V4L2Camera& camera)
	:_kernel(0)				{ setFormat(camera); }
    ~CaptureAndSave()				{ delete _kernel; }
    
    void		setFormat(V4L2Camera& camera)			;
    std::ostream&	saveHeader(std::ostream& out) const
			{
			    return _kernel->saveHeader(out);
			}
    std::ostream&	operator ()(std::ostream& out) const
			{
			    return (*_kernel)(out);
			}
    
  private:
    mutable KernelBase*		_kernel;
};

template <class T>
CaptureAndSave::Kernel<T>::Kernel(V4L2Camera& camera)
    :_camera(camera), _image(_camera.width(), _camera.height())
{
}
    
template <class T> std::ostream&
CaptureAndSave::Kernel<T>::saveHeader(std::ostream& out) const
{
    out << 'M' << 1 << std::endl;
    _image.saveHeader(out);

    return out;
}
    
template <class T> std::ostream&
CaptureAndSave::Kernel<T>::operator ()(std::ostream& out)
{
    _camera.snap() >> _image;
    _image.saveData(out);

    return out;
}
    
}
