
#include <iostream>
#include <vector>
#include <netcdf.h>
#include "IO/netcdf_output.hpp"

bool NetCdfRecorder::setupRecorder(const std::string &filename, const OutputGrid &grid) {
    const int imax = grid.imax, jmax = grid.jmax, kmax = grid.kmax;
    const Real dx = grid.dx, dy = grid.dy, dz = grid.dz;
    const Real xOrigin = grid.xOrigin, yOrigin = grid.yOrigin, zOrigin = grid.zOrigin;

    this->imax = imax;
    this->jmax = jmax;
    this->kmax = kmax;
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
    this->xOrigin = xOrigin;
    this->yOrigin = yOrigin;
    this->zOrigin = zOrigin;

    if (!mpiEnabled) {
        il = jl = kl = 1;
        iu = imax;
        ju = jmax;
        ku = kmax;
    } else {
        std::cerr << "Error: NetCdfRecorder doesn't support the distributed MPI solver (yet). "
                  << "Please consider using the VtkExporter class instead." << std::endl;
        exit(1);
    }

    if (hasOpenFile) {
        delete[] centeredU;
        delete[] centeredV;
        delete[] centeredW;
        nc_close(ncid);
    }

    std::string datasetFilename = filename + ".nc";
    int status = nc_create(datasetFilename.c_str(), NC_NETCDF4 | NC_CLOBBER, &ncid);
    if (status != 0) {
        std::cerr << "ERROR in loadNetCdfFile: File \"" << datasetFilename << "\" couldn't be opened!" << std::endl;
        return false;
    }
    hasOpenFile = true;
    recordIndex = 0;

    setTextAttribute(NC_GLOBAL, "Conventions", "CF-1.5");
    setTextAttribute(NC_GLOBAL, "title", "Fluid simulation in 3D");
    setTextAttribute(NC_GLOBAL, "history", "fluid3d");
    setTextAttribute(NC_GLOBAL, "institution", "Technische Universitaet Muenchen, Department of Informatics");
    setTextAttribute(NC_GLOBAL, "source", "Simulation using incompressible Navier-Stokes equations.");
    setTextAttribute(NC_GLOBAL, "references", "https://gitlab.lrz.de/ga42wis/fluid3d");
    setTextAttribute(NC_GLOBAL, "comment", "fluid3d was released under the 3-clause BSD license.");


    int timeAxis, xAxis, yAxis, zAxis;
    nc_def_dim(ncid, "time", NC_UNLIMITED, &timeAxis);
    nc_def_dim(ncid, "x", imax, &xAxis);
    nc_def_dim(ncid, "y", jmax, &yAxis);
    nc_def_dim(ncid, "z", kmax, &zAxis);


    nc_def_var(ncid, "x", NC_FP, 1, &xAxis, &xHandle);
    nc_def_var(ncid, "y", NC_FP, 1, &yAxis, &yHandle);
    nc_def_var(ncid, "z", NC_FP, 1, &zAxis, &zHandle);

    nc_def_var(ncid, "time", NC_FP, 1, &timeAxis, &timeHandle);
    setTextAttribute(timeHandle, "long_name", "Time");
    setTextAttribute(timeHandle, "units", "seconds since simulation start");

    setTextAttribute(xHandle, "coordinate_type", "Cartesian X");
    setTextAttribute(yHandle, "coordinate_type", "Cartesian Y");
    setTextAttribute(zHandle, "coordinate_type", "Cartesian Z");
    setTextAttribute(xHandle, "axis", "X");
    setTextAttribute(yHandle, "axis", "Y");
    setTextAttribute(zHandle, "axis", "Z");
    setTextAttribute(timeHandle, "axis", "T");

    int staticAxes3D[] = {xAxis, yAxis, zAxis};
    int varyingAxes3D[] = {timeAxis, xAxis, yAxis, zAxis};
    nc_def_var(ncid, "geometry", NC_FP, 3, staticAxes3D, &geometryHandle);
    nc_def_var(ncid, "U", NC_FP, 4, varyingAxes3D, &UHandle);
    nc_def_var(ncid, "V", NC_FP, 4, varyingAxes3D, &VHandle);
    nc_def_var(ncid, "W", NC_FP, 4, varyingAxes3D, &WHandle);
    nc_def_var(ncid, "P", NC_FP, 4, varyingAxes3D, &PHandle);
    nc_def_var(ncid, "T", NC_FP, 4, varyingAxes3D, &THandle);

    Real coordValue = xOrigin + Real(0.5) * dx;
    for (size_t i = 0; i < (size_t)imax; i++) {
        nc_put_var1_fp(ncid, xHandle, &i, &coordValue);
        coordValue += dx;
    }
    coordValue = yOrigin + Real(0.5) * dy;
    for (size_t j = 0; j < (size_t)jmax; j++) {
        nc_put_var1_fp(ncid, yHandle, &j, &coordValue);
        coordValue += dy;
    }
    coordValue = zOrigin + Real(0.5) * dz;
    for (size_t k = 0; k < (size_t)kmax; k++) {
        nc_put_var1_fp(ncid, zHandle, &k, &coordValue);
        coordValue += dz;
    }

    centeredU = new Real[imax*jmax*kmax];
    centeredV = new Real[imax*jmax*kmax];
    centeredW = new Real[imax*jmax*kmax];

    return true;
}

