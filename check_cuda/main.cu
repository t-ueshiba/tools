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

    const float		min_cc = 3.0;
    if ((dP.major + 0.1*dP.minor) < min_cc)
    {
	std::cerr << "Compute Capability " << min_cc
		  << " or higher required: " << dP.major << '.' << dP.minor
		  << " found."
		  << std::endl;

	return 1; /* Failure */
    }

    std::cout << dP.major << dP.minor;

    return 0;
}
