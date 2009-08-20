/*
 *  $Id: usage.cc,v 1.2 2009-08-20 00:55:09 ueshiba Exp $
 */
#include <iostream>

void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nCheck epipolar constraint between EPBM images\n"
	 << endl;
    cerr << " Usage: " << s << " [options] [-i epbm] [-j pairs] [<image #> <image #> ...]\n"
	 << endl;
    cerr << " I/O options.\n"
	 << "  -i epbm:      input EPBM file (default: stdin)\n"
	 << "  -j pairs:     input PAIR file for displaying epipolar lines"
	 << " (default: none)\n"
	 << endl;
    cerr << " other options.\n"
	 << "  -w lineWidth: width of epipolar lines (default: 1)\n"
	 << "  -n #images:   #images displayed per row (default: 2)\n"
	 << "  -H:           display with half size\n"
	 << "  -Q:           display with quater size\n"
	 << "  -h:           print this\n"
	 << endl;
}
