#!/usr/bin/python3
"""Public CFD3D application BUILD_SMOKE/regression hooks; not the hidden scientific oracle.

Every CMake, compiler, linker and application invocation is inside a whitelist bwrap
sandbox. Only the requested source tree, system toolchain and one work directory
are exposed. Inputs and VTK regression outputs are public, deliberately small and
lossy (upstream VTK text); independent FP64 eight-phase validation is separate.
"""
from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parent.parent
PUBLIC = Path(__file__).resolve().parent
NAMES = ('cpu-reference', 'cuda-fma', 'cuda-no-fma')
SYSTEM = ('/usr', '/bin', '/lib', '/lib64', '/sbin')
SCENARIO = '''scenario driven_cavity
geometry none
xLength 1.0
yLength 1.25
zLength 1.5
xOrigin 0
yOrigin 0
zOrigin 0
imax 4
jmax 5
kmax 6
dt 0.001
tEnd {end}
tau -1
dtWrite 0.0005
itermax 5000
eps 1e-8
omg 1.5
alpha 0.5
Re 100
GX 0
GY 0
GZ 0
PI 0
UI 0
VI 0
WI 0
'''


def stamp():
    return dt.datetime.now(dt.timezone.utc).strftime('%Y%m%dT%H%M%S.%fZ')


def digest(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def save(p, data):
    p = Path(p)
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps(data, indent=2, sort_keys=True) + '\n')


def inside(p, root):
    return p == root or root in p.parents


def output_path(p):
    p = Path(p).resolve()
    if not inside(p, ROOT / 'record'):
        raise ValueError(f'output must be inside {ROOT / "record"}: {p}')
    p.mkdir(parents=True, exist_ok=True)
    return p


def source_tree(p, build=True):
    p = Path(p).resolve()
    if not inside(p, ROOT) or p == ROOT:
        raise ValueError('source must be an isolated experiment subtree')
    if p.relative_to(ROOT).parts[0] in {'pristine', 'backups', 'private', 'evaluation'}:
        raise ValueError('protected source/archive location is not an execution tree')
    if not (p / 'src').is_dir() or (build and not (p / 'CMakeLists.txt').is_file()):
        raise ValueError('source tree lacks expected project files')
    return p


def sandbox(tree, work, cwd, argv, log):
    """No shell, inherited environment, host home, networking or broad writable bind."""
    tree, work, cwd = Path(tree).resolve(), Path(work).resolve(), Path(cwd).resolve()
    if not inside(cwd, work):
        raise ValueError('command cwd must stay in the exact writable output directory')
    cwd.mkdir(parents=True, exist_ok=True)
    cmd = ['/usr/bin/bwrap', '--die-with-parent', '--new-session', '--unshare-all',
           '--cap-drop', 'ALL', '--clearenv']
    for name in SYSTEM:
        if Path(name).exists():
            cmd += ['--ro-bind', name, name]
    cmd += ['--proc', '/proc', '--dev', '/dev', '--tmpfs', '/tmp', '--dir', '/tmp/home']
    for name in ('/etc/ld.so.cache', '/etc/ld.so.conf', '/etc/ld.so.conf.d',
                 '/etc/alternatives', '/etc/os-release', '/sys'):
        if Path(name).exists():
            cmd += ['--ro-bind', name, name]
    if Path('/dev/dxg').exists():
        cmd += ['--dev-bind', '/dev/dxg', '/dev/dxg']
    cmd += ['--ro-bind', str(tree), str(tree), '--ro-bind', str(PUBLIC), str(PUBLIC),
            '--bind', str(work), str(work), '--chdir', str(cwd)]
    env = {'PATH': '/usr/local/cuda-13.2/bin:/usr/bin:/bin', 'HOME': '/tmp/home',
           'LANG': 'C.UTF-8', 'LC_ALL': 'C.UTF-8', 'OMP_NUM_THREADS': '1',
           'OPENBLAS_NUM_THREADS': '1', 'CUDA_VISIBLE_DEVICES': '0',
           'CUDA_CACHE_PATH': str(work / '.cuda-cache'),
           'LD_LIBRARY_PATH': '/usr/lib/wsl/lib:/usr/local/cuda-13.2/lib64',
           'TMPDIR': '/tmp'}
    for k, v in env.items():
        cmd += ['--setenv', k, v]
    cmd += ['--', *map(str, argv)]
    log = Path(log)
    log.parent.mkdir(parents=True, exist_ok=True)
    start = time.monotonic()
    with log.open('x') as stream:
        stream.write('COMMAND ' + shlex.join(map(str, argv)) + '\n')
        stream.flush()
        result = subprocess.run(cmd, stdout=stream, stderr=subprocess.STDOUT,
                                env={'PATH': '/usr/bin:/bin'}, timeout=1800)
    receipt = {'command': list(map(str, argv)), 'sandbox_command': cmd,
               'returncode': result.returncode, 'seconds': time.monotonic() - start,
               'log': str(log), 'log_sha256': digest(log),
               'isolation': {'network': 'unshared', 'environment': 'clearenv whitelist',
                             'source': 'read-only', 'writable_bind': str(work)}}
    save(log.with_suffix(log.suffix + '.json'), receipt)
    if result.returncode:
        raise RuntimeError(f'command exited {result.returncode}; preserved log: {log}')
    return receipt


