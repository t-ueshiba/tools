/*
 *  $Id$
 */
#include <iterator>
#include <iostream>
#include <algorithm>

namespace TU
{
/************************************************************************
*  CaptureAndSave<CAMERA>						*
************************************************************************/
template <class CAMERA>
class CaptureAndSave
{
  public:
    typedef CAMERA	camera_type;

  private:
    class KernelBase
    {
      public:
	virtual ~KernelBase()						{}

	virtual std::ostream&	saveHeaders(std::ostream& out)	const	= 0;
	virtual std::ostream&	operator ()(std::ostream& out)		= 0;
    };

    template <class T>
    class Kernel : public KernelBase
    {
      public:
	Kernel(Array<CAMERA>& cameras)					;
	virtual ~Kernel()						{}
	
	virtual std::ostream&	saveHeaders(std::ostream& out)	const	;
	virtual std::ostream&	operator ()(std::ostream& out)		;
	
      private:
	Array<CAMERA>&			_cameras;
	mutable Array<Image<T> >	_images;
    };
    
  public:
    CaptureAndSave(Array<CAMERA>& cameras)
	:_kernel(0)				{ setFormat(cameras); }
    ~CaptureAndSave()				{ delete _kernel; }
    
    void		setFormat(Array<CAMERA>& cameras)		;
    std::ostream&	saveHeaders(std::ostream& out) const
			{
			    return _kernel->saveHeaders(out);
			}
    std::ostream&	operator ()(std::ostream& out) const
			{
			    return (*_kernel)(out);
			}
    
  private:
    KernelBase*		_kernel;
};

template <class CAMERA> template <class T>
CaptureAndSave<CAMERA>::Kernel<T>::Kernel(Array<CAMERA>& cameras)
    :_cameras(cameras), _images(_cameras.size())
{
    auto	image = _images.begin();
    for (const auto& camera : _cameras)
    {
	image->resize(camera.height(), camera.width());
	++image;
    }
}
    
template <class CAMERA> template <class T> std::ostream&
CaptureAndSave<CAMERA>::Kernel<T>::saveHeaders(std::ostream& out) const
{
    out << 'M' << _images.size() << std::endl;
    for (const auto& image : _images)
	image.saveHeader(out);

    return out;
}
    
template <class CAMERA> template <class T> std::ostream&
CaptureAndSave<CAMERA>::Kernel<T>::operator ()(std::ostream& out)
{
    for (auto& camera : _cameras)
	camera.snap();
    auto	image = _images.begin();
    for (const auto& camera : _cameras)
    {
	camera >> *image;
	++image;
    }
    for (const auto& image : _images)
	image.saveData(out);

    return out;
}
    
}
