# Independent layout-symbol explanation

**Strict official layout.symbols remains FAIL.** Separate read-only proof checks pass: all 27 reported differences have a source/ELF explanation. This is evidence for an explicit scientific checkpoint decision, not a waiver or full semantic-equivalence proof.

## Provenance and method

Baseline revision: `7a17ebe52929ab28978b1c582a1b9b0cf559d439`; source head: `665762f304386d792e8795817a7b46af166002ec`. Baseline ELFs came from sealed `RAW_TRANSFORMER_OUTPUT/record/work-base`, not a new build. All three hashes match `evaluation/layout-admission-001/BASELINE_INTEGRITY.json`. Current ELFs came from `record/work`. Inspection used `nm -C -g --defined-only` and `nm -g --defined-only`; weak globally defined symbols are included. Captured commands and raw results are retained here. All inspected input hashes were identical before/after. No application, GPU workload, model or validator was executed.

## CUDA stub differences: 24 findings, 12 pairs

`tools/anonymous/layout_check.py:65–73` renames whole identifier tokens. CUDA's generated stub identifier embeds `_Z<decimal length><target name><parameter encoding>` within a larger token, so the source identifier key does not match it. For each lost stub, this audit parses that encoding, applies the exact 743-entry source rename map, preserves its parameter suffix, and recomputes the nested target length. It then recomputes the length of the entire outer C++ stub identifier to derive the raw Itanium symbol. The derived symbol must actually occur in the head ELF; its predecessor must occur in the sealed baseline ELF, and the opposite names must be absent. No arbitrary exception lookup or symbol filtering is introduced.

| Profile | Exact source-map rename | Target-name length | Outer stub-identifier length |
|---|---|---:|---:|
| cuda-fma | `setHBoundariesCuda` → `setHBordersCuda` | 18 → 15 | 43 → 40 |
| cuda-fma | `setMountainBoundariesCuda` → `assignMountainBordersCuda` | 25 → 25 | 54 → 54 |
| cuda-fma | `setInternalTBoundariesCuda` → `assignInternalTBordersCuda` | 26 → 26 | 51 → 51 |
| cuda-fma | `setSingleTowerBoundariesCuda` → `assignSingleTowerBordersCuda` | 28 → 28 | 55 → 55 |
| cuda-fma | `setDrivenCavityBoundariesCuda` → `assignDrivenCavityBordersCuda` | 29 → 29 | 52 → 52 |
| cuda-fma | `setFlowOverStepBoundariesCuda` → `assignFlowOverStepBordersCuda` | 29 → 29 | 56 → 56 |
| cuda-no-fma | `setHBoundariesCuda` → `setHBordersCuda` | 18 → 15 | 43 → 40 |
| cuda-no-fma | `setMountainBoundariesCuda` → `assignMountainBordersCuda` | 25 → 25 | 54 → 54 |
| cuda-no-fma | `setInternalTBoundariesCuda` → `assignInternalTBordersCuda` | 26 → 26 | 51 → 51 |
| cuda-no-fma | `setSingleTowerBoundariesCuda` → `assignSingleTowerBordersCuda` | 28 → 28 | 55 → 55 |
| cuda-no-fma | `setDrivenCavityBoundariesCuda` → `assignDrivenCavityBordersCuda` | 29 → 29 | 52 → 52 |
| cuda-no-fma | `setFlowOverStepBoundariesCuda` → `assignFlowOverStepBordersCuda` | 29 → 29 | 56 → 56 |

All 24 CUDA lost/gained findings are accounted for by these exact pairings. `STUB_MAPPING.json` retains full demangled/raw symbols, suffix encodings and finding indices.

## Destructor differences: three profile-level findings

The sealed baseline contains **no** globally defined `CfdSolver::~CfdSolver()` symbol, including weak symbols. Thus W-to-T is not the observed result. Original `src/CfdSolver/CfdSolver.hpp:85` already defines `virtual ~CfdSolver() = default` inline, and its cpp only includes the header. Current `src/CfdSolver/solver_interface.hpp:18` declares it; `solver_interface.cpp:4` default-defines it out of line. Each current ELF emits strong deleting/complete/base destructor variants `_ZN9CfdSolverD0Ev`, `_ZN9CfdSolverD1Ev`, `_ZN9CfdSolverD2Ev`. The official demangled-name comparison reports one gained name per profile.

This is source-consistent absent-to-strong emission from outlining an already defaulted destructor. Its defaulted body adds no numerical operation. This is not a general linkage/ABI or complete program-semantics proof. Exact source snapshots and symbol observations are in `DESTRUCTOR_PROOF.json` and captured `git show` output.

## Decision boundary

`layout_check.py:261–283` compares strong symbols by name and `:304–307` reports FAIL when differences remain. This audit does not change those rules, snapshots, reports, source or status. Existing `evaluation/layout-admission-001` independently records rebuild, coverage, compiled-set and gates checks; its overall postamble is still FAIL because of strict symbols. Numerical/application results belong to their separately preserved evidence and were not rerun here.

Any continuation based on these independently characterized artifacts must be labeled a **manual scientific checkpoint adoption**, retaining the original official layout FAIL and Native12 resource ERROR. It is not the original full official workflow PASS. The complete first-attempt outputs remain preserved, and scientific conclusions remain bounded by independent test coverage.