def configure(tree, work, name, logs):
    out = work / name
    out.mkdir(parents=True, exist_ok=True)
    cuda = name != 'cpu-reference'
    args = ['/usr/bin/cmake', '-S', str(tree), '-B', str(out), '-G', 'Unix Makefiles',
            '-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_EXPORT_COMPILE_COMMANDS=ON',
            '-DUSE_CUDA=' + ('ON' if cuda else 'OFF'), '-DUSE_MPI=OFF', '-DUSE_OPENCL=OFF',
            '-DCMAKE_C_COMPILER=/usr/bin/gcc', '-DCMAKE_CXX_COMPILER=/usr/bin/g++',
            '-DCMAKE_CXX_FLAGS_RELEASE=-O2 -DNDEBUG -ffp-contract=off']
    if cuda:
        args += ['-DCMAKE_CUDA_COMPILER=/usr/local/cuda-13.2/bin/nvcc',
                 '-DCMAKE_CUDA_ARCHITECTURES=120', '-DCMAKE_CUDA_STANDARD=17',
                 '-DCMAKE_CUDA_FLAGS_RELEASE=-O2 -DNDEBUG --fmad=' +
                 ('true' if name == 'cuda-fma' else 'false') +
                 ' --ftz=false --prec-div=true --prec-sqrt=true']
    sandbox(tree, work, out, args, logs / (name + '-configure.log'))
    database = out / 'compile_commands.json'
    raw_database = database.read_bytes()
    entries = json.loads(raw_database)
    # NVCC response files otherwise hide include paths from impact's tree mapper.
    # Preserve the exact original DB; expansion changes representation, not flags.
    raw_path = logs / (name + '-compile-commands.raw.json')
    raw_path.write_bytes(raw_database)
    for entry in entries:
        argv = entry.get('arguments') or shlex.split(entry['command'])
        expanded, i = [], 0
        while i < len(argv):
            arg = argv[i]
            if arg == '--options-file':
                i += 1
                rp = (out / argv[i]).resolve()
                if not inside(rp, out):
                    raise ValueError('compiler response file escaped selected build directory')
                expanded.extend(shlex.split(rp.read_text()))
            elif arg.startswith('@'):
                rp = (out / arg[1:]).resolve()
                if not inside(rp, out):
                    raise ValueError('compiler response file escaped selected build directory')
                expanded.extend(shlex.split(rp.read_text()))
            else:
                expanded.append(arg)
            i += 1
        entry['arguments'] = expanded
        entry.pop('command', None)
    save(database, entries)
    if not entries or any(not e.get('file') for e in entries):
        raise RuntimeError('empty or invalid compile_commands database')
    tus = sorted({str(Path(e['file']).resolve().relative_to(tree)) for e in entries})
    if cuda and not any(p.endswith('.cu') for p in tus):
        raise RuntimeError('required CUDA build configured without CUDA translation units')
    save(logs / (name + '-compile-database.json'),
         {'entries': len(entries), 'translation_units': tus, 'sha256': digest(database)})
    return out


def build(tree, work, name, logs, clean=False):
    out = configure(tree, work, name, logs)
    argv = ['/usr/bin/cmake', '--build', str(out), '--parallel', '2', '--verbose']
    if clean:
        argv += ['--clean-first']
    sandbox(tree, work, out, argv, logs / (name + '-build.log'))
    exe = out / 'cfd3d'
    if not exe.is_file() or not os.access(exe, os.X_OK):
        raise RuntimeError('required application executable absent/nonexecutable')
    return exe


