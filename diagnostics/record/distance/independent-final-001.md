# Distance from upstream (independent-final-001)

Reference 5943ae78be (upstream), compared with fc82bc131f. Shares are of the reference: 1.0 = all of it is still there, 0.0 = none. These are a reference, not targets.

| measure | whole scope |
|---|---|
| upstream functions (tokens) | 241 (89346) |
| identifier overlap: project names left | 0.103 (76 of 740) |
| identifier overlap: share of project-name uses that are upstream names | 0.386 |
| token similarity: fingerprints left (anywhere / best match) | 0.964 / 0.896 |
| AST similarity: subtrees left | 0.951 |
| distinctiveness of upstream functions (mean, token-weighted) | 0.199 |
| distinctive fingerprints left | 0.911 |
| functions unchanged (same fingerprints) | 158 of 241 |
| functions with a recorded transformation | 110 of 241 (covering 0.403 of the distinctive shape) |

## Per upstream directory

| directory | functions | tokens | token sim. | AST sim. | ids left | distinctive left | unchanged | transformed |
|---|---|---|---|---|---|---|---|---|
| src/IO | 73 | 14003 | 0.921 | 0.948 | 0.175 | 0.864 | 50 | 30 |
| src/CfdSolver | 139 | 66836 | 0.993 | 0.982 | 0.348 | 0.951 | 105 | 56 |
| src | 1 | 2196 | 0.944 | 0.901 | 0.312 | 0.934 | 0 | 0 |
| src/ParticleTracer | 28 | 6311 | 0.76 | 0.657 | 0.25 | 0.883 | 3 | 24 |

## Most upstream-like functions (distinctive shape left)

residual = distinctive fingerprints still present. distinct = share of the function's fingerprints that only it has. tok/AST = share left anywhere in the tree. applied = transformations the logs record.

