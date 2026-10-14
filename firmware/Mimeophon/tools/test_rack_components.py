"""Run the continuation audit and focused native C++ tests; save actual results.

Usage: python tools/test_rack_components.py --compiler /path/to/g++
On Windows put the selected compiler's bin directory first on PATH for its DLLs.
Executables live only in a temporary directory. No Rack SDK is required.
"""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess
import sys
import uuid

ROOT = Path(__file__).resolve().parents[1]

def run(command):
    result = subprocess.run([str(x) for x in command],
                            capture_output=True, text=True, cwd=ROOT)
    if result.returncode:
        raise RuntimeError(f'Command failed: {command}\n{result.stdout}\n{result.stderr}')
    return result.stdout.strip()

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', default='g++')
    args = parser.parse_args()
    destination = ROOT/'analysis/continuation_validation.json'
    result = {'scope': 'Selected component checks, not full firmware or Rack equivalence',
              'passed': False}
    # Invalidate an earlier success even when this run fails midway.
    destination.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    result['compiler_target'] = run([args.compiler, '-dumpmachine'])
    result['compiler_version'] = run([args.compiler, '--version']).splitlines()[0]
    result['symbolic_audit'] = run([sys.executable, ROOT/'tools/audit_delay_reads.py'])
    flags = ['-std=c++17', '-O2', '-Wall', '-Wextra', '-pedantic', '-ffp-contract=off']
    result['compiler_flags'] = flags
    result['tests'] = {}
    # Use a workspace directory: managed Windows sessions may prohibit the
    # user's system temp directory. Cleanup is non-recursive and path checked.
    temporary = (ROOT/'tests'/('.native-'+uuid.uuid4().hex)).resolve()
    if temporary.parent != (ROOT/'tests').resolve():
        raise RuntimeError('Build directory escaped tests directory')
    temporary.mkdir(mode=0o777)
    binaries = []
    generated = temporary/'color_trace.generated.hpp'
    try:
        result['color_trace_generation'] = run([sys.executable, ROOT/'tools/generate_color_trace.py',
                                                 '--output', generated])
        for name in ('test_components', 'test_delay_read', 'test_color_feedback', 'test_modulation', 'test_events', 'test_hold_control'):
            binary = temporary/(name+'.exe' if sys.platform=='win32' else name)
            binaries.append(binary)
            run([args.compiler, *flags, '-I', temporary, ROOT/f'tests/{name}.cpp', '-o', binary])
            result['tests'][name] = run([binary])
    finally:
        for binary in binaries:
            binary.unlink(missing_ok=True)
        generated.unlink(missing_ok=True)
        temporary.rmdir()
    result['source_sha256'] = {
        path: hashlib.sha256((ROOT/path).read_bytes()).hexdigest()
        for path in ('reconstruction/delay_read_components.hpp',
                     'tests/test_delay_read.cpp', 'tools/audit_delay_reads.py',
                     'tools/test_rack_components.py', 'tools/generate_color_trace.py',
                     'reconstruction/color_feedback_components.hpp', 'tests/test_color_feedback.cpp',
                     'reconstruction/modulation_components.hpp', 'tests/test_modulation.cpp',
                     'tests/instruction_machine.hpp', 'reconstruction/event_components.hpp',
                     'tests/test_events.cpp', 'reconstruction/hold_control_components.hpp',
                     'tests/test_hold_control.cpp')}
    result['passed'] = True
    destination.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    continuation_files = [
        'README.md', 'CODEX_HANDOFF.md', 'analysis/RACK_RECONSTRUCTION.md',
        'analysis/delay_read_audit.json', 'analysis/continuation_validation.json',
        'analysis/color_trace_audit.json',
        'analysis/COLOR_HALO_ROUTING.md',
        'analysis/MODULATION_SCHEDULER.md',
        'analysis/EVENT_TRANSITIONS.md',
        'analysis/HOLD_AND_INPUTS.md',
        *result['source_sha256'],
    ]
    manifest = ''.join(f'{hashlib.sha256((ROOT/path).read_bytes()).hexdigest()}  {path}\n'
                       for path in sorted(continuation_files))
    (ROOT/'MANIFEST.continuation.sha256').write_text(manifest, encoding='utf-8')
    print(json.dumps(result, indent=2))

if __name__ == '__main__':
    main()
