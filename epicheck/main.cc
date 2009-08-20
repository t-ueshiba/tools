/*
 *  $Id: main.cc,v 1.4 2009-08-20 00:55:09 ueshiba Exp $
 */
#include <fstream>
#include <exception>
#include <unistd.h>
#include "TU/v/App.h"
#include "epicheck.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
template <class T> static void
restoreImages(std::istream& in, Array<Image<T> >& images)
{
    static u_int	n = 0;

    Image<T>	image;
    if (!image.restore(in))
	images.resize(n);
    else
    {
	++n;
	restoreImages(in, images);
	images[--n] = image;
    }
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
    
    void		usage(const char*);
    char		*imagefile = 0, *pairfile = 0;
    u_int		lineWidth = 1, ncol = 2, mul = 1, div = 1;
    extern char		*optarg;

  /* Parse command line */
    v::App	vapp(argc, argv);
    for (int c; (c = getopt(argc, argv, "i:j:w:n:HQh")) != EOF; )
	switch (c)
	{
	  case 'i':
	    imagefile = optarg;
	    break;
	  case 'j':
	    pairfile = optarg;
	    break;
	  case 'w':
	    lineWidth = atoi(optarg);
	    break;
	  case 'n':
	    ncol = atoi(optarg);
	    break;
	  case 'H':
	    mul = 1;
	    div = 2;
	    break;
	  case 'Q':
	    mul = 1;
	    div = 4;
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

    try
    {
      /* Read image file. */
	Array<Image<u_char> >	tmpImages;
	if (imagefile != 0)
	{
	    ifstream	in(imagefile);
	    if (!in)
		throw runtime_error("Cannot open the input image file!!");
	    restoreImages(in, tmpImages);
	}
	else
	    restoreImages(cin, tmpImages);

      /* Read pair file. */
	Array2<Array<Point2d> >	tmpPairs;
	if (pairfile != 0)
	{
	    ifstream	in(pairfile);
	    if (!in)
		throw runtime_error("Cannot open the input pair file!!");
	    in >> tmpPairs;
	    if (tmpPairs.ncol() != tmpImages.dim())
		throw runtime_error("# of frames in pair file is not equal to # of epbms!!");
	}

      /* Select views. */
	Array<Image<u_char> >	images;
	Array2<Array<Point2d> >	pairs;
	extern int		optind;
	if (optind == argc)
	{
	    images = tmpImages;
	    pairs  = tmpPairs;
	}
	else
	{
	    images.resize(argc - optind);
	    pairs.resize(tmpPairs.nrow(), images.dim());
	    for (u_int i = 0; i < images.dim(); ++i)
	    {
		u_int	n = atoi(argv[optind + i]);
		if (n >= tmpImages.dim())
		    throw runtime_error("Specified view is not found in input images!!");
		images[i] = tmpImages[n];
		for (u_int j = 0; j < pairs.nrow(); ++j)
		    pairs[j][i] = tmpPairs[j][n];
	    }
	}

      /* Main job */
	v::MyCmdWindow	myWin(vapp, images, pairs, lineWidth, ncol, mul, div);
	vapp.run();
    }
    catch (std::exception& err)
    {
	cerr << err.what() << endl;
    }

    return 0;
}