| upstream function | tokens | distinct | residual | tok | AST | ids | now | applied |
|---|---|---|---|---|---|---|---|---|
| `main` (src/Main.cpp) | 2196 | 0.686 | 227.0 | 0.944 | 0.901 | 0.312 | src/simulation_main.cpp:main | - |
| `initFlagFromGeometryFile` (src/CfdSolver/Flag.cpp) | 1199 | 0.554 | 122.0 | 0.987 | 0.982 | 0.16 | src/CfdSolver/cell_flags.cpp:initFlagFromShapeFile | - |
| `nearestNeighborUpsampling` (src/CfdSolver/Flag.cpp) | 945 | 0.726 | 84.0 | 0.927 | 0.987 | 0.0 | src/CfdSolver/cell_flags.cpp:resizeNearest | I3 R1 |
| `NetCdfWriter::initializeWriter` (src/IO/NetCdfWriter.cpp) | 803 | 0.704 | 81.0 | 0.991 | 0.994 | 0.256 | src/IO/netcdf_output.cpp:NetCdfRecorder::setupRecorder | - |
| `parseArguments` (src/IO/ArgumentParser.cpp) | 710 | 0.898 | 79.0 | 1.0 | 1.0 | 0.25 | src/IO/cli_options.cpp:parseOptions | - |
| `traceStreamlineParticle` (src/ParticleTracer/StreamlineTracer.cpp) | 515 | 0.762 | 68.0 | 0.881 | 0.891 | 0.12 | src/ParticleTracer/streamline_tracer.cpp:integrateFlowlineParticle | I1 |
| `writeTrajectoriesToBinLinesFile` (src/IO/TrajectoriesFile.cpp) | 480 | 0.653 | 63.0 | 0.815 | 0.843 | 0.0 | src/IO/path_writer.cpp:writePathsToBinary | R3 S1x2 S8 |
| `CfdSolverOpencl::CfdSolverOpencl` (src/CfdSolver/Opencl/CfdSolverOpencl.cpp) | 655 | 0.658 | 52.0 | 1.0 | 1.0 | 0.036 | src/CfdSolver/Opencl/CfdSolverOpencl.cpp:CfdSolverOpencl::CfdSolverOpencl | - |
| `mpiInit` (src/CfdSolver/Mpi/MpiHelpers.cpp) | 337 | 0.867 | 49.0 | 0.933 | 0.985 | 0.091 | src/CfdSolver/Mpi/MpiHelpers.cpp:mpiStartup | I1 |
| `loadVariablesFromDatFile` (src/IO/ScenarioFile.cpp) | 257 | 0.803 | 49.0 | 1.0 | 1.0 | 0.25 | src/IO/case_config.cpp:readSettingsFile | - |
| `writeTrajectoriesToObjFile` (src/IO/TrajectoriesFile.cpp) | 303 | 0.615 | 48.0 | 1.0 | 1.0 | 0.0 | src/IO/path_writer.cpp:writePathsToObj | S8 |
| `nearestNeighborUpsamplingPgm2D` (src/IO/PgmFile.cpp) | 478 | 0.52 | 45.0 | 0.922 | 0.91 | 0.0 | src/IO/pgm_image.cpp:nearestNeighborScalePgm2D | I3 S2 |
| `VtkWriter::writeCellData` (src/IO/VtkWriter.cpp) | 1558 | 0.492 | 42.0 | 0.847 | 0.925 | 0.118 | src/IO/vtk_output.cpp:VtkExporter::emitCellValues | P1 |
| `isValidCell` (src/IO/GeometryCreator.cpp) | 209 | 0.837 | 41.0 | 1.0 | 1.0 | 0.25 | src/IO/scenario_builder.cpp:isCellAllowed | - |
| `VtkWriter::writePointData` (src/IO/VtkWriter.cpp) | 1426 | 0.365 | 41.0 | 0.96 | 0.993 | 0.111 | src/IO/vtk_output.cpp:VtkExporter::emitPointValues | P1 |
| `CfdSolverCuda::initialize` (src/CfdSolver/Cuda/CfdSolverCuda.cu) | 1226 | 0.392 | 38.0 | 1.0 | 1.0 | 0.382 | src/CfdSolver/cuda/cuda_solver.cu:FluidSolverCuda::initialize | - |
| `NetCdfWriter::writeTimestep` (src/IO/NetCdfWriter.cpp) | 491 | 0.548 | 35.0 | 0.869 | 0.951 | 0.143 | src/IO/netcdf_output.cpp:NetCdfRecorder::writeSnapshot | S5 |
| `CfdSolverOpencl::initialize` (src/CfdSolver/Opencl/CfdSolverOpencl.cpp) | 1054 | 0.43 | 34.0 | 1.0 | 1.0 | 0.405 | src/CfdSolver/Opencl/CfdSolverOpencl.cpp:CfdSolverOpencl::initialize | - |
| `createTowerGeometry` (src/IO/GeometryCreator.cpp) | 297 | 0.516 | 32.0 | 1.0 | 1.0 | 0.188 | src/IO/scenario_builder.cpp:buildTowerGeometry | - |
| `ClInterface::compileSourceFile` (src/CfdSolver/Opencl/ClInterface.cpp) | 171 | 0.705 | 31.0 | 1.0 | 1.0 | 0.4 | src/CfdSolver/Opencl/ClInterface.cpp:ClInterface::compileKernelFile | - |
| `sorSolverOpencl` (src/CfdSolver/Opencl/SorSolverOpencl.cpp) | 866 | 0.234 | 30.0 | 1.0 | 1.0 | 0.193 | src/CfdSolver/Opencl/SorSolverOpencl.cpp:relaxSolverOpencl | - |
| `prepareOutputDirectory` (src/IO/IOUtils.cpp) | 292 | 0.706 | 28.0 | 0.824 | 0.955 | 0.1 | src/IO/text_file_utils.cpp:clearStaleOutputFiles | F2 I2 I6x2 |
| `ProgressBar::printProgress` (src/IO/ProgressBar.cpp) | 125 | 0.903 | 28.0 | 1.0 | 1.0 | 0.286 | src/IO/progress_report.cpp:StatusReporter::renderProgress | - |
| `ClInterface::printInfo` (src/CfdSolver/Opencl/ClInterface.cpp) | 115 | 0.964 | 27.0 | 1.0 | 1.0 | 0.333 | src/CfdSolver/Opencl/ClInterface.cpp:ClInterface::reportInfo | - |
| `VtkWriter::writeTimestep` (src/IO/VtkWriter.cpp) | 206 | 0.609 | 27.0 | 0.978 | 0.977 | 0.312 | src/IO/vtk_output.cpp:VtkExporter::writeSnapshot | I7 |
| `sorSolverIterationCpp` (src/CfdSolver/Cpp/SorSolverCpp.cpp) | 2045 | 0.145 | 26.0 | 0.96 | 0.836 | 0.643 | src/CfdSolver/serial/poisson_sor.cpp:relaxSolverSweepCpp | P1 |
| `createHeightMapGeometry` (src/IO/GeometryCreator.cpp) | 401 | 0.368 | 26.0 | 0.908 | 0.929 | 0.154 | src/IO/scenario_builder.cpp:buildElevationGeometry | S1 |
| `VtkWriter::writePointCoordinates` (src/IO/VtkWriter.cpp) | 354 | 0.413 | 26.0 | 1.0 | 1.0 | 0.4 | src/IO/vtk_output.cpp:VtkExporter::emitPointPositions | - |
| `CfdSolverMpi::initialize` (src/CfdSolver/Mpi/CfdSolverMpi.cpp) | 1397 | 0.333 | 25.0 | 1.0 | 1.0 | 0.737 | src/CfdSolver/Mpi/CfdSolverMpi.cpp:CfdSolverMpi::initialize | - |
| `readScenarioConfigurationFromFile` (src/IO/ScenarioFile.cpp) | 514 | 0.735 | 25.0 | 0.971 | 0.991 | 0.692 | src/IO/case_config.cpp:loadCaseConfigFromFile | I1 |
| `rayBoxPlaneIntersection` (src/ParticleTracer/Intersection.cpp) | 148 | 0.658 | 25.0 | 1.0 | 1.0 | 0.125 | src/ParticleTracer/ray_box.cpp:castRayAgainstSlab | - |
| `reduceSumCudaKernel` (src/CfdSolver/Cuda/SorSolverCuda.cu) | 196 | 0.436 | 24.0 | 1.0 | 1.0 | 0.0 | src/CfdSolver/cuda/poisson_kernels.cu:arraySumCudaKernel | - |
| `sorSolverComputeResidualArrayCuda` (src/CfdSolver/Cuda/SorSolverCuda.cu) | 362 | 0.284 | 23.0 | 1.0 | 1.0 | 0.385 | src/CfdSolver/cuda/poisson_kernels.cu:relaxSolverResidualArrayCuda | - |
| `loadValuesFromGeometryFile` (src/IO/GeometryFile.cpp) | 286 | 0.484 | 23.0 | 0.823 | 0.921 | 0.222 | src/IO/geometry_io.cpp:loadGeometryData | R3 S5 |
| `setBoundaryValuesScenarioSpecificCuda` (src/CfdSolver/Cuda/BoundaryValuesCuda.cu) | 283 | 0.489 | 22.0 | 1.0 | 1.0 | 0.353 | src/CfdSolver/cuda/boundary_kernels.cu:setBoundaryValuesScenarioSpecificCuda | - |
| `calculateMaximum` (src/CfdSolver/Cuda/UvwCuda.cu) | 260 | 0.386 | 22.0 | 1.0 | 1.0 | 0.2 | src/CfdSolver/cuda/momentum_kernels.cu:computePeak | - |
| `ClInterface::loadProgramFromBinaryFile` (src/CfdSolver/Opencl/ClInterface.cpp) | 170 | 0.649 | 22.0 | 0.946 | 0.974 | 0.727 | src/CfdSolver/Opencl/ClInterface.cpp:ClInterface::importProgramFromBinaryFile | R3 |
| `ProgressBar::printOutput` (src/IO/ProgressBar.cpp) | 83 | 1.0 | 22.0 | 1.0 | 1.0 | 0.167 | src/IO/progress_report.cpp:reportFileWritten | I5 |
| `sorSolverCuda` (src/CfdSolver/Cuda/SorSolverCuda.cu) | 573 | 0.193 | 21.0 | 1.0 | 1.0 | 0.27 | src/CfdSolver/cuda/poisson_kernels.cu:sorSolverCuda | - |
| `ClInterface::initialize` (src/CfdSolver/Opencl/ClInterface.cpp) | 130 | 0.724 | 21.0 | 1.0 | 1.0 | 0.273 | src/CfdSolver/Opencl/ClInterface.cpp:ClInterface::initialize | - |

