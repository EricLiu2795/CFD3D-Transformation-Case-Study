# Layout stage plan — cfd3d (`src/`)

The tree is the `cfd3d` 3D CFD solver: an abstract solver interface with three
backends (a serial host path, a CUDA path, and non-compiled OpenCL/MPI paths),
plus field/flag initialisation, an IO layer and a particle tracer.

## Build / scope facts that constrain the layout

- `CMakeLists.txt` is **frozen** and discovers sources with
  `file(GLOB_RECURSE src/*.cpp src/*.c)` and `file(GLOB_RECURSE src/*.cu)`, so
  renaming, splitting, merging and moving files inside `src/` needs no build-system
  edit — the glob picks them up. `include_directories(src)` means every header can
  be reached by its `src/`-rooted path.
- The only hard-coded paths in the frozen `CMakeLists.txt` are the OpenCL and MPI
  `.cpp` files it feeds to `list(REMOVE_ITEM ...)`. Those files are compiled by **no**
  build configuration (`cpu-reference`, `cuda-fma`, `cuda-no-fma` all exclude
  `Mpi/` and `Opencl/`). Renaming or moving those nine source files would both break
  the frozen `REMOVE_ITEM` and be unvalidated, so the **`Mpi/` and `Opencl/`
  directories are left as they are** (kept, see below). Their `#include` lines that
  point at renamed headers are updated in place (textual, logged), which the include
  gate checks.
- `src/Defines.hpp` is the project-wide configuration/typedef header included by
  essentially every TU, including the kept OpenCL/MPI sources, via several spellings
  (`Defines.hpp`, `../../Defines.hpp`). It is **kept** by this file group; its
  contents are reorganised by the content group (F8 reorder).

## Inventory

Full inventory in `inventory.json`. Compiled TUs per build in
`symbols/base/*.tus` (identical set across the three builds except the five `.cu`
files, which only the two CUDA builds compile). All header basenames are unique
across the tree (upstream disambiguated backends by `Cpp`/`Cuda` suffixes), so an
`#include` can be retargeted unambiguously by basename.

## Group 1 — F1 + F2 + F7 + F9 (files: which files exist, their names, directories)

This group reshapes the **file set**: it renames the recognisable
one-file-per-stage-per-backend sources to responsibility names, drops the
`Cpp`/`Cuda` backend-suffix scheme for `serial/` and `cuda/` directories, merges two
tiny string/filesystem helpers, and splits the scenario-specific boundary code out of
the general boundary file. The content group (F3–F6, F8) then moves individual
definitions and reorders the files.

### F9 — rename/move files and directories

Backend directories and their modules (declarations/definitions unchanged, only file
names and the directory):

