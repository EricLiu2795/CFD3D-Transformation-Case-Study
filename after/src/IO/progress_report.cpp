
#include <iostream>
#include "IO/progress_report.hpp"

const char prefix[] = "Progress: [";
const size_t leaderLength = sizeof(prefix) - 1;
const char suffix[] = "]";
const size_t trailerLength = sizeof(suffix) - 1;

void StatusReporter::renderProgress(Real t, Real tEnd, std::size_t maxVal) {
    int activePercentDone = (int)((t / tEnd) * 100);
    if (priorPercentDone == activePercentDone) {
        return;
    }
    priorPercentDone = activePercentDone;

    auto filledCount = size_t(t / tEnd * Real(maxVal));
    std::string buffer =
            prefix + std::string(filledCount, '#') + std::string(maxVal - filledCount, ' ') + suffix;

    std::cout << "\r" << char(27) << "[2K\r" << buffer;
    std::cout << activePercentDone << "%\tt = " << t;
    std::cout.flush();
}

void reportFileWritten(int n, Real t, std::size_t maxVal) {
    char output[] = "\rWrote file at %5s %6i %5s %10f\n";
    size_t clearLineWidth = maxVal + leaderLength + trailerLength + 30;
    std::string buffer(clearLineWidth, ' ');

    std::cout << "\r" << buffer;
    std::cout.flush();

    printf(output, "n: ", n, "t:", t);
    std::cout.flush();
}
