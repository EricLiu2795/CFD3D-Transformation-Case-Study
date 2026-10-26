# Rewrite plan (budget form)

One section per `PATTERNS` group in menu order. Each gives a **budget** (how many
instances to make), the **regions** it should cover, and the **constraints** the
executor must honour. The executor runs `candidates.py --run RUN --group G` on the
current tree, reads only the candidates it takes, spreads the budget over files and
regions, and logs every kept instance. No instance list is fixed here: the executors
choose the functions from the pool.

Scope: `src/*` (CfdSolver with Cpp/Cuda/Mpi/Opencl, IO, ParticleTracer, Main.cpp,
Defines.hpp). CAP 7 per function over all passes (the Clang stage's per-function
counts, when present, count too). The layout stage has already moved files, so the
agent passes never add, rename, move or split files, and never move a definition
between files.

Pinned (signatures frozen, bodies free): `calculateRsCuda`, `calculateUvwCuda`,
`calculateTemperatureCuda`, `calculateFghCuda`, `calculateDtCuda`,
`setBoundaryValuesCuda`, `setBoundaryValuesScenarioSpecificCuda`, `sorSolverCuda`.
Interface passes (I1–I7) must not alter their signatures; structure/robustness passes
may refactor their bodies.

The pool is large for every structural and interface group (S2 156, P1 63, I1 92,
I2 150, I4 111, I5 121), so CAP, not pool size, is the binding limit on the recognisable
functions. The budgets below are set to the amount each transformation supports
*naturally* while staying spread across files, so no single function collects more than
a couple of instances before the interface passes add theirs.

| # | group | budget | pool | main regions |
|---|---|---|---|---|
| 1 | S7 | 3 | 7 | IO, CfdSolver, ParticleTracer |
| 2 | S11 | 1 | 1 | Defines.hpp |
| 3 | S2 | 14 | 156 | CfdSolver (Cpp/Mpi/Cuda), IO, ParticleTracer |
| 4 | S3 | 3 | 6 | CfdSolver, Main.cpp |
| 5 | S1 | 4 | 10 | IO, CfdSolver, Main.cpp |
| 6 | S4 | 10 | 43 | CfdSolver (Cpp/Cuda/Mpi), IO |
| 7 | P1 | 8 | 63 | CfdSolver (Cpp/Mpi), IO |
| 8 | S5 | 6 | 20 | IO, CfdSolver, Main.cpp |
| 9 | R4 | 1 | 1 | IO |
| 10 | R1 | 4 | 11 | IO, CfdSolver |
| 11 | R3 | 5 | 12 | IO, CfdSolver/Opencl |
| 12 | I1 | 8 | 92 | CfdSolver (Cpp/Mpi), IO, ParticleTracer |
| 13 | I2 | 8 | 150 | CfdSolver (Cpp/Mpi), IO, ParticleTracer |
| 14 | I3 | 5 | 22 | CfdSolver, IO, ParticleTracer |
| 15 | I4 | 8 | 111 | CfdSolver (Cpp/Mpi/Cuda), ParticleTracer |
| 16 | I5 | 8 | 121 | IO, CfdSolver |
| 17 | I6 | 5 | 18 | CfdSolver, IO |
| 18 | I7 | 3 | 8 | CfdSolver, IO |
| 19 | S8 | 5 | 14 | CfdSolver, IO, ParticleTracer |

## 1. S7 — remove dead code (budget 3)
Regions: IO, CfdSolver, ParticleTracer. Remove only helpers dead in *every* build
(check Opencl/Cuda arms, not just the CPU reference). Candidates with no caller in the
tree (the `isLinked` flags, the `fetch*SettingOrDefault` readers, `tokenizeByChar`,
`advanceParticleEuler`) must be confirmed unreferenced under all preprocessor arms
first. Runs first so later passes never touch about-to-be-deleted code. Remove
declaration, definition and any include that only served it.

## 2. S11 — function to template (budget 1)
Region: Defines.hpp. `divCeil` over (int, int) is the only natural candidate.
Templatise only if every call still instantiates with the original integer types; keep
device/inline qualifiers.

## 3. S2 — extract shared logic (budget 14)
Regions: CfdSolver (Cpp/Mpi/Cuda), IO, ParticleTracer. The largest, most recognisable
pass. Spread over regions, ≤3–4 per file; prefer the repetitive border/boundary
families, the flux/temperature kernels and the VTK emitters. Keep expression order and
operand types exactly (a differently typed parameter inserts a conversion). Device
helpers keep their qualifiers, live in an already-included header, and must be checked
for FMA contraction. Vary the form (file-static function / member / lambda) between
sites. Pinned Cuda bodies may receive extractions.

## 4. S3 — branches to lookup table (budget 3)
Regions: CfdSolver, Main.cpp. Only where the table holds the same constants with the
same literal types and control flow is unchanged. The scenario-specific boundary
ladders are parallel across backends — convert at most one or two so the backends do
not all change the same way. Device tables are constexpr/GpuArray in kernel scope.

## 5. S1 — imperative to functional (budget 4)
Regions: IO, CfdSolver, Main.cpp. Host only (avoid the Cuda kernel candidate).
Order-preserving algorithms only, never `std::reduce`/execution policies. Keep the
accumulator type (`0.0`/`Real(0)`). Prefer simple scalar accumulations over the
relaxation-sweep reductions whose order the output depends on.