## Upstream identifiers most specific to upstream that are still used

| name | upstream uses | upstream files | uses now |
|---|---|---|---|
| `variables` | 55 | 1 | 55 |
| `Real` | 1726 | 56 | 1705 |
| `Buffer` | 166 | 6 | 166 |
| `jmax` | 802 | 56 | 745 |
| `kmax` | 799 | 56 | 742 |
| `Flag` | 560 | 40 | 539 |
| `ClInterface` | 43 | 4 | 43 |
| `ifile` | 10 | 1 | 11 |
| `numDirectFlag` | 54 | 6 | 39 |
| `NC_GLOBAL` | 7 | 1 | 7 |
| `CfdSolverMpi` | 18 | 3 | 18 |
| `CfdSolverOpencl` | 18 | 3 | 18 |
| `Program` | 15 | 3 | 15 |
| `T_c` | 76 | 18 | 88 |
| `T_h` | 76 | 18 | 78 |
| `scenarioName` | 105 | 26 | 105 |
| `CL_SUCCESS` | 4 | 1 | 4 |
| `platform` | 7 | 2 | 7 |
| `linearSystemSolverType` | 66 | 20 | 66 |
| `omg` | 66 | 20 | 66 |
| `xOrigin` | 25 | 8 | 24 |
| `yOrigin` | 25 | 8 | 24 |
| `zOrigin` | 25 | 8 | 24 |
| `binaries` | 3 | 1 | 3 |
| `programs` | 3 | 1 | 3 |
| `streamlines` | 3 | 1 | 3 |
| `useTemperature` | 53 | 20 | 49 |
| `Local` | 5 | 2 | 5 |
| `postfix` | 5 | 2 | 5 |
| `reduction` | 5 | 2 | 5 |
| `itermax` | 47 | 20 | 47 |
| `dtWrite` | 7 | 3 | 7 |
| `xLength` | 7 | 3 | 6 |
| `yLength` | 7 | 3 | 6 |
| `zLength` | 7 | 3 | 6 |
| `shallWriteOutput` | 51 | 22 | 45 |
| `tEnd` | 11 | 5 | 11 |
| `Singleton` | 4 | 2 | 4 |
| `seekg` | 4 | 2 | 4 |
| `LINEAR_SOLVER_JACOBI` | 9 | 5 | 9 |

## Transformation coverage

110 of 241 upstream functions have a recorded transformation; 158 are unchanged.

| family / stage | functions |
|---|---|
| stage pass | 106 |
| stage layout | 7 |
| family I | 74 |
| family S | 51 |
| family R | 14 |
| family P | 8 |
| family F | 7 |
