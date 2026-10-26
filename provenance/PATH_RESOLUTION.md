# Resolve historical report paths

All ten files under `reports/` are exact copies of final research reports. Their local paths, quoted commands, conclusions and numerical results have not been rewritten.

| Original reference | This package |
|---|---|
| `/home/junkun/genesis-transform-experiment/reports/full-run-001/<name>` | `reports/<name>` for the ten selected reports |
| Original `record/<path>` | `diagnostics/record/<path>` when selected |
| Original `evaluation/<path>` | `diagnostics/evaluation/<path>` when selected |
| Selected call-002/call-012 terminal metadata | `diagnostics/session-control/<path>` |
| `RAW_TRANSFORMER_OUTPUT_LIVE_006/source` or final `worktree` | `after/` for all tracked source files |
| `baseline` at `7a17...` | `before/` |
| Official tools/framework paths | Noether repository at the pinned framework commit; not vendored |
| Hidden reference, fixture payload, full trace, ELF, large archive | Local-only original; not uploaded |

This is a diagnostic selection, not a mirror. Original `SHA256.json`, `FILES_SHA256`, READBACK records and commands may refer to omitted files. Their historical checks remain intact; they do not assert this selected subtree is complete. For the actual publication inventory use `FILE_HASHES.json`, and for copied evidence use `ORIGIN_INDEX.json`.

In particular, `evaluation/static-coverage-diagnosis-001/inputs/official/` is omitted; its original checksum references are retained as provenance. Maintainers should inspect the pinned official source for those code files. Compiled ELFs, complete numerical buffers and hidden fixture payloads are likewise local-only. A pathname is a provenance reference, not a download URL or a claim of included evidence.

The full original archive remains on the desktop and in a Windows backup. Historical commands in copied reports may require that original sandbox or private assets and are not authorization for further paid model execution. `diagnostics/INDEX.md` lists limits of the selected evidence.
