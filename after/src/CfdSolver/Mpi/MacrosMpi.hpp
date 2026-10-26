
#ifndef FLUID3D_MACROSMPI_HPP
#define FLUID3D_MACROSMPI_HPP

#ifdef LINU

#undef LINU
#undef LINV
#undef LINW
#undef LINP
#undef LINT
#undef LINF
#undef LING
#undef LINH
#undef LINRS
#undef LINFLAG

#endif

#define LINU_NORMAL(i,j,k) ((i)*(jmax+2)*(kmax+2) + (j)*(kmax+2) + (k))
#define LINV_NORMAL(i,j,k) ((i)*(jmax+1)*(kmax+2) + (j)*(kmax+2) + (k))
#define LINW_NORMAL(i,j,k) ((i)*(jmax+2)*(kmax+1) + (j)*(kmax+1) + (k))
#define LINP_NORMAL(i,j,k) ((i)*(jmax+2)*(kmax+2) + (j)*(kmax+2) + (k))
#define LINT_NORMAL(i,j,k) ((i)*(jmax+2)*(kmax+2) + (j)*(kmax+2) + (k))
#define LINF_NORMAL(i,j,k) ((i)*(jmax+1)*(kmax+1) + (j)*(kmax+1) + (k))
#define LING_NORMAL(i,j,k) ((i)*(jmax+1)*(kmax+1) + (j)*(kmax+1) + (k))
#define LINH_NORMAL(i,j,k) ((i)*(jmax+1)*(kmax+1) + (j)*(kmax+1) + (k))
#define LINRS_NORMAL(i,j,k) ((i)*(jmax+1)*(kmax+1) + (j)*(kmax+1) + (k))
#define LINFLAG_NORMAL(i,j,k) ((i)*(jmax+2)*(kmax+2) + (j)*(kmax+2) + (k))

#define LINU_MPI(i,j,k) (((i) - (il-2))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINV_MPI(i,j,k) (((i) - (il-1))*(ju - jl + 4)*(ku - kl + 3) + ((j) - (jl-2))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINW_MPI(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 4) + ((j) - (jl-1))*(ku - kl + 4) + ((k) - (kl-2)))
#define LINP_MPI(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINT_MPI(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINF_MPI(i,j,k) (((i) - (il-2))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))
#define LING_MPI(i,j,k) (((i) - (il-1))*(ju - jl + 4)*(ku - kl + 3) + ((j) - (jl-2))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINH_MPI(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 4) + ((j) - (jl-1))*(ku - kl + 4) + ((k) - (kl-2)))
#define LINRS_MPI(i,j,k) (((i) - (il))*(ju - jl + 1)*(ku - kl + 1) + ((j) - (jl))*(ku - kl + 1) + ((k) - (kl)))
#define LINFLAG_MPI(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))

#define LINU(i,j,k) (((i) - (il-2))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINV(i,j,k) (((i) - (il-1))*(ju - jl + 4)*(ku - kl + 3) + ((j) - (jl-2))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINW(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 4) + ((j) - (jl-1))*(ku - kl + 4) + ((k) - (kl-2)))
#define LINP(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINT(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINF(i,j,k) (((i) - (il-2))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))
#define LING(i,j,k) (((i) - (il-1))*(ju - jl + 4)*(ku - kl + 3) + ((j) - (jl-2))*(ku - kl + 3) + ((k) - (kl-1)))
#define LINH(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 4) + ((j) - (jl-1))*(ku - kl + 4) + ((k) - (kl-2)))
#define LINRS(i,j,k) (((i) - (il))*(ju - jl + 1)*(ku - kl + 1) + ((j) - (jl))*(ku - kl + 1) + ((k) - (kl)))
#define LINFLAG(i,j,k) (((i) - (il-1))*(ju - jl + 3)*(ku - kl + 3) + ((j) - (jl-1))*(ku - kl + 3) + ((k) - (kl-1)))

#ifdef FP_FLOAT
#define MPI_REAL_FLUID3D DCI_FLOAT
#endif
#ifdef FP_DOUBLE
#define MPI_REAL_FLUID3D DCI_DOUBLE
#endif

#endif