def vtk_fields(p):
    """Parse complete ASCII VTK arrays; ignore human title and field label spelling.

    Exact comparison is of encoded finite float64 values parsed from upstream's
    ASCII output, not of the simulator's internal FP64 buffers or CPU versus CUDA.
    """
    lines = p.read_text().splitlines()
    if len(lines) < 4 or not lines[0].startswith('# vtk DataFile Version') or lines[2] != 'ASCII':
        raise ValueError('expected nonempty ASCII legacy VTK')
    t = ' '.join(lines[3:]).split()
    i, current, fields, dims = 0, None, [], None
    while i < len(t):
        k = t[i]; i += 1
        if k == 'DATASET':
            if t[i] != 'STRUCTURED_GRID':
                raise ValueError('unexpected VTK dataset')
            i += 1
        elif k == 'DIMENSIONS':
            dims = list(map(int, t[i:i+3])); i += 3
        elif k in {'POINT_DATA', 'CELL_DATA'}:
            current = [k, int(t[i])]; i += 1
        elif k in {'POINTS', 'VECTORS', 'SCALARS'}:
            if k == 'POINTS':
                n = int(t[i]) * 3; i += 2
            elif k == 'VECTORS':
                if current is None:
                    raise ValueError('vector has no association')
                n = current[1] * 3; i += 2
            else:
                if current is None:
                    raise ValueError('scalar has no association')
                n = current[1] * int(t[i+2]); i += 3
                if t[i] != 'LOOKUP_TABLE':
                    raise ValueError('missing scalar lookup table')
                i += 2
            values = tuple(float(v) for v in t[i:i+n]); i += n
            if len(values) != n or not all(math.isfinite(v) for v in values):
                raise ValueError('truncated or nonfinite VTK numeric array')
            fields.append({'kind': k, 'association': current, 'count': n,
                           'values': [v.hex() for v in values]})
        else:
            raise ValueError(f'unexpected VTK token {k!r}')
    if dims != [5, 6, 7] or len(fields) < 4:
        raise ValueError('incorrect smoke grid or incomplete field arrays')
    return {'dimensions': dims, 'fields': fields}


def smoke(tree, work, name, exe, logs, repeat):
    cases = {}
    for steps in (1, 2):
        base = work / 'smoke' / name / f'{stamp()}-steps{steps}-repeat{repeat}'
        run = base / 'run'
        (base / 'scenarios').mkdir(parents=True)
        run.mkdir()
        scenario = base / 'scenarios' / 'driven_cavity.dat'
        scenario.write_text(SCENARIO.format(end=f'{steps * 0.001:.3f}'))
        args = [str(exe), '--scenario', 'driven_cavity', '--solver',
                'cpp' if name == 'cpu-reference' else 'cuda',
                '--outputformat', 'vtk-ascii', '--output', 'true',
                '--linsolver', 'jacobi', '--numparticles', '0', '--tracestreamlines', 'false']
        if name != 'cpu-reference':
            args += ['--gpu', '0', '--blocksize', '4', '4', '4']
        log = logs / f'{name}-steps{steps}-repeat{repeat}.log'
        sandbox(tree, work, run, args, log)
        text = log.read_text(errors='replace')
        if re.search(r'(?i)(fatal error|CUDA error|residual.*not a number|without converging)', text):
            raise RuntimeError(f'application numerical/CUDA warning; preserved {log}')
        vtks = sorted((run / 'output').glob('*.vtk'))
        if len(vtks) != steps + 1:
            raise RuntimeError(f'expected {steps + 1} VTK states, found {len(vtks)}')
        states = [vtk_fields(p) for p in vtks]
        if states[0] == states[-1]:
            raise RuntimeError('public cavity smoke produced no numerical state change')
        case = {'steps': steps, 'scenario_sha256': digest(scenario),
                'states': states, 'vtk_files': [{'path': str(p), 'sha256': digest(p)} for p in vtks],
                'log': str(log), 'warnings': [s for s in text.splitlines() if 'warn' in s.lower()]}
        cases[str(steps)] = case
    return cases


