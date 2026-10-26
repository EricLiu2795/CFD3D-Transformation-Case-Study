# Source and experiment provenance

| Role | Repository / exact commit |
|---|---|
| Original scientific upstream | https://github.com/BOWANGESM/CFD_3D_Solver_CUDA — `5943ae78be7334888b7f7b1f4622f9cf61626a2c` |
| BEFORE compatibility baseline | `7a17ebe52929ab28978b1c582a1b9b0cf559d439`; tree `6ff16d1e442b5dd774695fdf6770ea982ed49b7b` |
| AFTER raw final output | `fc82bc131faa1922769fd501cf3e83779af1dc1c`; tree `7e655a0353be69c4dd6db22a3970cbbf8b28521f` |
| Official Transformer | https://github.com/MM0n5Ter/Noether — `f772f81d83f32bf852af335868379d75759681ac` |

BEFORE contains **172 tracked regular files**, AFTER **171**. Both are exact Git-blob exports, including original executable-mode bits. Every AFTER file also matches the sealed `RAW_TRANSFORMER_OUTPUT_LIVE_006/source` bytes and recorded source hash. No mismatch was found. `SNAPSHOT_VERIFICATION.json` gives per-file Git blob IDs, SHA-256 values, sizes and modes. `FILE_HASHES.json` covers the complete case-study payload except itself.

This new repository has one publication commit; it does not contain the original experiment as Git ancestry. Original history text, diffs, commit identities and full local archive references are retained separately.

## BEFORE is not pristine upstream

The only changed path from the original scientific commit to BEFORE is `CMakeLists.txt`: a three-insertion/one-deletion compatibility patch honors explicit `CMAKE_CUDA_ARCHITECTURES=120` rather than legacy hardcoded architecture targets. The exact pre-existing patch is `SM120_COMPATIBILITY.patch`. It does not change numerical algorithms. Official comment stripping and subsequent transformations occurred after this baseline and are included in the BEFORE/AFTER comparison.

## AFTER is raw official output

AFTER contains the output of all 19 configured rewrite groups and three built-in compilation-repair sessions. Those automatic repairs are part of the official output. There were **zero supervising-human scientific-source repairs**. Official final mechanical acceptance was FAIL; later independent scientific PASS does not relabel that outcome. No source edits, model calls, builds or scientific tests were made to package these snapshots.

## Local originals and verified archive

- Experiment root: `/home/junkun/genesis-transform-experiment/`.
- BEFORE Git objects: `worktree/` and independently clean `baseline/`, at `7a17ebe...`.
- AFTER Git objects: clean detached `worktree/`, at `fc82bc1...`.
- Frozen raw source/evidence: `RAW_TRANSFORMER_OUTPUT_LIVE_006/`.
- Raw manifest SHA-256: `1512172728ad50d499f2e904590c8efbef9b46d8da3d319d5c1d42b01bd40b6a`.
- Full archive: `/home/junkun/genesis-transform-experiment/archives/full-run-001-20261026/FULL_RUN_EVIDENCE.tar.gz`.
- Archive SHA-256: `d72b566d7e575a35947b11b4e0cc987eefe4c66a3ecf148c1b067c8fdbcccbc3`.
- Windows archive copy: `C:\Users\junku\Genesis_Mission_Backups\cfd3d-full-transform-full-run-001-20261026\`.

The full archive, original nested Git metadata, binaries, raw model/tool trajectories and hidden numerical references are not uploaded. `ORIGIN_INDEX.json` records exact local originals for selected evidence. Original report copies remain byte-for-byte unchanged. `PATH_RESOLUTION.md` explains their paths and incomplete selected subtrees.
