# Actual transformation changelog

Final source: `fc82bc131faa1922769fd501cf3e83779af1dc1c`, sealed in `RAW_TRANSFORMER_OUTPUT_LIVE_006/source/`. This inventory distinguishes committed source changes, model provenance counts, and analysis metrics. It does not turn a configured stage or a planned slot into an applied transformation.

## Applied stages and counts

- The source export contains171 files, including82 under`src/`; the original export had172/83. The reduction comes from module composition, not removal of an entire scientific backend.
- Main lexical map:743 identifier/path rename pairs;82 original paths affected, with49 old paths replaced by49 new paths and33 paths edited in place. The map categories include201 function-name tokens,319 variables,88 macros,17 types,6 members,19 filename tokens and93 other names. These are map-entry counts, not counts of unique function definitions or semantic rewrites.
- Layout: two actual commits and48 layout log entries. The composed FILE_MAP records60 original paths,16 symbol destinations and23 kept-reason entries. Observed operations include module renaming/moves, a serial scenario-boundary split, merging string/file utility modules, definition reordering and three out-of-line member definitions.
- Semantic source rewrites: all19 configured group sessions ran. Their final log has85 entries:82`kept` and3`corrected`. The original budget was109 slots. Corrected entries are source changes whose original defects were repaired by the official system; they are not discarded experiments or pristine first drafts. The final checker's82-kept distribution excludes the three corrected rows.

Evidence: `evaluation/final-structure-inventory-001/RESULT.json`, `history.txt`, `semantic-diff.patch`; `record/rename/rename_map.json`; `record/FILE_MAP.json`; `record/layout/TRANSFORM_LOG.json`; `record/TRANSFORM_LOG.json`. The detailed sealed-layout inventory, including its complete60-path mapping, remains in `drafts/CHANGELOG_PARTIAL.md`.

| Group | Kept log entries | Corrected entries | Observed operation family |
|---|---:|---:|---|
| S7 | 3 | 0 | Remove unused helpers |
| S11 | 0 | 1 | Generic integer division helper; CUDA mixed-type call compatibility corrected |
| S2 | 10 | 0 | Factor shared solver and I/O helpers |
| S3 | 2 | 1 | Table-driven neighbour sums; OpenMP sharing corrected |
| S1 | 3 | 0 | Standard algorithms for output-buffer conversion |
| S4 | 6 | 0 | Flatten conditionals with guard clauses |
| P1 | 8 | 0 | Precompute invariant grid/output quantities |
| S5 | 4 | 0 | RAII temporary I/O buffers |
| R4 | 1 | 0 | Explicit aggregate initialization |
| R1 | 4 | 0 | Guard empty buffer-copy ranges |
| R3 | 5 | 0 | I/O error handling |
| I1 | 7 | 1 | Parameter descriptor structs; missed caller corrected |
| I2 | 8 | 0 | Consistent helper parameter order |
| I3 | 3 | 0 | Computed outputs returned by value |
| I4 | 3 | 0 | Lightweight staggered-field views |
| I5 | 5 | 0 | Helper scope/internal linkage organization |
| I6 | 4 | 0 | Explicit modes in place of bool flags |
| I7 | 3 | 0 | Geometry/topology scalar structs |
| S8 | 3 | 0 | Named field-view and collection types |

These counts describe retained provenance. They are not85 independent semantic-equivalence proofs. The independent tests exercise the final linked application/helper closure, and their limits are stated in the scientific report.

## Source and build organization

The post-layout tree uses role-oriented names such as `src/CfdSolver/serial/`, `src/CfdSolver/cuda/`, `solver_interface`, `cell_flags`, `simulation_main.cpp`, and lowercase I/O/particle module filenames. The serial scenario-boundary function moved into its own source. Two utility modules merged into `IO/text_file_utils.{hpp,cpp}`. No numerical algorithm was removed to create that merge.

The content-layout commit reordered definitions and moved three small inline/defaulted members out of headers. An independent layout comparison found81 unchanged braced bodies across eight inspected files, with58 changed ordinal positions. The base destructor's new out-of-line definition emits a strong symbol absent from the original executables. That real symbol-set change and the CUDA stub-name comparison problem explain the preserved strict layout failure; they do not establish universal ABI equivalence.

