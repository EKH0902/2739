"""Package the whole app into a single Windows .exe.

This bundles the installer (install.py) together with the compiled program
executables in dist/ into one standalone .exe using PyInstaller. Running that
single exe on a target machine registers every program (except reboot) to run
at startup.

Prerequisites (run on Windows):
    python build.py            # produce dist/*.exe
    pip install pyinstaller
    python package.py

Note: PyInstaller produces a Windows .exe only when run on Windows.
"""

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DIST = ROOT / "dist"


def package() -> None:
    if sys.platform != "win32":
        sys.exit("error: single-exe packaging must be run on Windows")
    if not DIST.exists():
        sys.exit("error: dist/ not found; run build.py first")

    cmd = [
        sys.executable, "-m", "PyInstaller",
        "--onefile",
        "--name", "app2739",
        "--add-data", f"{DIST};dist",
        str(ROOT / "install.py"),
    ]
    print("[package]", " ".join(cmd))
    subprocess.run(cmd, check=True)
    print("[package] done -> dist/app2739.exe")


if __name__ == "__main__":
    package()
