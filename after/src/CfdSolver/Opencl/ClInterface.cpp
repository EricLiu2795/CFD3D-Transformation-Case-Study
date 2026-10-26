
#include <iostream>
#include <cassert>
#include <fstream>
#include "ClInterface.hpp"

ClInterface::~ClInterface() = default;

void ClInterface::initialize(CLContextConfig contextConfig)
{
    cl::Platform::get(&availablePlatforms);
    if (availablePlatforms.size() <= contextConfig.platformOrdinal) {
        std::cerr << "Fatal Error: No OpenCL platform with specified ID found!" << std::endl;
    	exit(1);
    }
    platform = availablePlatforms.at(contextConfig.platformOrdinal);

	platform.fetchDevices(CL_DEVICE_TYPE_ANY, &availableDevices);
	if(availableDevices.empty()){
        std::cout << "No OpenCL devices found. Please check your installation!" << std::endl;
		exit(1);
	}
	if (contextConfig.useAvailableDevices) {
		devices = availableDevices;
	} else {
		devices = {availableDevices.at(0)};
	}


	context = cl::Context(devices);
}

void ClInterface::reportInfo()
{
    std::cout << "Number of OpenCL platforms: " << availablePlatforms.size() << std::endl;
    std::cout << "Standard OpenCL platform:" << std::endl;
    std::cout << platform.fetchInfo<CL_PLATFORM_LABEL>() << std::endl;
    std::cout << platform.fetchInfo<CL_PLATFORM_RELEASE>() << std::endl << std::endl;
	int i = 0;
	for (cl::Device &device : devices) {
        std::cout << "Using device #" << i << ": " << device.fetchInfo<CL_DEVICE_LABEL>() << std::endl;
		i++;
	}
}


std::string importTextFile(const std::string &filename) {
	std::ifstream ifile(filename.c_str());
	if (!ifile.is_open()) {
		std::cerr << "ERROR in importTextFile: Couldn't open file \"" << filename <<  "\"." << std::endl;
		exit(1);
	}
	std::string kernelText((std::istreambuf_iterator<char>(ifile)), std::istreambuf_iterator<char>());
	ifile.close();
	return kernelText;
}

void ClInterface::assignPrefixHeader(const std::string &prefixHeader)
{
    this->prefixHeader = prefixHeader;
}

cl::Program ClInterface::importProgramFromSourceFile(const char *filename)
{
	return importProgramFromSourceFiles({filename});
}

cl::Program ClInterface::compileKernelFile(const std::string &filename) {
	std::cout << "Compiling " << filename << "..." << std::endl;

	cl::Program::Sources sources;
	std::string kernelText = prefixHeader + "\n" + importTextFile(filename);
	sources.push_back({kernelText.c_str(), kernelText.length()});
	cl::Program program(context, sources);
	std::string compileFlags = "-Werror";
	if (strcmp(platform.fetchInfo<CL_PLATFORM_LABEL>().c_str(), "AMD Accelerated Parallel Processing") == 0) {
		compileFlags = "";
	}
	if (program.compile(compileFlags.c_str()) != CL_SUCCESS) {
        std::cerr << "Error while building " << filename << ":" << std::endl
                << program.fetchCompileInfo<CL_PROGRAM_COMPILE_LOG>(devices[0]) << std::endl;
		exit(1);
	}
	return program;
}

cl::Program ClInterface::importProgramFromSourceFiles(const std::vector<std::string> &filenames)
{
	std::vector<cl::Program> programs;
	for (const std::string& filename : filenames) {
		programs.push_back(compileKernelFile(filename));
	}

	cl_status err = CL_SUCCESS;
	cl::Program combinedProgram = cl::combineProgram(
            programs, nullptr, nullptr, nullptr, &err);
	if (err != CL_SUCCESS) {
		std::cerr << "Error while linking programs:" << std::endl
		        << combinedProgram.fetchCompileInfo<CL_PROGRAM_COMPILE_LOG>(devices[0]) << std::endl;
		exit(1);
	}
	return combinedProgram;
}

cl::Program ClInterface::importProgramFromBinaryFile(const char *filename)
{
    cl::Program::Binaries binaries;
    std::ifstream ifile(filename, std::ifstream::binary);
    if (!ifile.is_open()) {
        std::cerr << "ERROR in importProgramFromBinaryFile: Couldn't open file \"" << filename << "\"." << std::endl;
        exit(1);
    }
    ifile.seekg(0, std::ios::end);
    std::streampos size = ifile.tellg();
    ifile.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    ifile.read((char*)buffer.data(), size);
    ifile.close();
    binaries.push_back(buffer);

    cl::Program program(context, devices, binaries);
    if (program.build(devices) != CL_SUCCESS){
        std::cerr << "Error while builing cl::Program: " << program.fetchCompileInfo<CL_PROGRAM_COMPILE_LOG>(devices[0])
                  << std::endl;
        exit(1);
    }
    return program;
}



cl::GridRange ClInterface::alignedExtent1D(int w, int local) {
	return cl::GridRange(divCeil(w, local)*local);
}

cl::GridRange ClInterface::alignedExtent2D(int w, int h, cl::GridRange local) {
    return cl::GridRange(
            divCeil(w, int(local[0]))*local[0],
            divCeil(h, int(local[1]))*local[1]);
}

cl::GridRange ClInterface::alignedExtent3D(int w, int h, int d, cl::GridRange local) {
    return cl::GridRange(
            divCeil(w, int(local[0]))*local[0],
            divCeil(h, int(local[1]))*local[1],
            divCeil(d, int(local[2]))*local[2]);
}

