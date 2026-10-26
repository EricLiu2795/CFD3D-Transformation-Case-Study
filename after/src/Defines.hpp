
#ifndef FLUID3D_DEFINES_HPP
#define FLUID3D_DEFINES_HPP

#include <glm/vec3.hpp>

#define FP_DOUBLE

typedef unsigned int FlagType;

enum LinearSystemSolverType {
    LINEAR_SOLVER_JACOBI, POISSON_RELAX, POISSON_GAUSS_SEIDEL, POISSON_RELAX_PARALLEL,
    LINEAR_SOLVER_GAUSS_SEIDEL_PARALLEL
};


#ifdef FP_FLOAT
typedef float Real;
#define stringToFp std::stof
#define nc_put_var_fp nc_put_var_float
#define nc_put_vara_fp nc_put_vara_float
#define nc_put_var1_fp nc_put_var1_float
#define NC_FP 5
#endif
#ifdef FP_DOUBLE
typedef double Real;
#define stringToFp std::stod
#define nc_put_var_fp nc_put_var_double
#define nc_put_vara_fp nc_put_vara_double
#define nc_put_var1_fp nc_put_var1_double
#define NC_FP 6
#endif

typedef glm::vec<3, Real, glm::defaultp> fpvec3;

#define SQR(x) ((x)*(x))

#define LINU(i,j,k) ((i)*(jmax+2)*(kmax+2) + (j)*(kmax+2) + (k))
#define LINV(i,j,k) ((i)*(jmax+1)*(kmax+2) + (j)*(kmax+2) + (k))
#define LINW(i,j,k) ((i)*(jmax+2)*(kmax+1) + (j)*(kmax+1) + (k))
#define LINP(i,j,k) ((i)*(jmax+2)*(kmax+2) + (j)*(kmax+2) + (k))
#define LINT(i,j,k) ((i)*(jmax+2)*(kmax+2) + (j)*(kmax+2) + (k))
#define LINF(i,j,k) ((i)*(jmax+1)*(kmax+1) + (j)*(kmax+1) + (k))
#define LING(i,j,k) ((i)*(jmax+1)*(kmax+1) + (j)*(kmax+1) + (k))
#define LINH(i,j,k) ((i)*(jmax+1)*(kmax+1) + (j)*(kmax+1) + (k))
#define LINRS(i,j,k) ((i)*(jmax+1)*(kmax+1) + (j)*(kmax+1) + (k))
#define LINFLAG(i,j,k) ((i)*(jmax+2)*(kmax+2) + (j)*(kmax+2) + (k))


template <typename T, typename U>
inline T divCeil(T x, U y) { return (x - 1) / static_cast<T>(y) + 1; }

#define SUPPRESS_UNUSED(x) ((void)(x))

#endif
