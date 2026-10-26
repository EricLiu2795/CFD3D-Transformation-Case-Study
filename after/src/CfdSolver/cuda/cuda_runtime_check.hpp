
#ifndef FLUID3D_CUDA_RUNTIME_CHECK_HPP
#define FLUID3D_CUDA_RUNTIME_CHECK_HPP

__device__ inline bool isWet(unsigned int flag) { return (flag >> 0) & 1; }
__device__ inline bool isSticky(unsigned int flag) { return (flag >> 1) & 1; }
__device__ inline bool isFrictionless(unsigned int flag) { return (flag >> 2) & 1; }
__device__ inline bool isOutlet(unsigned int flag) { return (flag >> 3) & 1; }
__device__ inline bool isInlet(unsigned int flag) { return (flag >> 4) & 1; }
__device__ inline bool B_L(unsigned int flag) { return (flag >> 5) & 1; }
__device__ inline bool B_R(unsigned int flag) { return (flag >> 6) & 1; }
__device__ inline bool B_D(unsigned int flag) { return (flag >> 7) & 1; }
__device__ inline bool B_U(unsigned int flag) { return (flag >> 8) & 1; }
__device__ inline bool B_B(unsigned int flag) { return (flag >> 9) & 1; }
__device__ inline bool B_F(unsigned int flag) { return (flag >> 10) & 1; }
__device__ inline bool isHeated(unsigned int flag) { return (flag >> 11) & 1; }
__device__ inline bool isChilled(unsigned int flag) { return (flag >> 12) & 1; }

#ifndef EXPAND_STRING
#define MAKE_STRING(x) #x
#define EXPAND_STRING(x) MAKE_STRING(x)
#endif

void _ensureCudaOk(cudaError_t result, const char *expression, const char *callSiteText);
#define ensureCudaOk(expr) _ensureCudaOk(expr, #expr, __FILE__ ":" EXPAND_STRING(__LINE__))

#endif
