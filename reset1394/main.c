/*
 *  $Id$
 */
#include <stdio.h>
#include <libraw1394/raw1394.h>

int
main()
{
    raw1394handle_t	handle = raw1394_new_handle();
    int			nports = raw1394_get_port_info(handle, NULL, 0);
    int			p;

    raw1394_destroy_handle(handle);

    fprintf(stderr, "%d port(s) found.\n", nports);
    
    for (p = 0; p < nports; ++p)
    {
	fprintf(stderr, "reset port %d...", p);
	
	handle = raw1394_new_handle_on_port(p);
	raw1394_reset_bus(handle);
	raw1394_destroy_handle(handle);

	fprintf(stderr, " done.\n");
    }

    return 0;
}
