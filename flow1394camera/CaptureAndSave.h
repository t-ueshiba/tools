/*
 *  $Id$
 */
#include <iostream>
#include "TU/Ieee1394CameraArray.h"

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
	Kernel(const Ieee1394CameraArray& cameras)			;
	~Kernel()							{}
	
	virtual std::ostream&	saveHeaders(std::ostream& out)	const	;
	virtual std::ostream&	operator ()(std::ostream& out)		;
	
      private:
	const Ieee1394CameraArray&	_cameras;
	Array<Image<T> >		_images;
    };
    
  public:
    CaptureAndSave(const Ieee1394CameraArray& cameras)
	:_kernel(0)				{ setFormat(cameras); }
    ~CaptureAndSave()				{ delete _kernel; }
    
    void		setFormat(const Ieee1394CameraArray& cameras)	;
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
CaptureAndSave::Kernel<T>::Kernel(const Ieee1394CameraArray& cameras)
    :_cameras(cameras), _images(_cameras.size())
{
    for (u_int i = 0; i < _images.size(); ++i)
	_images[i].resize(_cameras[i]->height(), _cameras[i]->width());
}
    
template <class T> std::ostream&
CaptureAndSave::Kernel<T>::saveHeaders(std::ostream& out) const
{
    out << 'M' << _images.size() << std::endl;
    for (u_int i = 0; i < _images.size(); ++i)
	_images[i].saveHeader(out);

    return out;
}
    
template <class T> std::ostream&
CaptureAndSave::Kernel<T>::operator ()(std::ostream& out)
{
    for (u_int i = 0; i < _cameras.size(); ++i)
	_cameras[i]->snap();			// 撮影
    for (u_int i = 0; i < _cameras.size(); ++i)
	*_cameras[i] >> _images[i];		// カメラからの取り込み
    for (u_int i = 0; i < _images.size(); ++i)
	_images[i].saveData(out);		// 書き出し

    return out;
}
    
}
