#!/usr/bin/env python3
"""패키징 스크립트.

start.py 와 동일하게 모든 C 소스를 개별 exe 로 크로스컴파일하여
exe_pack/ 에 배치한다. control.ps1 도 함께 복사한다.

실행 흐름:
  start.exe (UAC + 경고 + 시작프로그램 등록) -> main.exe -> reboot.exe
"""

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
OUT = ROOT / "exe_pack"

TARGETS = [
    ("start.c",   "start.exe",   ["-lshell32", "-ladvapi32", "-lcomctl32"]),
    ("main.c",    "main.exe",    []),
    ("reboot.c",  "reboot.exe",  []),
    ("effect.c",  "effect.exe",  []),
    ("effect2.c", "effect2.exe", []),
]

COPY_FILES = ["control.ps1"]


def find_compiler():
    for c in ("x86_64-w64-mingw32-gcc", "gcc", "cc", "clang"):
        p = shutil.which(c)
        if p:
            return p
    sys.exit("error: C 컴파일러를 찾을 수 없습니다.")


def main():
    compiler = find_compiler()
    print(f"[compiler] {compiler}")

    if OUT.exists():
        shutil.rmtree(OUT)
    OUT.mkdir()

    for src, exe, libs in TARGETS:
        src_path = ROOT / src
        if not src_path.exists():
            sys.exit(f"error: {src} not found")
        exe_path = OUT / exe
        print(f"[build] {src} -> {exe}")
        subprocess.run(
            [compiler, str(src_path), "-o", str(exe_path)] + libs,
            check=True,
        )

    for f in COPY_FILES:
        src = ROOT / f
        if src.exists():
            shutil.copy2(src, OUT / f)
            print(f"[copy] {f}")

    print(f"[done] 패키징 완료 -> {OUT}")


if __name__ == "__main__":
    main()
