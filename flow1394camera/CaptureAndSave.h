/*
 *  $Id$
 */
#include <iterator>
#include <iostream>
#include <boost/foreach.hpp>

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
	virtual std::ostream&	operator ()(std::ostream& out)	const	= 0;
    };

    template <class T>
    class Kernel : public KernelBase
    {
      public:
	Kernel(const Array<CAMERA*>& cameras)				;
	virtual ~Kernel()						{}
	
	virtual std::ostream&	saveHeaders(std::ostream& out)	const	;
	virtual std::ostream&	operator ()(std::ostream& out)	const	;
	
      private:
	const Array<CAMERA*>&		_cameras;
	mutable Array<Image<T> >	_images;
    };
    
  public:
    CaptureAndSave(const Array<CAMERA*>& cameras)
	:_kernel(0)				{ setFormat(cameras); }
    ~CaptureAndSave()				{ delete _kernel; }
    
    void		setFormat(const Array<CAMERA*>& cameras)	;
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
CaptureAndSave<CAMERA>::Kernel<T>::Kernel(const Array<CAMERA*>& cameras)
    :_cameras(cameras), _images(_cameras.size())
{
    typename Array<Image<T> >::iterator	image = _images.begin();
    BOOST_FOREACH (const camera_type* camera, _cameras)
    {
	image->resize(camera->height(), camera->width());
	++image;
    }
}
    
template <class CAMERA> template <class T> std::ostream&
CaptureAndSave<CAMERA>::Kernel<T>::saveHeaders(std::ostream& out) const
{
    out << 'M' << _images.size() << std::endl;
    BOOST_FOREACH (const Image<T>& image, _images)
	image.saveHeader(out);

    return out;
}
    
template <class CAMERA> template <class T> std::ostream&
CaptureAndSave<CAMERA>::Kernel<T>::operator ()(std::ostream& out) const
{
    exec(_cameras, &camera_type::snap);
    typename Array<Image<T> >::iterator	image = _images.begin();
    BOOST_FOREACH (const camera_type* camera, _cameras)
    {
	*camera >> *image;
	++image;
    }
    BOOST_FOREACH (const Image<T>& image, _images)
	image.saveData(out);

    return out;
}
    
}
