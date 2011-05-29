/*
 *  $Id: main.cc,v 1.1 2011-05-29 23:53:33 ueshiba Exp $
 */
#include <unistd.h>
#ifdef WIN32
#  include <io.h>
#  include <fcntl.h>
#endif
#include "Rectify.h"

#ifndef EOF
#  define EOF	(-1)
#endif

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nRotate images by 90 deg.\n"
	 << endl;
    cerr << " Usage: " << s << " [-l] [-r]\n"
	 << endl;
    cerr << " options.\n"
	 << "  -l:        rotate left\n"
	 << "  -r:        rotate right\n"
	 << "  -h:        print this\n"
	 << endl;
}

template <class T> Image<T>
rotateLeft(const Image<T>& image)
{
    Image<T>	rotated(image.height(), image.width());
    const u_int	w = image.width() - 1;
    for (u_int v = 0; v < rotated.height(); ++v)
	for (u_int u = 0; u < rotated.width(); ++u)
	    rotated[v][u] = image[u][w-v];
    
    Matrix33d	H;
    H[0][1] = H[2][2] = 1.0;
    H[1][0] = -1.0;
    H[1][2] = w;
    rotated.P  = H * image.P;
    rotated.d1 = image.d1;
    rotated.d2 = image.d2;

    return rotated;
}
    
template <class T> Image<T>
rotateRight(const Image<T>& image)
{
    Image<T>	rotated(image.height(), image.width());
    const u_int	h = image.height() - 1;
    for (u_int v = 0; v < rotated.height(); ++v)
	for (u_int u = 0; u < rotated.width(); ++u)
	    rotated[v][u] = image[h-u][v];
    
    Matrix33d	H;
    H[0][1] = -1.0;
    H[0][2] = h;
    H[1][0] = H[2][2] = 1.0;
    rotated.P  = H * image.P;
    rotated.d1 = image.d1;
    rotated.d2 = image.d2;

    return rotated;
}
    
}

/************************************************************************
*  global functions							*
************************************************************************/
int
main(int argc, char* argv[])
{
    using namespace	std;
    using namespace	TU;

    bool	right = true;
    extern char	*optarg;
    extern int	optind;
    for (int c; (c =getopt(argc, argv, "lrh")) != EOF; )
	switch (c)
	{
	  case 'l':
	    right = false;
	    break;
	  case 'r':
	    right = true;
	    break;
	    
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

    try
    {
#ifdef WIN32
	if (_setmode(_fileno(stdin), _O_BINARY) == -1)
	    throw runtime_error("Cannot set stdin to binary mode!!"); 
	if (_setmode(_fileno(stdout), _O_BINARY) == -1)
	    throw runtime_error("Cannot set stdout to binary mode!!"); 
#endif
	for (Image<u_char> image; image.restore(cin); )
	{
	    if (right)
		rotateRight(image).save(cout);
	    else
		rotateLeft(image).save(cout);
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }

    return 0;
}
