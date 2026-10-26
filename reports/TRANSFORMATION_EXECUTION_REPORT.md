# Official full CFD3D Transformer: execution report

The official `tools/anonymous/runner.py all` workflow was executed against a full, isolated CFD3D checkout, including model-assisted naming, layout and all19 configured structural rewrite groups. It did **not** achieve an untouched automatic PASS. All three rewrite batches eventually completed after three configured agent repairs, but the official final checker returned **FAIL**. Original layout resource/symbol failures and all intermediate compilation failures remain preserved. Independent scientific validation is reported separately.

## Pins, scope and preservation

| Item | Recorded identity |
|---|---|
| Official framework | `f772f81d83f32bf852af335868379d75759681ac` |
| Scientific upstream | BOWANGESM/CFD_3D_Solver_CUDA, `5943ae78be7334888b7f7b1f4622f9cf61626a2c` |
| Build-compatible baseline | `7a17ebe52929ab28978b1c582a1b9b0cf559d439` |
| Final transformed HEAD | `fc82bc131faa1922769fd501cf3e83779af1dc1c` |
| Writable experiment source | `/home/junkun/genesis-transform-experiment/worktree` |
| Sealed raw final output | `RAW_TRANSFORMER_OUTPUT_LIVE_006/` |
| Preserved V1 / V2 | `93a79702e7b8b24778d77b8afe02e99df1948b9b` / `6e722f4b763528b75a638d5382bf755cc264d232` |

All paths below are relative to `/home/junkun/genesis-transform-experiment/` unless stated otherwise. The full scientific repository was transformed, not an implementation-hole RS workspace. The pre-transform research backup passed5,022-file readback on Linux and Windows before execution. It includes the successful original repository-grounded RS experiment: normal completion,120/120 numerical PASS, RUN/EVAL pass/pass and estimated cost$0.4169775. Those historical costs are separate from this transformation experiment. See `backups/PRE_TRANSFORM_MANIFEST.json`, `PRE_TRANSFORM_SHA256.json` and `BACKUP_READBACK.json`.

The final preservation audit verifies the same5,022 original files,616 original RS evidence files,164 frozen inputs and39 official Transformer files in both original and runtime locations. Windows V1/V2 HEADs, branches, working trees and all refs are unchanged. See `evaluation/protected-final-001/RESULT.json` and `PROTECTED_WINDOWS_FINAL.json`.

## Actual execution and outcomes

| Stage | Actual result |
|---|---|
| Pristine CPU/CUDA baseline and inventory | Executed; baseline numerical outputs retained. |
| Words, naming cards and naming | Model-driven workflow completed. Six naming chunks were resolved; one failed first-chunk stream was recovered under a separately recorded timeout condition. |
| Merge and lexical/path application |743 rename pairs;82 original paths affected, including49 path changes. Optional empty output-map failure preserved before a configuration workaround. |
| Gates, public build/validation and rename commit | Completed after the project-only no-generator hook was adapted to the official temporary-directory contract. |
| Budgeted structural plan | Generated and validated;109 proposed instance slots across19 groups. This is a plan, not an applied count. |
| Layout | Two model-generated commits exist. The second session ended at the locally injected80-turn cap; the stage remains failed. The strict27-symbol check also remains FAIL. |
| Optional deterministic Clang | **NOT RUN**: LLVM/Clang unavailable; none installed. Tree-sitter parsing is not a Clang transformation. |
| Semantic rewrite batches1–3 | All19 group sessions ran; all three batches subsequently completed after configured repair sessions16,22 and34. |
| Official full-application validation | Executed at final HEAD before the final checker; it returned success. Fresh independent application results are reported in the validation report. |
| Official final checker | **FAIL**; four hard failed checks and one warning-level failed check. |
| Official distance stage | Not reached after final-check failure. The unchanged official distance tool was subsequently run independently, with label `independent-final-001`. |

Evidence: `record/runner/state.json`; sealed copies under `RAW_TRANSFORMER_OUTPUT_LIVE_006/record/`; `record/checks/final_check.json`; `evaluation/independent-distance-001/COMMAND.json` and `RESULT.json`. The original code for sequencing/checkpoint skipping is `tools/anonymous/runner.py:580–655`; group execution and configured repairs are at451–480.

The actual model was `claude-opus-4-8` through the existing JHU Anthropic-compatible endpoint. There were34 native sessions:32 normal completions and two preserved execution errors (2 and12). All were part of this one transformation workflow. No Curator or benchmark-synthesis experiment was launched.

## Isolated execution boundary

The official Transformer invokes Claude directly through `Runner.model`, not through the benchmark `run_worker.sh`. An experiment-local `claude` shim and host-only authentication relay supplied the native Linux Claude Code2.1.292 execution boundary. The official39-file tool snapshot remains byte-identical to the pinned framework.

The outer execution environment mounted the declared full source, transformation record and private host-side bookkeeping as writable. The nested model-tool sandbox did not expose that parent bookkeeping; it exposed only its declared source/record paths and separate session HOME for writes. Tools, public testbed, interpreter/toolchain and native CLI were read-only; each native session had a separate home. Original V1/V2 tasks, hidden Python references, fixtures/goldens, previous candidates, successful RS traces, host private configuration and unrelated workspaces were not mounted. Actual denied-path probes are retained in `session-control/TRANSPORT_READY.json` and the transport preflight evidence. New private evaluation payloads stayed under `evaluation/`; worker-visible record scans found no private fixture payloads or reference modules.

