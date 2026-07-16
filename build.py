"""Build every C program in this repo into a Windows .exe.

On Windows this uses the native ``gcc``/``cc``. On other platforms it falls
back to the mingw-w64 cross compiler so the produced binaries are still
Windows executables. Output goes to the ``dist/`` directory.
"""

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DIST = ROOT / "dist"

SOURCES = ["main.c", "effect.c", "effect2.c", "reboot.c", "start.c"]


def find_compiler() -> str:
    if sys.platform == "win32":
        candidates = ("gcc", "cc", "clang")
    else:
        candidates = ("x86_64-w64-mingw32-gcc", "i686-w64-mingw32-gcc")
    for candidate in candidates:
        path = shutil.which(candidate)
        if path:
            return path
    sys.exit(f"error: no suitable C compiler found (looked for {candidates})")


def build() -> None:
    compiler = find_compiler()
    DIST.mkdir(exist_ok=True)
    for source in SOURCES:
        src = ROOT / source
        out = DIST / (src.stem + ".exe")
        print(f"[build] {compiler} {source} -> dist/{out.name}")
        subprocess.run([compiler, str(src), "-o", str(out), "-mwindows"]
                       if source == "reboot.c"
                       else [compiler, str(src), "-o", str(out)],
                       check=True)
    print(f"[build] done -> {DIST}")


if __name__ == "__main__":
    build()
