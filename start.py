"""Build and run main.c.

Running this script compiles main.c with a C compiler and then executes
the resulting binary.
"""

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SOURCE = ROOT / "main.c"
BINARY = ROOT / "main"


def find_compiler() -> str:
    for candidate in ("cc", "gcc", "clang"):
        path = shutil.which(candidate)
        if path:
            return path
    sys.exit("error: no C compiler found (looked for cc, gcc, clang)")


def build() -> None:
    compiler = find_compiler()
    print(f"[build] {compiler} {SOURCE.name} -> {BINARY.name}")
    subprocess.run([compiler, str(SOURCE), "-o", str(BINARY)], check=True)


def run() -> int:
    print(f"[run] ./{BINARY.name}")
    return subprocess.run([str(BINARY)]).returncode


def start() -> int:
    build()
    return run()


if __name__ == "__main__":
    sys.exit(start())
