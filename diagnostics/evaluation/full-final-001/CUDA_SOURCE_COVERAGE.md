# Actual CUDA transformation coverage: limited, independently measured

The comparison is sealed layout-only RAW4 (`665762f304386d792e8795817a7b46af166002ec`) against final RAW6 (`fc82bc131faa1922769fd501cf3e83779af1dc1c`). It measures the later semantic/rewrite suffix only. It deliberately excludes earlier743 token renames, comment stripping, source-path changes and definition reordering, which are separate actual transformations.

**None of the eight held CUDA entry implementation-body byte strings changed between these snapshots.** This is source evidence, not a mathematical-failure verdict or a claim that nothing in the CUDA closure changed. See `TARGET_BODY_CHANGE_INVENTORY.json` for original/final hashes and lines.

| Actual source/header | RAW4?RAW6 file bytes | Observed braced-definition changes |
|---|---|---|
| `src/CfdSolver/cuda/boundary_kernels.cu` | changed | assignInternalTBordersCuda, assignInternalUBordersCuda, assignInternalVBordersCuda, assignInternalWBordersCuda, added computeInteriorCellIndex |
| `src/CfdSolver/cuda/boundary_kernels.hpp` | byte-identical |  |
| `src/CfdSolver/cuda/cuda_runtime_check.cu` | byte-identical |  |
| `src/CfdSolver/cuda/cuda_runtime_check.hpp` | changed | removed isLinked |
| `src/CfdSolver/cuda/cuda_solver.cu` | byte-identical |  |
| `src/CfdSolver/cuda/cuda_solver.hpp` | byte-identical |  |
| `src/CfdSolver/cuda/momentum_kernels.cu` | byte-identical |  |
| `src/CfdSolver/cuda/momentum_kernels.hpp` | byte-identical |  |
| `src/CfdSolver/cuda/poisson_kernels.cu` | byte-identical |  |
| `src/CfdSolver/cuda/poisson_kernels.hpp` | byte-identical |  |
| `src/Defines.hpp` | changed | divCeil |

The only changed CUDA translation unit is `boundary_kernels.cu`: a new `__device__ inline` helper `computeInteriorCellIndex` shares interior-thread indexing, changing the four internal U/V/W/T boundary-helper bodies; W/T helpers also acquire early-return guards. `cuda_runtime_check.hpp` removes unused device `isLinked`. All other CUDA TUs and their role-named headers are byte-identical to RAW4. Shared `Defines.hpp` changes `divCeil` to a two-type template with an explicit cast of the divisor; this is used by CUDA host launch calculations, so it is part of the tested closure.

The final root `record/TRANSFORM_LOG.json` has four directly CUDA-associated retained entries: S7 unused-flag helper removal; S2 shared index helper; S4 W guard; S4 T guard. S11 is **corrected**, with a retained template in actual source and `correction_commit=a3ec92a`. Counting only82 literal-kept entries across18 groups must not imply that S11 left no source effect. Overall planned109 operations, requested19 groups, retained/corrected log rows and actually modified CUDA functions are different quantities.

## Why zero held-body edits must not be blamed on an unproven freeze bug

Frozen `record/run.json` explicitly sets every one of the eight entries to `body: free`; scope includes all `src/*` with no source exclusions. Official `anonkit/core.py:60?61` retains the frozen-file list and pinned dictionaries as supplied. In `candidates.py:119?134`, only pins whose body is literally `frozen` are removed from all candidate groups. In `candidates.py:156?159`, pinned names are removed specifically from interface groups, as expected for unchanged public signatures. The runtime copies of both files exactly match the audited official source.

The preserved candidate pool contains **all eight held entries in S2**, plus held CUDA candidates in S3, S4 and P1. Thus they were demonstrably eligible for non-interface body transformations. The preserved plan explicitly allows pinned CUDA-body extraction (S2) and guard refactoring (S4), while preferring host-only operations in S1, P1, S5 and several interface groups. No broad machine filter freezing all pinned bodies is established by source. Actual executors retained mostly CPU/IO transformations and a small CUDA helper subset. The exact internal reason for each model selection is not inferable from these facts alone.

This single execution therefore demonstrates **limited CUDA structural coverage**, rather than19 groups rewriting every CUDA core. It does not establish general model/Transformer CUDA capability or impossibility. No further model attempt was launched to force a different result.

## Independent scientific evaluation remains valid

All eight final phases still require the full960 numerical cases because common headers, called helpers, launch wrappers and full-link closure can affect behavior. Final evaluation passes960/960 and the copied-source RS no-op fails120/120 numerical checks; those are independent correctness/admission findings, not evidence of how many source transformations occurred. The unchanged reference/tolerance and direct caller-order audit prevent interpreting an adapter mismatch as a mathematical failure. Application-level regression remains additional evidence.

Primary immutable evidence: RAW4/RAW6 `source/`, RAW6 `record/run.json`, `record/candidates.json`, `record/plan.md`, `record/plan.json`, `record/TRANSFORM_LOG.json`; official/runtime `tools/anonymous/candidates.py` and `anonkit/core.py`; complete per-file/function SHA256 inventory in `CUDA_SOURCE_COVERAGE.json`. No active source inspection, source editing, tool execution or model assistance was used to construct this inventory.
