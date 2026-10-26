
#ifndef FLUID3D_VTK_OUTPUT_HPP
#define FLUID3D_VTK_OUTPUT_HPP

#include "IO/output_recorder.hpp"

enum VtkDataFormat {
    VTK_FORMAT_ASCII, VTK_FORMAT_BINARY
};

struct GridGeometry {
    Real dx, dy, dz;
    Real xOrigin, yOrigin, zOrigin;
};

class VtkExporter : public OutputFileRecorder {
public:
    VtkExporter(int partCount, int localRank, VtkDataFormat dataFormat = VTK_FORMAT_BINARY)
            : partCount(partCount), localRank(localRank), useBinaryFormat(dataFormat == VTK_FORMAT_BINARY)
    {}
    ~VtkExporter() override;

    std::string getFileExtension() override;

    void setMpiBounds(int il, int iu, int jl, int ju, int kl, int ku) override;

    bool setupRecorder(const std::string &filename, const OutputGrid &grid) override;
    void writeSnapshot(
            int stepIndex, Real time, Real *U, Real *V, Real *W, Real *P, Real *T, FlagType *Flag) override;

private:
    template <typename T>
    static void reverseByteOrder(T *values, int n);

    void emitVtkHeader(FILE *file);
    void emitPointPositions(FILE *file,
            Real dx, Real dy, Real dz, Real xOrigin, Real yOrigin, Real zOrigin);
    void emitPointValues(FILE *file, Real *U, Real *V, Real *W, FlagType *Flag);
    void emitCellValues(FILE *file, Real *P, Real *T, FlagType *Flag);

    std::string filename;
    bool mpiEnabled = false;
    int partCount, localRank;
    bool useBinaryFormat;
    int imax{}, jmax{}, kmax{};
    int il{}, iu{}, jl{}, ju{}, kl{}, ku{};
    GridGeometry geometry{};

    float *pointBuffer = nullptr;
    float *cellBuffer = nullptr;
    uint8_t *cellByteBuffer = nullptr;
};


#endif
