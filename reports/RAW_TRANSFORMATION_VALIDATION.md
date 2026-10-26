# Raw Transformer output: independent validation

Raw final snapshot:`/home/junkun/genesis-transform-experiment/RAW_TRANSFORMER_OUTPUT_LIVE_006/`.

Final source HEAD:`fc82bc131faa1922769fd501cf3e83779af1dc1c`; source clean at capture;171 source-export files. The snapshot contains3,446 checksum-verified files, source, copied Git metadata, original maps, plans, model outputs, logs, failed checks and indexed build artifacts. Manifest SHA256:`1512172728ad50d499f2e904590c8efbef9b46d8da3d319d5c1d42b01bd40b6a`.

The raw source is the output after the official system's configured repairs. It has not received a supervising human numerical-source correction. Earlier failing commits and diagnostics are separately preserved; they are not replaced by the repaired official output.

| Check | Actual status / evidence |
|---|---|
| Official all workflow | Executed through final; exit1; original layout error retained. |
| Three rewrite batches | DONE after configured repairs;19 group sessions executed. |
| Official final checker | FAIL:history.shape,history.tells,code.names,layout.symbols; warning-level plan.log-covers-diff also failed. |
| Group static compiler coverage | EMPTY COVERAGE despite command PASS; missing CMake depfile mapping plus framework fail-open behaviour. |
| Fresh final CUDA full builds |2/2 PASS,29-TU/link admission. |
| All eight required phase oracles |960/960 PASS on RTX5080/SM120. |
| Pristine complete-buffer comparison |960/960 byte-identical. |
| Copied RS no-op |Compiled/ran;0/120 numerical passes, correctly rejected. |
| Paired sanitizer matrix |80/80 PASS; zero skipped/blocked cases. |
| Fresh public full application |3 builds;6/6 CPU/CUDA cases PASS. |
| Frozen raw/source integrity |3,446 snapshot files and171 source files unchanged. |
| CPU-only reference/binding tests |45/45 executed PASS; three pytest modules BLOCKED. |
| Fixture identities |96/96 unchanged;50 input/reference assets unchanged. |
| LLVM/Clang stage |NOT RUN; unavailable and not installed. |
| Human scientific source repairs |None. |

Primary scientific evidence:`evaluation/full-final-001/FULL_FINAL_RESULTS.json`; `SCIENTIFIC_EQUIVALENCE_REPORT.md` gives per-phase errors, precision, preservation and test limits. Application evidence:`evaluation/full-final-app-001/APPLICATION_RESULTS.json`. Original research preservation:`evaluation/protected-final-001/RESULT.json`, `PROTECTED_WINDOWS_FINAL.json`.

## Official failure is not overwritten

The final checker reads cached layout-symbol results explicitly attributed to the earlier665762f layout HEAD. It must not be cited as a fresh final-binary comparison. Its27 differences were independently explained (CUDA embedded-name mapping and an outlined destructor), but strict FAIL remains. The final source has real generated-looking MPI identifiers and history/provenance findings; those were not silently scrubbed or rebased to force PASS.

The scientifically tested result is **raw numerical PASS with mechanical workflow FAIL**. This combination is useful evidence about the collaborator's transformation system. It is not an untouched automatic full-pipeline success or a proof of whole-repository equivalence.