The real API key was injected only by the host authentication relay. Workers received a placeholder key and a loopback endpoint in a network namespace with no external route. The spending update retained that boundary. No global Claude authentication settings were modified.

## Preserved failures and explicit continuations

1. The scientific CMake file hardcoded an unsupported older CUDA target. A three-insertion/one-deletion compatibility patch honors explicit `CMAKE_CUDA_ARCHITECTURES=120`. It changes build selection, not numerical algorithms (`planning/BUILD_COMPATIBILITY.diff`).
2. Native2 failed with a stream-idle timeout. Its source, trace, reported cost and incomplete-usage uncertainty remain. One frozen continuation raised transport/client idle timeouts to900seconds; the failed native record was not relabeled PASS (`RAW_TRANSFORMER_OUTPUT_LIVE_001`, `planning/TIMEOUT_RECOVERY_CONDITION.json`).
3. The official apply loop attempted to load an empty optional `output_map.json` after applying the main map; the loader rejects empty mappings. The failed source was sealed, the empty optional map archived, and only the owned source was restored/reapplied (`RAW_TRANSFORMER_OUTPUT_LIVE_002`; `runner.py:568–577`, `rename_map.py:134–143`). No completed paid naming stage was repeated.
4. The project no-generator action initially rejected the official gate's temporary directories. A separate two-line, generate-only overlay returns no files, consistent with CFD3D having no source-generation rule. All other public actions are unchanged (`RAW_TRANSFORMER_OUTPUT_LIVE_003`; `continuations/nogen-hook-recovery-001/`).
5. Native12 hit an80-turn limit injected by our local shim, not the official layout caller. Its committed layout output passed an independent960-case checkpoint and six application cases, but strict symbols still failed. A reviewed manual suffix used the supported `--from pass-1`, preserving failed layout state and call markers. Future sessions omitted only the unsolicited default turn cap; the explicit planning40-turn limit remained (`RAW_TRANSFORMER_OUTPUT_LIVE_004`; `planning/MANUAL_SUFFIX_CONDITION.json`). This was not an automatic clean layout PASS.
6. The user replaced the old$75 session-boundary soft stop with a$120 cumulative post-request cutoff. Native16 finished normally; the next session paused before any paid request. After offline tests, the same workflow resumed and skipped completed pass-1. Within the historical native ledger, only its policy header and administrative block were changed. A separate tested host-side request meter and isolated launch variant implemented the new cutoff; old policy records remain preserved (`RAW_TRANSFORMER_OUTPUT_LIVE_005`; `spending-policy-120/`).

No manual scientific source repair was made. The official system itself corrected three compilation defects, separately preserved in `evaluation/transform-failures/`: generic integer-divisor deduction in CUDA(S11), a missing OpenMP shared declaration(S3), and a missed caller after a parameter-struct refactor(I1). See `MANUAL_FIXES.md` for the distinction between automatic repairs, execution/configuration workarounds and human source repairs.

## Final mechanical result

The final checker reports hard failures in `history.shape`, `history.tells`, `code.names` and `layout.symbols`. `plan.log-covers-diff` is a failed check with warning severity. The symbol report explicitly refers to the earlier layout HEAD665762f, not newly compared final binaries. Its27 findings were independently explained, but its FAIL was never waived. Generated-looking MPI identifiers and history/provenance defects were not silently rewritten.

Static per-group compile labels were also misleading: the frozen project configuration lacked CMake's `.o`→`.o.d` depfile mapping, and the framework returned PASS for zero compiled translation units. This is recorded as **empty coverage**, not successful static compilation. Full batch application builds did execute and caught all three preserved compile defects. See `evaluation/static-coverage-diagnosis-001/AUDIT.md` and `TRANSFORMER_FEEDBACK.md`.

## Spending result

The canonical post-request estimate is **$91.17033050**, including all reported costs since this experiment began. The$120 cutoff was not reached; unspent amount to that cutoff is$28.82966950. The post-request meter accounted for875 responses after its historical bootstrap, and reconciled with the complete native records without double counting. Cached input buckets are separate, and thinking tokens are already included in output.

This is an API-rate estimate, not verified JHU billing. The original native2 interrupted request may have unreported usage. The account-level Gateway limit was not changed. No further paid work is authorized merely by the unspent amount. See `COST_AUDIT.json` and `spending-policy-120/SPENDING_POLICY_UPDATE.md`.

## Actual CUDA transformation depth

The completed configuration does not imply deep restructuring of every scientific kernel. Between the layout checkpoint and final source, none of the eight held CUDA entry bodies changed; one of five CUDA translation units changed, affecting four internal boundary kernels and an extracted device index helper. Earlier lexical/path/layout transformations remain present. Independent official distance analysis reports token similarity 0.964 and AST similarity 0.951, with 158 of 241 function fingerprints unchanged. These are limits on the observed transformation, not proof of anonymization or repository-memory resistance. See `evaluation/full-final-001/CUDA_SOURCE_COVERAGE.md` and `record/distance/independent-final-001.json`. The official distance stage was not reached after final-check failure; that report was generated separately without model calls.
