/*
 *  $Id: main.cc,v 1.3 2009-09-09 07:18:18 ueshiba Exp $
 */
#include <unistd.h>
#ifdef WIN32
#  include <io.h>
#  include <fcntl.h>
#endif
#include <fstream>
#include "TU/Image++.h"

namespace TU
{
/************************************************************************
*  static functions							*
************************************************************************/
static void
usage(const char* s)
{
    using namespace	std;

    cerr << "\nEmbed caliblation data to the header of epbm files.\n"
	 << endl;
    cerr << " Usage: " << s << " -c calib\n"
	 << endl;
    cerr << " General options.\n"
	 << "  -c calib:       calibration data file. (cannot be ommited)\n"
	 << endl;
    cerr << " Other options.\n"
	 << "  -h:             print this.\n"
	 << endl;
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

    const char*		calibFile = 0;
    extern char*	optarg;
    for (int c; (c = getopt(argc, argv, "c:h")) != EOF; )
	switch (c)
	{
	  case 'c':
	    calibFile = optarg;
	    break;
	  case 'h':
	    usage(argv[0]);
	    return 1;
	}
    if (calibFile == 0)
    {
	cerr << "Calibration file must be specified!" << endl;
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
	ifstream	in(calibFile);

	for (GenericImage image; image.restore(cin); )
	{
	    if (!in)
		break;
	    in >> image.P >> image.d1 >> image.d2;
	    image.save(cout);
	}
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
	return 1;
    }
    
    return 0;
}
