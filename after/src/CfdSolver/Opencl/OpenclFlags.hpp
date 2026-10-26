
#ifndef FLUID3D_OPENCLFLAGS_HPP
#define FLUID3D_OPENCLFLAGS_HPP

#define FlagType unsigned int


#ifdef FP_FLOAT
#define Real float
#endif
#ifdef FP_DOUBLE
#define Real double
#endif

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

inline bool isWet(unsigned int flag) { return (flag >> 0) & 1; }
inline bool isSticky(unsigned int flag) { return (flag >> 1) & 1; }
inline bool isFrictionless(unsigned int flag) { return (flag >> 2) & 1; }
inline bool isOutlet(unsigned int flag) { return (flag >> 3) & 1; }
inline bool isInlet(unsigned int flag) { return (flag >> 4) & 1; }
inline bool B_L(unsigned int flag) { return (flag >> 5) & 1; }
inline bool B_R(unsigned int flag) { return (flag >> 6) & 1; }
inline bool B_D(unsigned int flag) { return (flag >> 7) & 1; }
inline bool B_U(unsigned int flag) { return (flag >> 8) & 1; }
inline bool B_B(unsigned int flag) { return (flag >> 9) & 1; }
inline bool B_F(unsigned int flag) { return (flag >> 10) & 1; }
inline bool isHeated(unsigned int flag) { return (flag >> 11) & 1; }
inline bool isChilled(unsigned int flag) { return (flag >> 12) & 1; }

#endif