void NetCdfRecorder::setMpiBounds(int il, int iu, int jl, int ju, int kl, int ku) {
    this->il = il;
    this->iu = iu;
    this->jl = jl;
    this->ju = ju;
    this->kl = kl;
    this->ku = ku;
    mpiEnabled = true;
}

NetCdfRecorder::~NetCdfRecorder() {
    if (hasOpenFile) {
        delete[] centeredU;
        delete[] centeredV;
        delete[] centeredW;
        nc_close(ncid);
    }
}

void NetCdfRecorder::writeField3D_Collocated(int varHandle, int jExtent, int kExtent, Real *values) {
    size_t start[] = {recordIndex, 0, 0, 0};
    size_t count[] = {1, 1, 1, (size_t)kmax};
    for (int i = 0; i < imax; i++) {
        start[1] = i;
        for (int j = 0; j < jmax; j++) {
            start[2] = j;
            nc_put_vara_fp(ncid, varHandle, start, count, (values + i*jExtent*kExtent + j*kExtent));
        }
    }
}

void NetCdfRecorder::writeField3D_Staggered(int varHandle, int jExtent, int kExtent, Real *values) {
    size_t start[] = {recordIndex, 0, 0, 0};
    size_t count[] = {1, 1, 1, (size_t)kmax};
    for (int i = 1; i <= imax; i++) {
        start[1] = i-1;
        for (int j = 1; j <= jmax; j++) {
            start[2] = j-1;
            nc_put_vara_fp(ncid, varHandle, start, count, (values + i*jExtent*kExtent + j*kExtent + 1));
        }
    }
}

void NetCdfRecorder::setTextAttribute(int varid, const std::string &name, const std::string &value) {
    nc_put_att_text(ncid, varid, name.c_str(), value.size(), value.c_str());
}

void NetCdfRecorder::writeSnapshot(int stepIndex, Real time, Real *U, Real *V, Real *W, Real *P, Real *T,
        FlagType *Flag) {
    if (stepIndex == 0) {
        std::vector<Real> geometrySamples(imax*jmax*kmax);
        #pragma omp parallel for shared(geometrySamples, Flag) default(none)
        for (int i = 0; i < imax; i++) {
            for (int j = 0; j < jmax; j++) {
                for (int k = 0; k < kmax; k++) {
                    geometrySamples[i*jmax*kmax + j*kmax + k] = Real(isWet(Flag[LINFLAG(i+1,j+1,k+1)]) ? 1 : 0);
                }
            }
        }
        nc_put_var_fp(ncid, geometryHandle, geometrySamples.data());
    }

    #pragma omp parallel for shared(U, V, W) default(none)
    for (int i = 0; i < imax; i++) {
        for (int j = 0; j < jmax; j++) {
            for (int k = 0; k < kmax; k++) {
                centeredU[i*jmax*kmax + j*kmax + k] = (U[LINU(i+1,j+1,k+1)] + U[LINU(i,j+1,k+1)]) / Real(2.0);
                centeredV[i*jmax*kmax + j*kmax + k] = (V[LINV(i+1,j+1,k+1)] + V[LINV(i+1,j,k+1)]) / Real(2.0);
                centeredW[i*jmax*kmax + j*kmax + k] = (W[LINW(i+1,j+1,k+1)] + W[LINW(i+1,j+1,k)]) / Real(2.0);
            }
        }
    }

    nc_put_var1_fp(ncid, timeHandle, &recordIndex, &time);
    writeField3D_Collocated(UHandle, jmax, kmax, centeredU);
    writeField3D_Collocated(VHandle, jmax, kmax, centeredV);
    writeField3D_Collocated(WHandle, jmax, kmax, centeredW);
    writeField3D_Staggered(PHandle, jmax+2, kmax+2, P);
    writeField3D_Staggered(THandle, jmax+2, kmax+2, T);
    recordIndex++;

}

std::string NetCdfRecorder::getFileExtension()
{
    return ".nc";
}
