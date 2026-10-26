# Maintainer diagnostic index

All copied originals are unmodified and indexed in `../provenance/ORIGIN_INDEX.json`. No transformation, build, GPU test or model was rerun for publication.

## 1. Official final mechanical FAIL

Start with [final_check.json](record/checks/final_check.json): four hard failures (`history.shape`, `history.tells`, `code.names`, `layout.symbols`) and one warning-severity failed check (`plan.log-covers-diff`). Supporting originals include [layout_check.json](record/checks/layout_check.json), [plan_check-log.json](record/checks/plan_check-log.json), [tells_lint-code.json](record/checks/tells_lint-code.json), and the [original experiment history](RAW_TRANSFORMER_OUTPUT_LIVE_006/git-history.txt).

**Cached symbol caveat:** `layout.symbols` reuses a verdict at layout HEAD `665762f304386d792e8795817a7b46af166002ec`, not a fresh final-binary comparison at `fc82bc1...`. The [stored verdict](record/layout/symbols/verdict-665762f30438.json), baseline `.nm`/`.tus` text, and [independent explanation](evaluation/layout-symbol-explanation-001/AUDIT.md) are included. That explanation derives 12 lost/gained CUDA-stub pairs (24 findings) and three destructor-symbol gains. It does not waive FAIL or prove universal ABI equivalence. ELFs remain local-only.

## 2. Planned settings versus observed source changes

- [runner.json](record/runner.json): 19 groups in three batches, model/resource settings and configured repair/naming retries.
- [run.json](record/run.json), [testbed.json](record/testbed.json), [pinned interfaces](public/abi-freeze.txt): scope, context and build matrix. These historical configurations are not portable launch instructions.
- [plan.json](record/plan.json): 109 planned instance slots, not 109 successful source edits.
- [743 lexical pairs](record/rename/rename_map.json): functions, variables, macros, types, members, filename and generic tokens; not 743 functions.
- [FILE_MAP.json](record/FILE_MAP.json) and [verified inventory](evaluation/final-structure-inventory-001/RESULT.json): 49 renamed source paths plus 33 additional in-place affected paths.
- [48 layout records](record/layout/TRANSFORM_LOG.json), [layout plan](record/layout/plan.md) and [moves](record/layout/moves-1.json).
- [85 semantic records](record/TRANSFORM_LOG.json): 82 literal-kept and three corrected entries. Corrected entries were repaired by the official pipeline; their source effects were not necessarily reverted.
- [runner state](record/runner/state.json), batch logs and `record/verify/pass-*/tests.jsonl`: three rewrite batches completed; layout/final errors remain. `runner/calls.jsonl` contains blocked/offline rows and is not a paid-request counter.
- [CUDA depth](evaluation/full-final-001/CUDA_SOURCE_COVERAGE.md), [entry-body inventory](evaluation/full-final-001/TARGET_BODY_CHANGE_INVENTORY.json): 0/8 held entry bodies changed **after layout**, and only 1/5 CUDA TUs changed in the structural suffix. Four internal boundary kernels and a new helper did change. Earlier lexical/path/layout changes remain real. Signature pinning does not establish that all bodies were accidentally excluded.
- [Independent official distance analysis](record/distance/independent-final-001.json): token similarity 0.964, AST 0.951, 158/241 unchanged fingerprints. The pipeline stopped before its own distance stage; this later independent invocation used the unchanged tool with no model calls.

Group settings, plan slots, log records, renamed paths and changed function bodies are distinct measures. One run does not causally establish whether the failed layout stage or local turn cap limited eventual transformation depth.

## 3. Abnormal sessions and automatic repairs

The [call-002](session-control/call-002/native.stdout.json) and [call-012](session-control/call-012/native.stdout.json) objects are small terminal metadata, **not raw tool transcripts**. Call 2 has `is_error=true`/API error despite subtype `success`. Call 12 hit an injected local 80-turn cap at 81 reported turns. Neither original error was relabeled success. The reports document subsequent explicit checkpoint/resource changes; the workflow was not an uninterrupted clean PASS.