| old | new |
|---|---|
| `CfdSolver/Cpp/BorderValuesCpp.{hpp,cpp}` | `CfdSolver/serial/boundary_conditions.{hpp,cpp}` |
| `CfdSolver/Cpp/VelocityCpp.{hpp,cpp}` | `CfdSolver/serial/momentum_eqs.{hpp,cpp}` |
| `CfdSolver/Cpp/RelaxSolverCpp.{hpp,cpp}` | `CfdSolver/serial/poisson_sor.{hpp,cpp}` |
| `CfdSolver/Cpp/FluidSolverCpp.{hpp,cpp}` | `CfdSolver/serial/serial_solver.{hpp,cpp}` |
| `CfdSolver/Cuda/BorderValuesCuda.{hpp,cu}` | `CfdSolver/cuda/boundary_kernels.{hpp,cu}` |
| `CfdSolver/Cuda/VelocityCuda.{hpp,cu}` | `CfdSolver/cuda/momentum_kernels.{hpp,cu}` |
| `CfdSolver/Cuda/RelaxSolverCuda.{hpp,cu}` | `CfdSolver/cuda/poisson_kernels.{hpp,cu}` |
| `CfdSolver/Cuda/FluidSolverCuda.{hpp,cu}` | `CfdSolver/cuda/cuda_solver.{hpp,cu}` |
| `CfdSolver/Cuda/CudaUtils.{hpp,cu}` | `CfdSolver/cuda/cuda_runtime_check.{hpp,cu}` |
| `CfdSolver/CfdSolver.{hpp,cpp}` | `CfdSolver/solver_interface.{hpp,cpp}` |
| `CfdSolver/Init.{hpp,cpp}` | `CfdSolver/field_init.{hpp,cpp}` |
| `CfdSolver/Flag.{hpp,cpp}` | `CfdSolver/cell_flags.{hpp,cpp}` |
| `IO/BinaryStream.{hpp,cpp}` | `IO/byte_streams.{hpp,cpp}` |
| `IO/GeometryStore.{hpp,cpp}` | `IO/geometry_io.{hpp,cpp}` |
| `IO/GeometryBuilder.{hpp,cpp}` | `IO/scenario_builder.{hpp,cpp}` |
| `IO/PgmImage.{hpp,cpp}` | `IO/pgm_image.{hpp,cpp}` |
| `IO/CaseConfigFile.{hpp,cpp}` | `IO/case_config.{hpp,cpp}` |
| `IO/OptionParser.{hpp,cpp}` | `IO/cli_options.{hpp,cpp}` |
| `IO/StatusReporter.{hpp,cpp}` | `IO/progress_report.{hpp,cpp}` |
| `IO/OutputFileRecorder.hpp` | `IO/output_recorder.hpp` |
| `IO/NetCdfRecorder.{hpp,cpp}` | `IO/netcdf_output.{hpp,cpp}` |
| `IO/VtkExporter.{hpp,cpp}` | `IO/vtk_output.{hpp,cpp}` |
| `IO/PathsFile.{hpp,cpp}` | `IO/path_writer.{hpp,cpp}` |
| `ParticleTracer/ParticleTracer.{hpp,cpp}` | `ParticleTracer/particle_integrators.{hpp,cpp}` |
| `ParticleTracer/FlowlineIntegrator.{hpp,cpp}` | `ParticleTracer/streamline_tracer.{hpp,cpp}` |
| `ParticleTracer/RayCast.{hpp,cpp}` | `ParticleTracer/ray_box.{hpp,cpp}` |
| `ParticleTracer/TrajectoryFeatures.{hpp,cpp}` | `ParticleTracer/trajectory_output.{hpp,cpp}` |
| `Main.cpp` | `simulation_main.cpp` |

All new basenames are unique across the tree; include guards of renamed headers are
reset to `FLUID3D_<NEW_STEM>_HPP`. Every `#include` in the tree that resolves to a
renamed file — including those in the kept OpenCL/MPI sources — is retargeted to the
new `src/`-rooted path.

### F2 / F7 — merge files/headers

- `IO/TextHelpers.{hpp,cpp}` + `IO/FileHelpers.{hpp,cpp}` →
  `IO/text_file_utils.{hpp,cpp}`: two tiny modules of string predicates / tokenisers
  and output-directory helpers, which already travel together (`FileHelpers.cpp`
  includes `TextHelpers.hpp`). Same compile configurations for both.

### F1 — split a file by responsibility

- Split the scenario-specific inflow boundary out of the general boundary file:
  `assignScenarioBorderValuesCpp` moves from
  `CfdSolver/serial/boundary_conditions.cpp` into a new
  `CfdSolver/serial/scenario_boundary.cpp` (declaration stays in
  `boundary_conditions.hpp`, so callers are unaffected).

### Instances

One instance per module rename (header+source together), one for each backend
directory batch, one for the merge, one for the split. Listed with ids in
`TRANSFORM_LOG.json`; moves recorded in `moves-1.json`.

## Files kept by this group (rule logged)

| path(s) | rule |
|---|---|
| `src/Defines.hpp` | tree-wide configuration header included by the frozen OpenCL/MPI sources; content reorganised by the F8 content group instead of renaming |
| `src/CfdSolver/Mpi/*` (10 files) | compiled by no build configuration and referenced by exact path in the frozen `CMakeLists.txt`; moving/renaming is unvalidated and would break the frozen build |
| `src/CfdSolver/Opencl/*` (12 files incl. `.cl`) | same as `Mpi/`: not compiled by any configuration, `.cpp` paths hard-coded in the frozen `CMakeLists.txt` |

> The kept OpenCL/MPI *sources* still have their `#include` lines updated when they
> reference a renamed header; only their own paths are unchanged.
