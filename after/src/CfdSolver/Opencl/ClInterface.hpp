
#ifndef CLBRIDGE_HPP_
#define CLBRIDGE_HPP_

#define CL_HPP_LOWEST_OPENCL_VERSION 120
#define CL_HPP_CHOSEN_OPENCL_VERSION 120
#define CL_HPP_ENABLE_THROWING

#include <vector>
#include <string>
#include "CL/cl2.hpp"
#include "Singleton.hpp"

struct CLContextConfig {
    size_t platformOrdinal;
    bool useAvailableDevices;

    CLContextConfig(size_t platformOrdinal = 0, bool useAvailableDevices = false)
        : platformOrdinal(platformOrdinal), useAvailableDevices(useAvailableDevices)
    {
    }
};

class ClInterface : public Singleton<ClInterface> {
public:
    virtual ~ClInterface();

	void initialize(CLContextConfig contextConfig = CLContextConfig());
	void reportInfo();

	cl::Program importProgramFromSourceFile(const char *filename);
	cl::Program importProgramFromSourceFiles(const std::vector<std::string> &filenames);
	cl::Program importProgramFromBinaryFile(const char *filename);

	void assignPrefixHeader(const std::string &prefixHeader);

    inline int divCeil(int x, int y) { return (x - 1) / y + 1; }
    cl::GridRange alignedExtent1D(int w, int local);
    cl::GridRange alignedExtent2D(int w, int h, cl::GridRange local);
    cl::GridRange alignedExtent3D(int w, int h, int d, cl::GridRange local);

    inline cl::Context &fetchContext() { return context; }
    inline cl::Platform &fetchPlatform() { return platform; }
    inline std::vector<cl::Device> &fetchDevices() { return devices; }

private:
    cl::Program compileKernelFile(const std::string &filename);

    std::vector<cl::Platform> availablePlatforms;
	std::vector<cl::Device> availableDevices;
    cl::Platform platform;
	std::vector<cl::Device> devices;
    cl::Context context;
    std::string prefixHeader;
};

std::string importTextFile(const std::string &filename);

#endif
