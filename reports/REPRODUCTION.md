# Reproduction and evidence lookup — full CFD3D transformation

This records one executed full official transformation experiment. It does not authorize another paid run. Use fresh output directories for local validation; do not write into historical evidence or repeat completed model stages.

## Provenance and environment

`X=/home/junkun/genesis-transform-experiment`. Commands run in Ubuntu WSL2 on its ext4 filesystem, with NVIDIA RTX 5080 (SM120), driver 610.88, NVCC 13.2.86, GCC/G++ 15.2.0, CMake 4.2.3 and Bubblewrap 0.11.1. Model: JHU Gateway `claude-opus-4-8`, native Claude Code 2.1.292, medium effort, configured per-response output cap 32768. No alternative provider or model was used.

| Source | Exact identity |
|---|---|
| Official framework and copied Transformer | `f772f81d83f32bf852af335868379d75759681ac` |
| Scientific upstream | `5943ae78be7334888b7f7b1f4622f9cf61626a2c` |
| CMake-only compatibility baseline | `7a17ebe52929ab28978b1c582a1b9b0cf559d439` |
| Official comment stripping | `e70328736481eb4d1c3cc6785658c6d9d2f62a8b` |
| Lexical/path rename | `dcb8fd09a7f8cad74de7989146289865e6e54479` |
| Layout checkpoint | `665762f304386d792e8795817a7b46af166002ec` |
| Final transformed source | `fc82bc131faa1922769fd501cf3e83779af1dc1c` |

The final repository is `X/worktree`, detached and clean. The independently evaluated immutable source is `X/RAW_TRANSFORMER_OUTPUT_LIVE_006/source`. No V1/V2 branch was updated. The trusted scientific source is `X/pristine` at the pinned upstream commit. Architecture compatibility changes are in `planning/BUILD_COMPATIBILITY.diff`; no upstream numerical implementation was edited by the supervisor.

Approved dependencies were installed only in the experiment Python 3.14 environment: `tree-sitter==0.25.2` and `tree-sitter-cpp==0.23.4`, verified prebuilt wheel contents, import and parser tests. See `dependency-install/INSTALLATION.json` for wheel URLs, checksums and RECORD verification. No system packages were installed. Optional LLVM/Clang transformations were NOT RUN because LLVM/Clang was unavailable. Numerical validation uses the existing private numerical-validation virtual environment, including NumPy 2.5.4; it was not reinstalled or changed.

## Actual official commands and continuation history

The official inner command executed through the frozen local boundary was:

```sh
"$X/venv/bin/python" -B "$X/tools/anonymous/runner.py" all \
  --config "$X/record/runner.json"
```

After preserved layout execution failure and explicit checkpoint admission, the supported suffix was:

```sh
"$X/venv/bin/python" -B "$X/tools/anonymous/runner.py" all \
  --config "$X/record/runner.json" --from pass-1
```

These are historical inner argv, not safe standalone launch instructions and not authorization to replay model calls. The outer wrappers isolate network/filesystem access and credentials; calling the inner command on an unrestricted host would not reproduce that boundary. Exact outer and inner argv are retained under `session-control/transport/*/{HOST_COMMAND,OFFICIAL_COMMAND}.json`. The final policy-controlled continuation is recorded under `spending-policy-120/` and `session-control/transport/spending120-001/`.

Original configuration hashes and execution-boundary hashes are in `planning/EXPERIMENT_FREEZE.json` and `planning/EXECUTION_BOUNDARY_FREEZE.json`. Separate immutable continuation conditions record timeout recovery, the project-specific no-generator hook, removal of a local unsolicited 80-turn default, administrative checkpoint admission and the user-authorized spending policy update. The official explicit planning turn limit remained unchanged. Completed stages were not automatically replayed. Native sessions 2 and 12 remain errors in the evidence, and layout remains a failed official stage; continuation did not relabel either as PASS.

The existing host-only relay authenticates with JHU `x-api-key`; workers receive only a placeholder and isolated loopback transport. Private oracles and prior synthesis evidence were not mounted into model sessions. Do not archive or print the private Gateway environment file.

## Spending records

The active local policy changed from a $75 completed-session soft stop to the user-authorized $120 cumulative post-request cutoff. `spending-policy-120/REQUEST_LEDGER.json` accounts for all reported experiment usage: the first 16 sessions form one historical baseline, later completed requests are settled once, and session totals only reconcile those receipts. It does not reserve prospective spending. At $120 it would block the next paid forward with `BUDGET_CUTOFF_REACHED`; one already completing request may overshoot, as authorized.

The run ended below the cutoff, at estimated **$91.17033050**, with no in-flight request. See `COST_AUDIT.json`. This is an API-rate estimate, not a JHU invoice. The original interrupted native-2 request may have unreported billable usage; no amount was invented for it. Further paid execution requires new authorization.

## Offline numerical reproduction

Existing results should normally be inspected rather than regenerated. A fresh eight-phase local GPU verification can use the validated adapter and frozen contract:

```sh
X=/home/junkun/genesis-transform-experiment
P=/home/junkun/genesis-curator-rs-pilot/reviewed-tasks/rs-native-002-reviewed-001/numerical-validation/venv/bin/python
OUT="$X/evaluation/replay-$(date -u +%Y%m%dT%H%M%SZ)"
"$P" -B "$X/control/transformed_scientific_v2.py" run \
  --contract "$X/evaluation/full-final-001/contract-bound/contract.json" \
  --out "$OUT" \
  --phase rs uvw temperature dt fgh boundary scenario pressure
```

