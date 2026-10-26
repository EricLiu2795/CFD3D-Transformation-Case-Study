Done with the part I can do here. Here is the report.

## What I did

**Found the breaking instance.** The pass-3 verify failed with `exit 2` on the `cpu-reference` **build**. The error was in `src/ParticleTracer/streamline_tracer.cpp:38`:

```
invalid initialization of reference of type 'const SamplingGrid&' from expression of type 'const fpvec3'
```

The culprit is the I1 instance at commit `2684aef611` ("Group related parameters into descriptor structs", log step 56, `plan_id I1-8`): it changed `appendTrajectoryFeatures` to take a `SamplingGrid` descriptor and converted the main call site in `integrateFlowlineParticle` (line 44-45), but left the **second** call site in that function — the boundary-exit branch (line 37-38) — still passing the individual grid parameters. The group's static check (step 1 only, no compile/run) never caught it.

**Corrected it in one new commit on top of HEAD** (`fc82bc1`, history not rewritten): wrapped the grid arguments in the `SamplingGrid` brace-init `{domainOrigin, domainExtent, imax, jmax, kmax, dx, dy, dz}` to match the other call site.

**Marked the log entry.** `RECORD/TRANSFORM_LOG.json` step 56 is now `"status": "corrected"`, with `"corrected_by": fc82bc1…` and the reason. JSON re-validated.

## Checks I ran

- `testbed.py check --builds cpu-reference` on the two touched files → **`status: pass`**: the real regression (`cpu-reference` build + numeric smoke against baseline) is resolved. Both CUDA builds also compile and link successfully with the fix (present in the run's `executables`).
- The exact `batch.py verify … pass-3 --base aaa10078a` → still **FAIL**, but now for a different, environment-only reason: the testbed `check` mandatorily *runs* the `cuda-fma`/`cuda-no-fma` executables (`--solver cuda --gpu 0`), and this sandbox has no GPU — `FluidSolverCuda::initialize: No CUDA devices were found`. Confirmed `nvidia-smi` absent and no `/dev/dxg` / `/dev/nvidia*` device nodes are mapped into the Bash sandbox. The CUDA failure is independent of my change (it only surfaced because the build now gets far enough to reach the run stage) and I cannot resolve it without a GPU; `dangerouslyDisableSandbox` is disabled in this session.

## Status

The instance that broke the test is fixed and verified for every build that can run in this environment. `batch.py verify` cannot reach `PASS` here because its CHECK executes the CUDA binaries and no GPU device is available in the sandbox — a provisioning decision that's yours. Run the same verify command on the GPU-capable host (as `gpu_check` describes) to get the final green verdict; it should pass there now that the compile and CPU smoke are clean.

- Commit: `fc82bc1` — "Convert remaining trajectory feature call to the sampling grid struct"
- Log updated: `/home/junkun/genesis-transform-experiment/record/TRANSFORM_LOG.json` (step 56 → corrected)