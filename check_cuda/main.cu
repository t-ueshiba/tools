#include <iostream>

int
main()
{
    cudaDeviceProp	dP;
    const auto		rc = cudaGetDeviceProperties(&dP, 0);
    if (rc != cudaSuccess)
    {
        const auto	error = cudaGetLastError();
	std::cerr << "CUDA error: " << cudaGetErrorString(error) << std::endl;

	return rc; /* Failure */
    }

    const auto	min_cc = 4.0;
    const auto	arch = 10*dP.major + dP.minor;
    if (0.1*arch < min_cc)
    {
	std::cerr << "Compute Capability " << min_cc
		  << " or higher required: " << dP.major << '.' << dP.minor
		  << " found."
		  << std::endl;

	return -1; /* Failure */
    }

    std::cout << arch;

    return 0;
}
