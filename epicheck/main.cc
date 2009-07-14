/*
 *  $Id: main.cc,v 1.2 2009-07-14 05:26:53 ueshiba Exp $
 */
#include <fstream>
#include <exception>
#include <unistd.h>
#include "TU/v/App.h"
#include "TU/Calib++.h"
#include "epicheck.h"

static const int	NFRAMES_MAX = 10;


int
main(int argc, char* argv[])
{
    using namespace	std;
    using namespace	TU;
    
    void		usage(const char*);
    char		*imagefile = 0, *pairfile = 0;
    u_int		index[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, nframes = 0,
			lineWidth = 1;
    extern char		*optarg;
    extern int		optind;

  /* Parse command line */
    v::App	vapp(argc, argv);
    for (int c; (c = getopt(argc, argv, "i:j:w:0123456789h")) != EOF; )
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
	  case '0':
	  case '1':
	  case '2':
	  case '3':
	  case '4':
	  case '5':
	  case '6':
	  case '7':
	  case '8':
	  case '9':
	    if (nframes < NFRAMES_MAX)
		index[nframes++] = c - '0';
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}

  /* Read image file. */
    Image<u_char>	image[NFRAMES_MAX];
    u_int		nimages = 0;
    if (imagefile != 0)
    {
	ifstream	in(imagefile);
	if (!in)
	{
	    cerr << "Cannot open the input image file (" << imagefile << ")!"
		 << endl;
	    return 1;
	}
	while (nimages < NFRAMES_MAX && image[nimages].restore(in).good())
	    ++nimages;
    }
    else
    {
	while (nimages < NFRAMES_MAX && image[nimages].restore(cin).good())
	    ++nimages;
    }

  /* Read pair file. */
    Matrix<double>	pair;
    if (pairfile != 0)
    {
	ifstream	in(pairfile);
	if (!in)
	{
	    cerr << "Cannot open the input pair file (" << pairfile << ")!"
		 << endl;
	    return 1;
	}

	Matrix<double>	tmp;
	in >> tmp;
	pair.resize(tmp.nrow(), 3*tmp.ncol() / 2);
	if (pair.ncol() != 3*nimages)
	{
	    cerr << "# of frames in pair file is not equal to # of epbms!!"
		 << endl;
	    return 1;
	}
	for (int i = 0; i < pair.nrow(); ++i)
	    for (int j = 0; j < pair.ncol()/3; ++j)
	    {
		pair[i][3*j]	 = tmp[i][2*j];
		pair[i][3*j + 1] = tmp[i][2*j + 1];
		pair[i][3*j + 2] = 1.0;
	    }
    }

  /* Set # of frames and check indices. */
    if (nframes == 0)			// Options -[0-9] are not specified.
	nframes = nimages;
    for (int i = 0; i < nframes; ++i)
	if (index[i] >= nimages)
	{
	    cerr << "Specified image (" << index[i]
		 << ") is not in the EPBM file!!" << endl;
	    return 1;
	}

  /* Main job */
    try
    {
	v::MyCmdWindow	myWin(vapp, image, pair, index, nframes, lineWidth);
	vapp.run();
    }
    catch (std::exception& err)
    {
	cerr << err.what() << endl;
    }

    return 0;
}
