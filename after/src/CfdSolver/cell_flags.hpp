
#ifndef FLUID3D_CELL_FLAGS_HPP
#define FLUID3D_CELL_FLAGS_HPP

#include <string>
#include <vector>
#include "Defines.hpp"
inline bool isInlet(unsigned int flag) { return (flag >> 4) & 1; }
inline bool isOutlet(unsigned int flag) { return (flag >> 3) & 1; }
inline bool isFrictionless(unsigned int flag) { return (flag >> 2) & 1; }
inline bool isSticky(unsigned int flag) { return (flag >> 1) & 1; }

inline bool isWet(unsigned int flag) { return (flag >> 0) & 1; }
inline bool B_F(unsigned int flag) { return (flag >> 10) & 1 ; }
inline bool B_B(unsigned int flag) { return (flag >> 9) & 1 ; }
inline bool B_U(unsigned int flag) { return (flag >> 8) & 1 ; }
inline bool B_D(unsigned int flag) { return (flag >> 7) & 1 ; }
inline bool B_R(unsigned int flag) { return (flag >> 6) & 1 ; }

inline bool B_L(unsigned int flag) { return (flag >> 5) & 1 ; }
inline bool isChilled(unsigned int flag) { return (flag >> 12) & 1; }

inline bool isHeated(unsigned int flag) { return (flag >> 11) & 1; }



void initFlagFromShapeFile(const std::string &scenarioName, const std::string &shapeFilename,
        int imax, int jmax, int kmax, FlagType *&Flag);

void initFlagNoBarriers(const std::string &scenarioName, int imax, int jmax, int kmax, FlagType *&Flag);

void resizeNearest(
        const std::vector<unsigned int> valuesSrc,
        int widthSrc,
        int heightSrc,
        std::vector<unsigned int> &valuesDst,
        int widthDst,
        int heightDst);

#endif
