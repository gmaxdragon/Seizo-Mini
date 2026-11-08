"""Build a native, hardware-free core simulator and run every host test.

No compiler or dependency downloads. No serial import, port discovery or device
access. Supply the unmodified reviewed firmware core.h directory explicitly.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest


def main(argv=None):
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument("--compiler", required=True, help="Existing trusted clang/gcc/zig executable")
    p.add_argument("--zig", action="store_true", help="Add the cc subcommand for Zig")
    p.add_argument("--core-dir", type=Path, required=True, help="Directory containing reviewed firmware core.h")
    p.add_argument("--report", type=Path, help="Optional JSON result file")
    a=p.parse_args(argv)
    root=Path(__file__).resolve().parent
    core=(a.core_dir/"core.h").resolve()
    expected="d806645811b5cb8795e9d65ede17dc5cbad2d792851868cc3601e6c6b698447e"
    if not core.is_file() or hashlib.sha256(core.read_bytes()).hexdigest()!=expected:
        p.error("core.h is not the unchanged reviewed three-driver core")
    compiler=shutil.which(a.compiler) or (str(Path(a.compiler).resolve()) if Path(a.compiler).is_file() else None)
    if not compiler:p.error("Compiler not found; this tool does not install software")
    with tempfile.TemporaryDirectory(prefix="r41-host-test-") as tmp:
        executable=Path(tmp)/("firmware_core_sim.exe" if os.name=="nt" else "firmware_core_sim")
        command=[compiler]+(["cc"] if a.zig else [])+[
            "-std=c11","-O2","-Wall","-Wextra","-Werror","-pedantic",
            "-I",str(core.parent),str(root/"tests/firmware_protocol_sim.c"),"-o",str(executable)]
        build=subprocess.run(command,check=False)
        if build.returncode:return build.returncode
        os.environ["R41_CORE_SIM"]=str(executable)
        suite=unittest.defaultTestLoader.discover(str(root/"tests"))
        result=unittest.TextTestRunner(verbosity=2).run(suite)
        report={"tests_run":result.testsRun,"failures":len(result.failures),"errors":len(result.errors),
                "skipped":len(result.skipped),"all_passed_without_skips":result.wasSuccessful() and not result.skipped,
                "firmware_core_sha256":expected,"physical_testing":False,"hardware_serial_ports_opened":0,
                "firmware_flashed":False,"live_application_changed":False}
        if a.report:a.report.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
        return 0 if report["all_passed_without_skips"] else 1


if __name__=="__main__":raise SystemExit(main())
