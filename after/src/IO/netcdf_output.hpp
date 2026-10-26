
#ifndef FLUID3D_NETCDF_OUTPUT_HPP
#define FLUID3D_NETCDF_OUTPUT_HPP

#include "IO/output_recorder.hpp"

class NetCdfRecorder : public OutputFileRecorder {
public:
    NetCdfRecorder(int partCount, int localRank) : partCount(partCount), localRank(localRank) {}

    ~NetCdfRecorder() override;

    std::string getFileExtension() override;

    void setMpiBounds(int il, int iu, int jl, int ju, int kl, int ku) override;

    bool setupRecorder(const std::string &filename, const OutputGrid &grid) override;
    void writeSnapshot(
            int stepIndex, Real time, Real *U, Real *V, Real *W, Real *P, Real *T, FlagType *Flag) override;

private:
    void writeField3D_Staggered(int varHandle, int jExtent, int kExtent, Real *values);
    void writeField3D_Collocated(int varHandle, int jExtent, int kExtent, Real *values);
    void setTextAttribute(int varid, const std::string &name, const std::string &value);

    bool mpiEnabled = false;
    int partCount, localRank;
    int imax{}, jmax{}, kmax{};
    int il{}, iu{}, jl{}, ju{}, kl{}, ku{};
    Real dx{}, dy{}, dz{}, xOrigin{}, yOrigin{}, zOrigin{};
    Real *centeredU{};
    Real *centeredV{};
    Real *centeredW{};

    bool hasOpenFile = false;
    int ncid{};
    size_t recordIndex{};

    int timeHandle{}, xHandle{}, yHandle{}, zHandle{}, geometryHandle{}, UHandle{}, VHandle{}, WHandle{}, PHandle{}, THandle{};
};


#endif