## 6. S4 — nested branches to guard clauses (budget 10)
Regions: CfdSolver (Cpp/Cuda/Mpi), IO. Only where control flow is identical on every
path; watch for code after the nested block an early return would skip. The
`assignInternalU/V/W/TBorders*` families across the three backends are the richest
sites — spread across backends rather than all variants of one border. Match each
file's early-return idiom. Pinned Cuda bodies allowed.

## 7. P1 — hoist loop invariant (budget 8)
Regions: CfdSolver (Cpp/Mpi), IO. Hoist the exact same expression and type; never a
reciprocal multiply. Confirm bit-identity (FMA contraction). Prefer the long CPU/MPI
bodies and VTK emitters. Do not hoist in a way that changes the iteration order of
reduction/RNG/I/O loops.

## 8. S5 — raw pointer to RAII (budget 6)
Regions: IO, CfdSolver, Main.cpp. Host only: fopen/fclose → fstream, new[] → vector
(`<fstream>` may be added). Do not change GPU/arena allocators or buffer lifetimes that
async kernels read. The IO readers/writers are the natural sites; vary the replacement.
Keep the exact bytes written/read.

## 9. R4 — fixed buffer to container (budget 1)
Region: IO. `readPgmImage`'s fixed-size local array → std::array/vector of the same
extent, host only. Keep contents and read sizes identical.

## 10. R1 — check before memory ops (budget 4)
Regions: IO, CfdSolver. Guard a copy only where the guard is always true or a provable
no-op when false. Nothing added to the success path. Prefer host buffer copies
(BinaryStream, resizeNearest, PGM scalers, getDataForOutput) over kernel code.

## 11. R3 — check return value (budget 5)
Regions: IO, CfdSolver/Opencl. Add a checked error path to unchecked I/O where the
error path is unreachable on valid runs and the success path is unchanged — no new
output on stdout. Vary between abort/throw/early-return to match each file.

## 12. I1 — introduce parameter object (budget 8)
Regions: CfdSolver (Cpp/Mpi), IO, ParticleTracer. Only where all call sites are in
scope and not frozen; edit every call site in the same instance (call sites cost no
CAP but must be listed). Field types match exactly; pass by const& where the parameters
were. The wide host signatures (initialize family, computeFghCpp, border setters,
flowline integrator, loadCaseConfigFromFile) are strong; group related parameters, do
not blob all into one. Device structs must be trivially copyable in an included header.
**Do not touch the pinned Cuda signatures.** Keep CAP low: do not stack I1/I2/I3/I4 on
the same function.

## 13. I2 — reorder parameters (budget 8)
Regions: CfdSolver (Cpp/Mpi), IO, ParticleTracer. One consistent rule per family
(inputs, options, outputs); update every call site; arguments side-effect free. Spread
onto functions I1 did not restructure. No pinned signatures.

## 14. I3 — output parameter to return value (budget 5)
Regions: CfdSolver, IO, ParticleTracer. Only for always-written outputs; prefer the
multi-output candidates (loadCaseConfigFromFile, relaxSolverOpencl, mpiStartup,
parseOptions, computeDtOpencl) over single-output ones. Keep computation order; small
trivially copyable structs in device code. Edit all call sites; avoid pinned functions;
mind CAP overlap with I1/I2.

## 15. I4 — raw pointer to typed view (budget 8)
Regions: CfdSolver (Cpp/Mpi/Cuda), ParticleTracer. Same element type and extent; never
change which memory a kernel touches or when. Device sites only under device rules
(GpuArray/Array4), same memory space; pinned Cuda signatures off limits. The
flux/velocity/border functions carry the most pointers; spread and vary the view type.
Update every call site.

## 16. I5 — change function home (budget 8)
Regions: IO, CfdSolver. Within the SAME file only (free ↔ static member, namespace ↔
struct, merge/split namespace): no file moves (layout stage owns those). Keep
device/inline qualifiers. The IO recorders and solver classes have many members; spread
across files and vary direction. Update call sites; no pinned signatures.

## 17. I6 — flag parameter to enum or split (budget 5)
Regions: CfdSolver, IO. Only where every call site picks the same branch as before;
update all call sites. The initialize family and compute-dt/relax variants carry bool
flags — enum where the flag names a mode, split where it gates a distinct path. Vary
between the two; avoid pinned signatures.

## 18. I7 — regroup struct fields (budget 3)
Regions: CfdSolver, IO. Only for structs not serialised, written to output, or mirrored
byte-wise to the device (P4 rules; update every aggregate initialiser and member
access). The solver context structs and VtkExporter are candidates — verify none is
device-mirrored or checkpointed first; skip the ByteStream structs if serialisation
relies on their layout. Runs after the interface passes, over the final struct shapes.

## 19. S8 — rename with alias (budget 5)
Regions: CfdSolver, IO, ParticleTracer. Type aliases only (using/typedef), over the
final types including any struct the interface passes introduced. Alias a
frequently-used type at a natural header boundary and use it consistently where it
reads; do not alias everything the same way. Underlying type unchanged.