The only CMake source difference from upstream is the explicit-SM120 compatibility patch (three inserted lines, one deleted). The existing recursive source discovery found the relocated files. The CPU and CUDA matrix disables MPI/OpenCL. Their source remains in the full repository, but numerical/backend compatibility for MPI/OpenCL has not been validated.

## Actual CUDA coverage

All eight held entry names and interfaces were retained, with bodies explicitly configured`free`, not frozen. Independent formal-parameter and caller-expression review passed. Nevertheless, direct comparison between the sealed layout checkpoint and the final output finds **zero changed bodies among those eight entry definitions**. That excludes the earlier identifier replacements and path/layout operations, and does not mean the linked helper closure was unchanged.

Among the five CUDA translation units, only`boundary_kernels.cu` changed after layout. `cuda_runtime_check.cu`, `cuda_solver.cu`, `momentum_kernels.cu` and`poisson_kernels.cu` are byte-identical to that checkpoint. Shared headers and host/caller code changed. Thus the run genuinely executed the full model-driven menu, but it provides **limited evidence of structural transformation of the principal CUDA numerical bodies**. It must not be presented as19 rewrites of every device kernel.

See `evaluation/full-final-001/TARGET_BODY_CHANGE_INVENTORY.json`, `API_CALLER_REVIEW.json` and the final CUDA/helper coverage inventory. Interface groups intentionally exclude pinned signatures (`tools/anonymous/candidates.py:119–120,157`); other bodies remain eligible. Selection outcomes and constraints must be reported rather than attributed to an unverified universal CUDA capability.

The optional deterministic Clang stage was NOT RUN. Tree-sitter was used for facts/candidate parsing; installing it does not imply a deterministic CUDA AST rewrite occurred.

## Distance from original source

The official final checker failed before the scripted distance stage. Afterwards, the unchanged official `distance.py` was run independently at the exact final commit against scientific upstream5943, label`independent-final-001`. This static analysis made no model/compiler/GPU calls and did not change source.

| Official metric | Result |
|---|---:|
| Upstream functions / tokens considered |241 /89,346|
| Project identifier names remaining |76/740 (10.3%)|
| Original-name occurrence share |38.6%|
| Token fingerprint share remaining |96.4%|
| Best-match token similarity |89.6%|
| AST subtree share remaining |95.1%|
| Distinctive fingerprint share remaining |91.1%|
| Functions unchanged by fingerprint measure |158/241|
| Functions with a recorded transformation |110/241|

For `src/CfdSolver`, token/AST shares remain99.3%/98.2%. Recorded transformations and unchanged normalized fingerprints can overlap; these are different measurements, not disjoint categories. The metrics are heuristic source comparisons, not proof of semantic equivalence, memorization resistance, or model capability. Source: `record/distance/independent-final-001.{json,md}`; exact command/status: `evaluation/independent-distance-001/`.

The strong identifier reduction should therefore not be confused with extensive algorithmic shape change. Eight public entry names/types and original README/scientific documentation/scenarios remain. Further research should explicitly distinguish context/name transformation from deep CUDA algorithm restructuring, with a new reviewed condition if desired. No additional paid run was started to improve these numbers.

## Ownership and unsupported behaviour

The model introduced RAII buffers, typed modes and descriptor/view structs. The immutable ownership review found the geometry stream's const-void constructor deep-copies, so the new vector-based path has separate owners and safe lifetimes; it also adds a full-file allocation/copy and can increase peak memory. No zero-copy or performance-preservation claim is justified.

The original release PGM reader places required input reads inside`assert`, which disappears under`NDEBUG` (`5943:src/IO/PgmFile.cpp:52,59`). The issue remains present in the reviewed intermediate transformations. It is inherited, lies outside the eight GPU phase oracles, and is not evidence of a newly introduced GPU arithmetic defect. Final-source I/O limitations and any bounded characterization are reported separately; original input behaviour was not silently repaired.
