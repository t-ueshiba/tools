/*
 *  $Id$
 */
#include <iterator>
#include <iostream>
#include <boost/foreach.hpp>

namespace TU
{
/************************************************************************
*  CaptureAndSave<CAMERAS>						*
************************************************************************/
template <class CAMERAS>
class CaptureAndSave
{
  public:
    typedef typename std::iterator_traits<
		typename CAMERAS::value_type>::value_type	camera_type;

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
	Kernel(const CAMERAS& cameras)					;
	virtual ~Kernel()						{}
	
	virtual std::ostream&	saveHeaders(std::ostream& out)	const	;
	virtual std::ostream&	operator ()(std::ostream& out)	const	;
	
      private:
	const CAMERAS&			_cameras;
	mutable Array<Image<T> >	_images;
    };
    
  public:
    CaptureAndSave(const CAMERAS& cameras)
	:_kernel(0)				{ setFormat(cameras); }
    ~CaptureAndSave()				{ delete _kernel; }
    
    void		setFormat(const CAMERAS& cameras)		;
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

template <class CAMERAS> template <class T>
CaptureAndSave<CAMERAS>::Kernel<T>::Kernel(const CAMERAS& cameras)
    :_cameras(cameras), _images(_cameras.size())
{
    typename Array<Image<T> >::iterator	image = _images.begin();
    BOOST_FOREACH (const camera_type* camera, _cameras)
    {
	image->resize(camera->height(), camera->width());
	++image;
    }
}
    
template <class CAMERAS> template <class T> std::ostream&
CaptureAndSave<CAMERAS>::Kernel<T>::saveHeaders(std::ostream& out) const
{
    out << 'M' << _images.size() << std::endl;
    BOOST_FOREACH (const Image<T>& image, _images)
	image.saveHeader(out);

    return out;
}
    
template <class CAMERAS> template <class T> std::ostream&
CaptureAndSave<CAMERAS>::Kernel<T>::operator ()(std::ostream& out) const
{
    _cameras.exec(&camera_type::snap);
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
