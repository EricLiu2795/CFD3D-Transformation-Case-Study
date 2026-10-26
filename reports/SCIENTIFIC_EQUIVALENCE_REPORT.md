# Scientific equivalence: final raw transformed CFD3D

**Tested CUDA scope: PASS.** All eight curated phases passed960/960 required GPU evaluations, with960/960 complete outputs byte-identical to pristine. This does not prove full-program semantic equivalence or upstream scientific validity for all inputs.

Source: `RAW_TRANSFORMER_OUTPUT_LIVE_006/source`, HEAD`fc82bc131faa1922769fd501cf3e83779af1dc1c`. Scientific origin:`5943ae78be7334888b7f7b1f4622f9cf61626a2c`; original and final builds use the same documented SM120 compatibility/precision regime. Full result:`evaluation/full-final-001/FULL_FINAL_RESULTS.json`; detailed array hashes/error diagnostics:`FINAL_NUMERICAL_RESULTS.json` in that directory.

## Fixed method and actual results

The unchanged V2 oracle uses96 deterministic fixtures:12 per phase. Each phase runs five block/reduction configurations in each of two FMA modes, giving120 phase evaluations. A phase can launch several CUDA kernels; these are not960 distinct single-kernel launches or960 complete CFD simulations.

The existing mathematical references, fixture payloads and expected behaviour were retained. Numerical acceptance remains FP64 with the frozen mixed criterion`abs(actual-expected) <= 2e-11 + 2e-12*abs(expected)`, together with exact preservation checks where modification is forbidden. ULP counts and pristine-byte comparisons are diagnostics; ULP equality was not substituted for the frozen tolerance. Independent references capture the validated legacy contracts, including known backend distinctions; pristine source is not treated as universal scientific truth.

| Phase | Required evaluations | Max absolute error vs reference | Max relative error vs reference | Max ULP vs reference | Complete outputs identical to pristine |
|---|---:|---:|---:|---:|---:|
| Pressure RHS (RS) |120/120| 0 | 0 | 0 |120/120|
| Velocity correction (UVW) |120/120| 4.4408921e-16 | 1.32980506e-14 | 74 |120/120|
| Temperature |120/120| 3.72092472e-08 | 5.67768058e-13 | 5114 |120/120|
| Timestep |120/120| 0 | 0 | 0 |120/120|
| Intermediate momentum F/G/H |120/120| 2.55531631e-08 | 3.90200202e-13 | 3512 |120/120|
| SOR pressure phase |120/120| 7.27595761e-11 | 1.20030583e-13 | 768 |120/120|
| Standard boundaries |120/120| 0 | 0 | 0 |120/120|
| Scenario boundaries |120/120| 0 | 0 | 0 |120/120|

The nonzero reference errors already occur in pristine and satisfy the unchanged criterion. They must not be confused with transformation-induced errors. For final-versus-pristine buffers, maximum absolute, relative and ULP differences are all **zero** in the tested cases.

Two fresh full-application CUDA builds (FMA on/off),171-source-file admission,29-TU/link closure and exact FP64/enum/flag proof passed. Formal parameters and actual caller expressions were compared against original semantic roles, not merely C++ type strings; all eight interfaces match. No source-binding compatibility adjustment was needed. Input immutability, unwritten halo/mask preservation and memory guards passed in every required positive evaluation.

## Negative and memory/synchronization evidence

The copied-source RS no-op compiled and actually ran120 evaluations;0/120 passed numerically. Its first diagnostic was`rs-00`, FMA on, block1x1x1:RS(1,1,1) remained18.706129734039667 where the oracle expected0. All120 failed meaningfully. The mutation changed only the copied RS body, not the sealed raw source or hidden comparator. See `rs-noop/RESULT.json` and its source admission records.

Compute Sanitizer2026.1.1.0 ran80 representative instrumented phase cases:

- Memcheck and global initialization checks:64/64 PASS, split32 transformed and32 matched original-pristine.
- Synchronization and info-level race checks for timestep/pressure:16/16 PASS, split8 transformed and8 original.

All80 required explicit complete zero-error/hazard summaries, target completion and the unchanged numerical readback checks. There were zero skipped or blocked cases. Exact executable-file mounts kept historical result directories and goldens outside each executed child. These80 checks supplement rather than replace the960-case numerical matrix. See `sanitizers/SANITIZER_RESULTS.json` and `RUN_CONTRACT.json`.

## Whole-application smoke regression

Three clean builds produced six real application cases:CPU/OpenMP and CUDA with/without FMA, each for one and two driven-cavity timesteps on a4x5x6 grid. All6/6 passed, with nontrivial state changes, the expected VTK states and no recorded numerical/CUDA warnings. Each backend's complete parsed output fields matched its own immutable, repeated-determinism original baseline; CPU and CUDA were not assumed identical.

Evidence:`evaluation/full-final-app-001/APPLICATION_RESULTS.json`; exact sandbox/build/execution command:`COMMAND.json`; full logs under`record/full-final-app-001/`. Reference SHA256:`921afef995980c624859ea110417bffe68bb94f0967bb3bf7cd0faf3792d16ce`.

Upstream ASCII VTK is lossy. Exact equality here means equality of encoded finite values parsed from those files, not every internal FP64 simulation state. The full-buffer GPU oracles provide the higher-precision phase comparison.

## Integrity, coverage and interpretation

All3,446 sealed raw files remained unchanged through numerical/control/sanitizer execution. The96 fixture identities and50 frozen reference/input assets were independently verified before and after CPU tests. Those CPU-only checks passed45/45 executed tests; three pytest modules remain BLOCKED because pytest was absent, with no package installed to mask that limitation.

The four CUDA translation units other than`boundary_kernels.cu` did not change after the layout checkpoint. None of the eight held entry bodies changed after that checkpoint; the boundary/helper and shared-header closure did change. Numerical PASS therefore validates the final linked program under these inputs, but does not establish successful deep reconstruction or structural rewriting of every CUDA algorithm.

MPI/OpenCL, long-duration CFD trajectories, grid refinement, turbulent/high-Reynolds validation, large-memory scaling and complete I/O behaviour are not established. A release PGM reader bug is inherited from upstream; the geometry RAII change can add a full-file copy/allocation. Kernel tests are not a substitute for those missing application tests. No throughput or performance-preservation claim is made.

The official Transformer final result remains FAIL for mechanical/history/name/layout checks. Scientific PASS is recorded independently and does not rewrite that failure. No human scientific source repair was needed for the tested CUDA scope, and no oracle, fixture or tolerance was changed.
