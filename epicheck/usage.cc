/*
 *  $Id: usage.cc,v 1.1.1.1 2008-06-16 03:15:08 ueshiba Exp $
 */
#include <iostream>

void
usage(const char* s)
{
    using namespace	std;
    
    cerr << "\nCheck epipolar constraint on EPBM image pair\n"
	 << endl;
    cerr << " Usage: " << s << " [options] [-i in.epbm] [-j in.pairs]\n"
	 << endl;
    cerr << " I/O options.\n"
	 << "  -i in.epbm:    input EPBM file (default: stdin)\n"
	 << "  -j in.pairs:   input PAIR file for displaying epipolar lines "
	 << " (default: none)\n"
	 << endl;
    cerr << " EPBM pair selection options.\n"
	 << "  -[0123456789]: combination of images"
	 << "  (ex. -01: use 0th and 1st images)\n"
	 << endl;
    cerr << " other options.\n"
	 << "  -w lineWidth:  width of epipolar lines (default: 1)\n"
	 << "  -h:            print this\n"
	 << endl;
}