| Group | Original compiler evidence | Official automatic repair |
|---|---|---|
| S11 | Mixed int/unsigned template deduction; `evaluation/transform-failures/S11-build-diagnostics-001/logs/cuda-fma-build.log` | [a3ec92ae49](automatic-repairs/S11-official-repair.patch) |
| S3 | Missing OpenMP shared variable with default(none); `evaluation/transform-failures/S3-original-001/logs/cpu-reference-build.log` | [aaa10078a5](automatic-repairs/S3-official-repair.patch) |
| I1 | Descriptor refactor missed cross-file caller; `evaluation/transform-failures/I1-original-001/logs/cpu-reference-build.log` | [fc82bc131f](automatic-repairs/I1-official-repair.patch) |

Bad-commit patches, batch diffs, check logs, successful verifier records and the three model-written repair summaries are included. Supervising-human scientific source repairs: **zero**. Full bad-source tars, nested Git metadata and full repair trajectories remain local-only.

## 4. Numerical, sanitizer and application evidence

- [Compact final results](evaluation/full-final-001/FULL_FINAL_RESULTS.json) and [per-case numerical results](evaluation/full-final-001/FINAL_NUMERICAL_RESULTS.json): 8 phases × 12 fixtures × 5 block settings × 2 FMA modes = **960/960 PASS**; all 960 complete outputs byte-identical to pristine. These are finite phase-level checks, not complete CFD simulations.
- [API/caller review](evaluation/full-final-001/API_CALLER_REVIEW.json): complete formal order and actual argument roles, not merely equal types. Raw source was hash-checked after evaluation.
- [Sanitizer results](evaluation/full-final-001/sanitizers/SANITIZER_RESULTS.json) and [contract](evaluation/full-final-001/sanitizers/RUN_CONTRACT.json): **80/80 paired**, 40 transformed + 40 pristine. Memcheck/global initcheck cover all eight phases; synccheck/info racecheck target timestep/pressure. Not a universal race-freedom proof.
- [Application results](evaluation/full-final-app-001/APPLICATION_RESULTS.json): three clean builds, six tiny 4×5×6 driven-cavity cases, CPU/CUDA-FMA/CUDA-no-FMA, one/two timesteps. Same-backend parsed ASCII VTK matched original; ASCII output is lossy. Long simulation behavior and CPU/GPU identity were not established.
- RS no-op compiled and executed but passed **0/120**; summary evidence is retained. Hidden reference code, fixture payloads and full golden buffers are not uploaded.

## 5. Static PASS with zero compiled TUs

Read [AUDIT.md](evaluation/static-coverage-diagnosis-001/AUDIT.md), [EVIDENCE.json](evaluation/static-coverage-diagnosis-001/EVIDENCE.json), and exact small reports under `record/provenance/{S11,S3,I1}`. CMake generated `.o.d`; the testbed omitted its supported mapping and the framework guessed `.d`. Missing deps returned before adding the direct TU. The impact checker failed only on an actually selected nonzero compiler exit, so an empty selection could PASS. Testbed configuration was absent from cache keys. Full batch builds did execute and caught the defects.

Saved compile databases, build.make, `.o.d` files and frozen configuration substantiate the diagnosis. `FUTURE_CONFIG_PROPOSAL.json` is a proposal, not an applied fix. Duplicated `inputs/official/` code evidence is omitted; source citations identify official commit `f772f81...`. Original checksum manifests may reference omitted files; the case-study inventory covers only the actual publication.

## Evidence limitations

This selection omits raw model/tool trajectories, authentication/HOME files, unrelated research, private oracle implementations, fixture payloads, full numerical buffers, compiled ELFs/objects, failed-source tars and the full 718 MB archive. It supports inspection of source, maps, plans, failures, repair changes and recorded scientific outcomes; it is not independently executable hidden evaluation. Exact local references and archive checksum appear in `../provenance/COMMITS.md` and `ORIGIN_INDEX.json`.

Whether the failed layout/turn cap caused reduced depth requires a separately authorized controlled experiment. Semantic equivalence beyond tested fixtures, all MPI/OpenCL behavior, performance and general memorization resistance remain unestablished. Missing selected evidence is not fabricated or silently counted as PASS.
