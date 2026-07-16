"""Register the built programs to run at Windows startup.

Every program is registered under the current user's ``Run`` key EXCEPT
``reboot``, which is intentionally left out so it only runs on demand.

Two modes:
* Run directly (``python install.py``) after ``python build.py`` — registers
  the executables straight from ``dist/``.
* Run as the packaged single exe (see package.py) — the bundled executables
  are first copied to a persistent per-user install directory, then that
  persistent copy is registered for startup.

Use ``--uninstall`` to remove the entries again.
"""

import argparse
import os
import shutil
import sys
from pathlib import Path

RUN_KEY = r"Software\Microsoft\Windows\CurrentVersion\Run"

# Programs to auto-start. "reboot" is deliberately excluded.
STARTUP_PROGRAMS = ["main", "effect", "effect2", "start"]
ALL_PROGRAMS = STARTUP_PROGRAMS + ["reboot"]
PREFIX = "app2739_"


def _bundle_dir() -> Path:
    """Where the .exe files live in the current run mode."""
    if getattr(sys, "frozen", False):
        # Packaged by PyInstaller: bundled data lives under _MEIPASS/dist.
        return Path(sys._MEIPASS) / "dist"  # type: ignore[attr-defined]
    return Path(__file__).resolve().parent / "dist"


def _install_dir() -> Path:
    base = os.environ.get("LOCALAPPDATA") or str(Path.home())
    return Path(base) / "app2739"


def _open_run_key():
    import winreg

    return winreg.OpenKey(
        winreg.HKEY_CURRENT_USER, RUN_KEY, 0,
        winreg.KEY_SET_VALUE | winreg.KEY_QUERY_VALUE
    )


def _stage_executables() -> Path:
    """Copy bundled exes to a persistent dir when frozen; return their dir."""
    src = _bundle_dir()
    if not getattr(sys, "frozen", False):
        return src

    dest = _install_dir()
    dest.mkdir(parents=True, exist_ok=True)
    for name in ALL_PROGRAMS:
        exe = src / f"{name}.exe"
        if exe.exists():
            shutil.copy2(exe, dest / exe.name)
    return dest


def install() -> None:
    import winreg

    exe_dir = _stage_executables()
    with _open_run_key() as key:
        for name in STARTUP_PROGRAMS:
            exe = exe_dir / f"{name}.exe"
            if not exe.exists():
                sys.exit(f"error: {exe} not found; run build.py first")
            value = f'"{exe}"'
            winreg.SetValueEx(key, PREFIX + name, 0, winreg.REG_SZ, value)
            print(f"[install] startup: {PREFIX + name} -> {value}")
    print("[install] done (reboot intentionally not registered)")


def uninstall() -> None:
    import winreg

    with _open_run_key() as key:
        for name in STARTUP_PROGRAMS:
            try:
                winreg.DeleteValue(key, PREFIX + name)
                print(f"[uninstall] removed {PREFIX + name}")
            except FileNotFoundError:
                pass
    print("[uninstall] done")


def main() -> None:
    if sys.platform != "win32":
        sys.exit("error: startup registration is only supported on Windows")

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--uninstall", action="store_true",
                        help="remove the startup entries")
    args = parser.parse_args()

    if args.uninstall:
        uninstall()
    else:
        install()


if __name__ == "__main__":
    main()
