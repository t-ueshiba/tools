/*
 *  $Id: main.cc,v 1.1.1.1 2012-09-15 08:03:09 ueshiba Exp $
 */
#include <cstdlib>
#include <stdexcept>
#include "TU/USB++.h"

int
main(int argc, char* argv[])
{
    using namespace	std;
    using namespace	TU;

    uint16_t	idVendor  = 0x2101;
    uint16_t	idProduct = 0x8500;
    int		port	  = 1;
    bool	power	  = false;
    int		led	  = -1;
    for (int c; (c = getopt(argc, argv, "v:p:n:P:L:")) != -1; )
	switch (c)
	{
	  case 'v':
	    idVendor = strtoul(optarg, 0, 0);
	    break;
	  case 'p':
	    idProduct = strtoul(optarg, 0, 0);
	    break;
	  case 'n':
	    port = atoi(optarg);
	    break;
	  case 'P':
	    power = atoi(optarg);
	    break;
	  case 'L':
	    led = atoi(optarg);
	    break;
	}

    try
    {
	if (idVendor == 0 || idProduct == 0 || port < 0)
	{
	    USBHub::listup(cout);
	    throw runtime_error("Please specify idVendor, idProduct and port# by; -v idVendor -p idProduct -n port#");
	}

	USBHub	hub(idVendor, idProduct);
	if (led < 0)
	    hub.setPower(port, power);
	else
	    hub.setLED(port, led);

	USBHub::listup(cerr);
    }
    catch (exception& err)
    {
	cerr << err.what() << endl;
    }

    return 0;
}
