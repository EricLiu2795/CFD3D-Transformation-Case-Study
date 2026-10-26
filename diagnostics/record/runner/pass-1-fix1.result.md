## Report

**What broke.** Commit `d6b5072` ("Make divCeil generic over integer types") rewrote `divCeil` in `src/Defines.hpp` from `inline int divCeil(int, int)` to a single-parameter template `template <typename T> inline T divCeil(T x, T y)`. That template only deduces when both arguments share a type, but the CUDA grid-dimension call sites (e.g. `boundary_kernels.cu:524`) pass an `int` extent together with an `unsigned` `dim3` component. Template deduction failed → 17 compile errors in the `cuda-fma` build. The CPU-reference build was unaffected (its `divCeil` callers are all `int,int`).

**Fix.** New commit on top of HEAD, `a3ec92a` "Allow divCeil to take a differently typed divisor":
```cpp
template <typename T, typename U>
inline T divCeil(T x, U y) { return (x - 1) / static_cast<T>(y) + 1; }
```
Deduces the compute/return type from the dividend and converts the divisor to it. This matches the original integer semantics exactly: for CPU callers (`T=U=int`) the cast is a no-op; for the CUDA call sites (`T=int`, `U=unsigned`) the divisor converts to `int` just as the original non-template overload did, so the computed integers are unchanged. History was not rewritten.

**Log.** The `S11` instance entry for `6eff0a6e04` / pass commit `d6b50722da` is marked `"status": "corrected"` with `"correction_commit": "a3ec92a"` and a reason, in `RECORD/TRANSFORM_LOG.json`.

**Checks run.**
- `cpu-reference` smoke: returncode 0 — passes (this is the bit-identity criterion).
- `testbed.py gpu-build --builds cuda-fma cuda-no-fma`: **pass** — both CUDA builds now compile and link cleanly, confirming the defect is resolved (device-free, as the contract specifies GPU builds are compiled not run on this host).
- `batch.py verify ... pass-1 --base 665762f30`: **does not reach PASS.** The compile defect is gone; it now fails at the mandatory `cuda` smoke run with `Fatal error in FluidSolverCuda::initialize: No CUDA devices were found.`

**Why verify cannot pass locally.** The GPU device node `/dev/dxg` is absent from the host right now (confirmed with the bash sandbox disabled; `nvidia-smi` not found, no `/dev/nvidia*`). It was present when the baseline was recorded at 01:34 today (the baseline has `cuda-fma`/`cuda-no-fma` cases). This is an environment regression, not a transformation defect — no instance in the chain causes it, and it would equally block any commit, including the baseline. Per the run contract (`gpu_check`: "mandatory separate trusted host evaluation") the CUDA numerical run belongs to a separate trusted host; GPU runs are not mine to start.

**State handed over.** Fix committed at `a3ec92a`; all three builds compile/link; CPU-reference smoke passes. The CUDA smoke/numerical evaluation for pass-1 at `a3ec92a` is pending a host with the GPU present (`/dev/dxg`); it should be run with the reference-build settings against `record/work-base/public-baseline.json`. I did not modify the testbed, baseline, or tests, and did not work around the missing device.