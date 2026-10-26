
#include <string>
#include <stdexcept>
#include <cuda.h>
#include "CfdSolver/cuda/cuda_runtime_check.hpp"

void _ensureCudaOk(cudaError_t result, const char *expression, const char *callSiteText) {
    if (result != cudaSuccess) {
        const char* errorText = cudaGetErrorString(result);
        throw std::runtime_error(
                std::string() + callSiteText + ": Expression '" + expression + "' failed with error '"
                + errorText + "'.");
    }
}
