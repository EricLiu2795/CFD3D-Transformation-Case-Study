# CFD3D full-repository transformation case study

This private research package documents one application of the **official Noether full-repository Transformer** to [CFD_3D_Solver_CUDA](https://github.com/BOWANGESM/CFD_3D_Solver_CUDA). It lets maintainers inspect actual source changes, limited CUDA transformation depth and final-check failures. It is a historical case study, not a repaired Transformer, new benchmark or new model experiment.

## Navigate

| Directory | Contents |
|---|---|
| [`before/`](before/) | Exact 172-file build-compatible baseline, including source, headers, documentation, inputs and original license |
| [`after/`](after/) | Exact 171-file raw final output, including the official pipeline's automatic repairs |
| [`reports/`](reports/) | Ten unchanged final reports, machine-readable results and cost audit |
| [`diagnostics/`](diagnostics/INDEX.md) | Actual final checks, maps, plans, group records, selected failures, repair diffs, CUDA depth and validation evidence |
| [`provenance/`](provenance/COMMITS.md) | Commits, compatibility patch, per-file verification, SHA-256 inventory and original-evidence index |

Start with [maintainer feedback](reports/TRANSFORMER_FEEDBACK.md), [diagnostic index](diagnostics/INDEX.md) and [official final_check.json](diagnostics/record/checks/final_check.json). A separate [path guide](provenance/PATH_RESOLUTION.md) resolves local-only references in unchanged reports. The original Chinese [final summary](reports/FINAL_SUMMARY.md) is retained.

## Main results

| Dimension | Observed outcome |
|---|---|
| Official execution | All **19 configured structural rewrite groups** ran in three completed rewrite batches; original layout errors remain recorded |
| Official final mechanical acceptance | **FAIL**: history.shape, history.tells, code.names, cached layout.symbols; plan.log-covers-diff also fails at warning severity |
| Independent numerical checks | **960/960 GPU phase evaluations PASS**, 120 per curated phase |
| Complete output comparison | **960/960 byte-identical** to pristine output on executed fixtures |
| CUDA memory/synchronization checks | **80/80 paired**, 40 transformed + 40 pristine |
| Tiny application regression | **6/6 PASS**, three fresh CPU/CUDA builds |
| Generated-code compilation defects | **Three**, automatically repaired by the official pipeline; original failures retained |
| Supervising-human scientific source repairs | **Zero** |
| Agent execution | 34 native Claude sessions: 32 normal, two abnormal sessions preserved |
| Reported API-rate spending estimate | **$91.17033050**, not verified invoice-level JHU billing |
| CUDA structural depth | Limited: **0/8 held entry bodies changed after layout**, **1/5 CUDA TUs** changed during the structural suffix |

The workflow genuinely performed model-driven naming, layout and structural rewrites. Counts are distinct: **743 lexical mapping pairs** are not 743 functions; **109 planned slots**, **48 layout records** and **85 semantic records (82 kept + three corrected)** are different quantities. There were 49 renamed source paths and 33 additional in-place affected paths. Independent official distance analysis found token similarity **0.964**, AST similarity **0.951** and **158/241 unchanged fingerprints**. Those results do not establish strong anonymization or repository-memory resistance.

Scientific PASS does not erase mechanical FAIL. Cached layout symbols are not a fresh final ABI comparison. All groups running does not imply every scientific kernel was substantially rewritten. The effect of the failed layout session/turn cap on final depth remains unresolved by this single experiment.

## Main questions for the maintainer

1. Which history/log/identity contract should satisfy `history.shape`, `history.tells` and `code.names`, especially for preparation commits and corrected entries?
2. Should `layout.symbols` reuse an earlier failed verdict after HEAD changes? How should compiler-generated CUDA stub names and out-of-line destructor emission be interpreted?
3. Why was CUDA structural coverage limited despite all groups running? How should candidate eligibility, pinned APIs, budgets and actually changed bodies be reported?
4. Did the failed layout stage or locally injected 80-turn cap materially affect transformation depth? The available run does not isolate that cause.
5. How should empty static compilation coverage fail closed? Which depfile/configuration and cache-key checks should be mandatory?
6. Which independently frozen settings might encourage meaningful, scientifically safe restructuring without changing numerical definitions or retrying this historical result to improve it?

Findings distinguish framework behavior, local configuration/resource problems, model-generated code defects and scientific limitations. They are not all attributed to Transformer bugs.

## Source provenance and reproducibility

| Input/output | Commit |
|---|---|
| Scientific upstream | `5943ae78be7334888b7f7b1f4622f9cf61626a2c` |
| BEFORE compatibility baseline | `7a17ebe52929ab28978b1c582a1b9b0cf559d439` |
| AFTER raw final output | `fc82bc131faa1922769fd501cf3e83779af1dc1c` |
| Official Noether Transformer | `f772f81d83f32bf852af335868379d75759681ac` |

BEFORE differs from pristine upstream only by the [SM120 CMake compatibility patch](provenance/SM120_COMPATIBILITY.patch). It is not falsely labeled pristine. Both exports are exact Git-blob snapshots; all AFTER bytes also match frozen raw final source. [Snapshot proof](provenance/SNAPSHOT_VERIFICATION.json) includes modes and Git blobs; [FILE_HASHES.json](provenance/FILE_HASHES.json) inventories the entire package except itself.

Inspect the source without executing code:

```sh
git diff --no-index --stat before after
git diff --no-index -- before/CMakeLists.txt after/CMakeLists.txt
# --no-index exits 1 when differences exist; this is expected.
```

A [full source comparison](diagnostics/source-diff/before-to-after.patch) is included. Historical build environment: Ubuntu WSL2, RTX 5080/SM120, driver 610.88, NVCC 13.2.86, GCC/G++ 15.2.0, CMake 4.2.3, FP64. Model: `claude-opus-4-8`, native Claude Code 2.1.292. Optional LLVM/Clang was unavailable and its stage was NOT RUN. [Reproduction report](reports/REPRODUCTION.md) records historical commands and boundaries; it is not authorization to launch new paid work.

Numerical acceptance used unchanged mathematical references, 96 deterministic fixtures, existing tolerances, input/halo preservation and buffer guards. The 960 evaluations are **phase-level**: 12 fixtures × five block configurations × two FMA modes for each of eight phases. They are not 960 full CFD simulations. The six application cases are tiny smoke tests with lossy ASCII VTK comparison. Neither proves universal scientific/ABI/performance equivalence, long simulation correctness or MPI/OpenCL coverage. Three pytest modules were blocked; no skipped mandatory check is represented as PASS.

The full private local archive is not included. Hidden oracle implementations, fixture payloads, raw trajectories, binaries, nested Git histories and unrelated research remain local. [Evidence limitations](diagnostics/INDEX.md#evidence-limitations) explain what is missing for full executable reproduction. [Path resolution](provenance/PATH_RESOLUTION.md) preserves useful references without rewriting scientific results. [Attribution](ATTRIBUTION.md) and both scientific licenses remain intact.

Keep this repository private for maintainer review. It deliberately contains target implementations, maps and debugging evidence, so it must not be mounted into a future synthesis-agent workspace.