def scientific_projection(cases):
    return {n: {k: c[k] for k in ('steps', 'scenario_sha256', 'states')} for n, c in cases.items()}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('action', choices=('baseline', 'generate', 'commands', 'rebuild', 'quick',
                                       'check', 'validate', 'gpu-build'))
    ap.add_argument('--run-contract', type=Path)
    ap.add_argument('--tree', type=Path)
    ap.add_argument('--work', type=Path)
    ap.add_argument('--reference', type=Path)
    ap.add_argument('--out', type=Path)
    ap.add_argument('--files', nargs='*', default=[])
    ap.add_argument('--builds', nargs='*', choices=NAMES)
    args = ap.parse_args()
    if args.action == 'generate':
        return 0
    contract = json.loads(args.run_contract.read_text()) if args.run_contract else {}
    record = Path(contract.get('record', ROOT / 'record')).resolve()
    tree = source_tree(args.tree or os.environ.get('TREE') or contract.get('tree', ROOT / 'tree'),
                       build=args.action != 'generate')
    default_work = os.environ.get('BASE_WORK') if args.action == 'baseline' else None
    work = output_path(args.work or default_work or record / 'work')
    logs = work / 'logs' / (stamp() + '-' + args.action)
    logs.mkdir(parents=True)
    result = {'action': args.action, 'classification': 'PUBLIC_APPLICATION_BUILD_SMOKE',
              'tree': str(tree), 'work': str(work), 'logs': str(logs),
              'independent_eight_phase_oracle': 'not invoked by this public hook',
              'mpi_opencl_matrix': 'NOT RUN; disabled in this public build matrix',
              'output_precision_limitation': 'ASCII VTK is lossy upstream output; not full FP64 buffers',
              'requested_files': args.files, 'status': 'error'}
    try:
        if args.action == 'generate':
            out = output_path(args.out or logs / 'generated')
            save(out / 'NO_GENERATORS.json', {'status': 'not-applicable',
                 'reason': 'Pinned CFD3D CMake has no source code generator', 'tree': str(tree)})
            result['status'] = 'pass'
        else:
            names = args.builds or (list(NAMES[1:]) if args.action == 'gpu-build' else list(NAMES))
            if args.action == 'commands':
                for name in names:
                    configure(tree, work, name, logs)
                result['status'] = 'pass'
            else:
                exes = {n: build(tree, work, n, logs, clean=args.action in {'baseline', 'validate'})
                        for n in names}
                result['executables'] = {n: {'path': str(p), 'sha256': digest(p)} for n, p in exes.items()}
                if args.action in {'rebuild', 'quick', 'gpu-build'}:
                    result['status'] = 'pass'
                else:
                    first = {n: smoke(tree, work, n, p, logs, 1) for n, p in exes.items()}
                    result['cases'] = first
                    if args.action == 'baseline':
                        reference = work / 'public-baseline.json'
                        if reference.exists():
                            raise ValueError('baseline reference already exists; refusing replacement')
                        second = {n: smoke(tree, work, n, p, logs, 2) for n, p in exes.items()}
                        for n in names:
                            if scientific_projection(first[n]) != scientific_projection(second[n]):
                                raise RuntimeError(f'nondeterministic public baseline for {n}')
                        save(reference, {'schema': 1, 'classification': 'PUBLIC_APPLICATION_BASELINE',
                             'tree': str(tree), 'cases': {n: scientific_projection(first[n]) for n in names},
                             'repeated_determinism': True,
                             'precision': result['output_precision_limitation']})
                        result['reference'] = {'path': str(reference), 'sha256': digest(reference)}
                        result['deterministic_repeats'] = 2
                    else:
                        reference = args.reference or record / 'work-base' / 'public-baseline.json'
                        baseline = json.loads(reference.read_text())
                        if not baseline.get('repeated_determinism'):
                            raise RuntimeError('reference lacks repeated-run determinism evidence')
                        for n in names:
                            if n not in baseline['cases']:
                                raise RuntimeError('required build missing from baseline')
                            if scientific_projection(first[n]) != baseline['cases'][n]:
                                raise RuntimeError(f'public application numeric regression for {n}')
                        result['reference'] = {'path': str(reference), 'sha256': digest(reference)}
                    result['status'] = 'pass'
        save(logs / 'RESULT.json', result)
        print(json.dumps({k: result[k] for k in ('action', 'classification', 'status', 'logs')}))
        return 0
    except Exception as exc:
        result['error'] = str(exc)
        save(logs / 'RESULT.json', result)
        print(json.dumps({'status': 'error', 'error': str(exc), 'logs': str(logs)}), file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
