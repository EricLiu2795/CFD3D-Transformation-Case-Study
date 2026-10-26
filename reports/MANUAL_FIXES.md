# Manual fixes and original failure preservation

## Human scientific source repair outcome

**None required/applied for the tested CUDA scope.** The sealed raw final output passed 960/960 phase evaluations and six full-application smoke cases. Paired sanitizer validation passed 80/80 checks: 40 on final transformed code and 40 on pristine code. No separate human-repaired scientific source tree was created because those tests found no residual source defect requiring one. The official mechanical FAIL was preserved instead of changing code/history just to turn it green.

This does not mean the first model edits were error-free. The official pipeline generated and corrected three compilation defects before raw final capture:

| Original defect | Preserved first failure | Official correction / outcome |
|---|---|---|
| S11 generic divCeil required identical argument types; CUDA callers use mixed int/unsigned |Bad commit d6b50722da; `evaluation/transform-failures/S11-original-001/` and `S11-build-diagnostics-001/`; CUDA boundary compile diagnostics |Native16 / a3ec92ae49; typed divisor conversion; pass-1 subsequently DONE. |
| S3 neighbour table omitted from OpenMP default(none) sharing |Bad commit fe7d9da899; `S3-original-001/logs/cpu-reference-build.log:50`; pressureNeighbors not specified |Native22 / aaa10078a; explicit shared clause; pass-2 subsequently DONE. |
| I1 parameter descriptor changed a callee but missed a particle/trajectory caller |Bad commit2684aef611; `I1-original-001/logs/cpu-reference-build.log:82`; const SamplingGrid& received fpvec3 |Native34 / fc82bc131f; remaining caller converted; pass-3 subsequently DONE. |

These are automatic repairs by the official configured system, not manual Codex candidate repairs. The originals, diffs, complete source archives, Git objects and compiler logs remain. No hidden numerical oracle feedback was sent to repair agents.

## Execution/configuration interventions, separately classified

- **Upstream toolchain compatibility:** three inserted/one deleted CMake lines honor explicit SM120 selection before obsolete targets. Only the experiment-compatible baseline changed; trusted scientific source and numerical algorithms did not.
- **Official optional-map incompatibility:** archive the empty optional output map after preserving the partially applied failure, then reapply only in the owned disposable source. No completed paid naming stage was rerun.
- **Project generator-hook contract:** a separate generate-only overlay returns no generated files, because CFD3D has no source generator. This accommodates the official gate's temporary directories without changing other testbed actions.
- **Local streaming execution limit:** one separately frozen900-second transport/client continuation after native2's300-second stream timeout. Original error and cost uncertainty remain.
- **Local turn-limit interference:** remove our unsolicited80-turn default for future sessions; retain explicit planning40, native$10/session and one-hour parent limits. The interrupted layout session was not rerun; a manually reviewed suffix preserved its ERROR and strict symbol FAIL.
- **Explicit user spending-policy change:** let native16 finish, pause the next admission, then activate the host-only$120 post-request control. Original$75 headers/rows/cost records are preserved; the authorized ledger header amendment is recorded. No reservations or future-spending predictions are used.

These interventions are preserved in the RAW_LIVE001–006 snapshots, `continuations/`, frozen conditions and `spending-policy-120/`. None modifies the official39-file Transformer implementation or V1/V2 baselines.

## Known issues deliberately not repaired to obtain PASS

The initial project testbed omitted CMake's `.o`→`.o.d` depfile mapping; the framework returned PASS for empty static compile coverage. The active frozen run was not retroactively altered. The future supported mapping and the fail-open/cache problems are documented in `evaluation/static-coverage-diagnosis-001/`.

The original release PGM loader has input reads inside assert (`5943:src/IO/PgmFile.cpp:52,59`; final `src/IO/pgm_image.cpp:111,119`). Those reads disappear under NDEBUG. This is inherited and outside the eight phase oracles. It was not silently fixed in transformed source while claiming baseline identity. The geometry RAII path also adds allocation/copy overhead; no performance claim is made.

History/name hygiene findings and the strict layout-symbol failure remain. Rewriting history, undoing otherwise valid outlining, or renaming identifiers solely to get a green final check would obscure the original outcome. Further maintenance can be proposed independently with this raw evidence preserved.
