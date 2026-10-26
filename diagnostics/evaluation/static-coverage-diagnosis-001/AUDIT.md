# Static impact-compile coverage diagnosis

Status: confirmed from frozen metadata and source; no compiler, Transformer or model was executed. Active source, testbed, run contract and historical reports are unchanged.

## Two separate defects

**Local adapter omission:** the frozen testbed has no per-build `depfile` mapping (`inputs/testbed.frozen.json:42–72`). The official default replaces an object's `.o` suffix with `.d` (`inputs/official/anonkit/builds.py:119–124`). CMake creates `.o.d` instead. Missing guessed files return `None` immediately (`builds.py:151–158`), before even the direct translation unit is added at171–173. Impact planning therefore finds no dependencies/candidates (`impact.py:115–130`).

**Framework admission gap:** unknown/uncovered files and preprocessing errors are findings, but only an actual nonzero compile return code sets failure (`impact.py:201–222`). With an empty compile list, `any(...)` is false, so zero coverage is reported as hard PASS. These are empty-coverage results, not successful syntax validation and not evidence that NVCC or OpenMP compilers accepted a bad change.

S11 explicitly reports `compiled:[]`, `preprocessed:0`, unknown `src/Defines.hpp` (`inputs/S11.compile.json:9–22`). S3 reports the same zero counts for the MPI pressure TU and two serial TUs (`inputs/S3.compile.json:9–31`). The historical PASS labels remain preserved.

## Frozen evidence excludes a path-root mismatch

All three compile databases, generated build.make files, and representative dependency files were hash-checked against `RAW_TRANSFORMER_OUTPUT_LIVE_005/INDEXED_BUILD_OUTPUTS.json` before copying. The tested current metadata bytes exactly match those immutable snapshot hashes; no current candidate source was inspected.

|Build|Actual TUs|TU paths under configured commands_tree|Default guessed .d files in frozen index|Actual .o.d files in frozen index|
|---|---:|---:|---:|---:|
|CPU|24|24|0|24|
|CUDA FMA|29|29|0|29|
|CUDA no-FMA|29|29|0|29|

Each database directory equals `record/work/<build>`; `commands_tree` is the correct absolute worktree root (`testbed.frozen.json:10,46,56,66`; `core.py:115,124–128`). Both serial boundary and pressure TUs are explicitly in every database. Their frozen dependency files include `worktree/src/Defines.hpp`. This is not a cwd, source-prefix, response-file or CUDA-architecture mismatch.

Important precision: the exported compile databases contain no `-MF` flag. The hash-matching generated **build.make** files provide the exact flag evidence. All three CPU rules at `inputs/<build>/build.make:107` use:

```text
-MF CMakeFiles/cfd3d.dir/src/CfdSolver/serial/boundary_conditions.cpp.o.d
```

Both CUDA rules at `inputs/cuda-{fma,no-fma}/build.make:416` use:

```text
-MF CMakeFiles/cfd3d.dir/src/CfdSolver/cuda/boundary_kernels.cu.o.d
```

`EVIDENCE.json` preserves complete selected commands, hashes and per-build counts. The schema's description claims an automatic compiler `-M` fallback (`testbed.schema.json:147`), but the implemented `deps()` missing-file path simply returns `None`; no such fallback is implemented there.

## Supported future configuration proposal — NOT APPLIED

Add this object to each of the three build entries in a separately frozen future condition:

```json
"depfile": {"from": ".o", "to": ".o.d"}
```

This is supported by `contract/testbed.schema.json:144–157` and directly consumed by `builds.py:119–124`. `FUTURE_CONFIG_PROPOSAL.json` is a proposal only. It does not modify or retrospectively repair the current experiment.

The MPI pressure TU remains genuinely outside the matrix: MPI is OFF and CMake removes that backend (`inputs/CMakeLists.frozen.txt:36–44`). Correct dependency discovery cannot create MPI/OpenCL coverage. Such exclusions need explicit reporting, rather than attributing them to the serial/CUDA mapping omission.

Fresh records are required: `batch.py:128–146` reuses historical PASS records by tree/base/toolchain, while `toolchain():88–97` hashes compiler versions and batch.py, not testbed configuration. Reusing the old provenance directory could preserve an empty cached PASS even after changing the mapping.

Actual full CPU/CUDA public builds and private GPU scientific evaluation remain separate evidence; this diagnosis neither substitutes for them nor invalidates their executed results. Current empty static PASS entries establish no additional syntax coverage.

## Optional independent audit after FINAL RAW freeze — NOT RUN

Root may choose this only after the mandatory final960GPU checks. Use a new audit namespace, an isolated copy of FINAL RAW source plus its Git metadata, fresh run/testbed files and fresh full-build directories. Preserve the sealed FINAL RAW and all original configuration/results. Point `tree` and `commands_tree` at the audit source; use absolute `builds[].cwd` values matching the fresh CMake databases; add the supported dependency rule above. Point hook output paths at the fresh audit record and keep all model admission disabled.

First verify dependency discovery for one representative header, serial TU and CUDA TU using official `builds.commands()`, `builds.deps()` and `to_tree()` APIs. Then run actual official `impact.check()`/`Report` APIs for a documented base-to-final diff that contains those representative changes. Require nonempty compiled/preprocessed data and coverage of the selected files; a PASS return alone remains insufficient. If the selected diff does not change one representative, a direct official `check_argv(..., mode='syntax')` compile of a dependent TU can test that toolchain separately without inventing source edits. NVCC flag compatibility remains untested until this is executed.

There is **no checkfiles.py CLI** in this pinned framework. A supported full-range entry point is:

```sh
# FUTURE ONLY, isolated source copy and entirely fresh record; no paid calls.
"$PY" -B "$TOOLS/batch.py" verify --run "$AUDIT/run.json" final-static-audit --base "$BASE"
```

Exact flags: `batch.py:355–378`; implementation302–341. `verify` includes public check hooks and can temporarily checkout commits during failure bisection, so it must never target the sealed raw source or active worktree. `--static` exists only for the separate `check` subcommand, not `verify`. The final adapter/API audit should preserve first failures and classify any new NVCC/preprocessor issue; do not alter the original static reports or loosen scientific acceptance.