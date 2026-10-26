
#ifndef FLUID3D_OUTPUT_RECORDER_HPP
#define FLUID3D_OUTPUT_RECORDER_HPP

#include <string>
#include "CfdSolver/cell_flags.hpp"

struct OutputGrid {
    int imax, jmax, kmax;
    Real dx, dy, dz;
    Real xOrigin, yOrigin, zOrigin;
};

class OutputFileRecorder {
public:
    virtual ~OutputFileRecorder() = default;

    virtual std::string getFileExtension() = 0;

    virtual void setMpiBounds(int il, int iu, int jl, int ju, int kl, int ku) = 0;

    virtual bool setupRecorder(const std::string &filename, const OutputGrid &grid) = 0;
    virtual void writeSnapshot(
            int stepIndex, Real time, Real *U, Real *V, Real *W, Real *P, Real *T, FlagType *Flag) = 0;
};


#endif