This command uses the already bound source/ABI manifest; it fails closed if immutable inputs changed. It does not call a model. The adapter compiles trusted private harness assets against exact frozen transformed source and runs 12 fixtures × 5 block configurations × 2 FMA modes per phase. No fixture regeneration or tolerance changes are needed. Full formal parameter order and actual callers were checked, not merely equal types; see `API_CALLER_REVIEW.json` and `contract-bound/ABI_PROOF.json`.

The completed final results are:

- `evaluation/full-final-001/FULL_FINAL_RESULTS.json`: compact summary and preserved scope limitations.
- `evaluation/full-final-001/FINAL_NUMERICAL_RESULTS.json`: detailed 960 numerical readbacks and pristine comparisons.
- `evaluation/full-final-001/{rs,remaining-seven}/RESULT.json`: compiler commands, binaries and per-case outcomes.
- `evaluation/full-final-001/rs-noop/RESULT.json`: copied-source no-op, compilation PASS, all 120 numerical cases rejected.
- `evaluation/full-final-001/sanitizers/{RUN_CONTRACT,SANITIZER_RESULTS}.json`: exact binary/fixture hashes and 80 paired sanitizer outcomes.

A fresh sanitizer replay can use the same validated helper, preserving both conditions:

```sh
"$P" -B "$X/control/run_sanitizer_matrix.py" \
  --baseline-contract "$X/evaluation/full-final-001/sanitizer-baseline/contract-bound/contract.json" \
  --baseline-result "$X/evaluation/full-final-001/sanitizer-baseline/compile/RESULT.json" \
  --transformed-contract "$X/evaluation/full-final-001/contract-bound/contract.json" \
  --transformed-result "$X/evaluation/full-final-001/rs/RESULT.json" \
  --transformed-result "$X/evaluation/full-final-001/remaining-seven/RESULT.json" \
  --out "$X/evaluation/sanitizer-replay-$(date -u +%Y%m%dT%H%M%SZ)"
```

This runs memcheck/global initcheck across all eight phases and synccheck/info racecheck on timestep/pressure for both original and transformed code. It requires explicit zero-error summaries and successful numerical readback; unavailable/skipped execution is not PASS. It does not establish race freedom for all possible inputs.

## Full application and source checks

The final independent clean application run is `evaluation/full-final-app-001/COMMAND.json`; its complete Bubblewrap argv can be inspected without executing it. It mounts final source read-only and a fresh work directory writable, with no network, credentials, model CLI, hidden oracle or mutable active repository. `APPLICATION_RESULTS.json` records three fresh builds and six 4×5×6 driven-cavity cases: CPU/OpenMP, CUDA FMA and CUDA no-FMA, each one/two timesteps. The immutable original reference is `RAW_TRANSFORMER_OUTPUT/record/work-base/public-baseline.json`. Parsed finite ASCII VTK output matches within each original backend. This is a bounded smoke regression, not broad CFD validation or a CPU/GPU identity claim.

`evaluation/python-reference-final-001/PYTHON_REFERENCE_RESULTS.json` records 45 executed tests and 96 unchanged fixtures. Three pytest-dependent modules were BLOCKED; no test framework was installed or substituted. `evaluation/protected-final-001/RESULT.json` and `PROTECTED_WINDOWS_FINAL.json` verify protected originals, framework files and Windows branches.

The official final acceptance is independently inspectable at `record/checks/final_check.json`. It is FAIL despite numerical PASS. `evaluation/static-coverage-diagnosis-001/` explains why earlier static compile labels covered zero translation units; it is not a repaired active configuration. `evaluation/layout-symbol-explanation-001/` explains, without waiving, the cached strict symbol differences. No further model run is needed to inspect these findings.

## Archives and checksums

The final raw output is frozen separately from evaluation and later reports: `RAW_TRANSFORMER_OUTPUT_LIVE_006/`, 3446 files, manifest SHA-256 `1512172728ad50d499f2e904590c8efbef9b46d8da3d319d5c1d42b01bd40b6a`. `evaluation/full-final-001/RAW_READBACK_AFTER_EVALUATION.json` verifies it remained unchanged.

The pre-transform archive remains under `backups/` and Windows `C:\Users\junku\Genesis_Mission_Backups\cfd3d-pre-transform-20261025T202752Z\`. Final capture is designed for `archives/full-run-001-20261026/`, mirrored to `C:\Users\junku\Genesis_Mission_Backups\cfd3d-full-transform-full-run-001-20261026\`. Read `READBACK.json` in that directory for actual final archive checksum and verified completion; this report is written before capture to avoid circular checksums. The final archive excludes real credentials, private HOME settings, vendor binary/environment duplication and historical large archives. Original archives remain referenced and unchanged.

Copying reports alone does not preserve executable reproduction; source snapshots, mappings, configuration, exact private oracle assets, compiler environment and numerical evidence are also required. Keep mappings, Git ancestry and original source out of any future synthesis workspace. Creating new Curator tasks on this transformed source is a separate, not-yet-authorized experiment.
