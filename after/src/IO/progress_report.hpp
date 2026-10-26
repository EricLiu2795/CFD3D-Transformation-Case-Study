
#ifndef FLUID3D_PROGRESS_REPORT_HPP
#define FLUID3D_PROGRESS_REPORT_HPP

#include <cstdint>
#include <string>
#include "Defines.hpp"

class StatusReporter {
public:
    void renderProgress(Real t, Real tEnd, size_t maxVal);

private:
    int priorPercentDone = -1;
};

void reportFileWritten(int n, Real t, size_t maxVal);


#endif
