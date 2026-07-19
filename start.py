#!/usr/bin/env python3
"""빌드 래퍼.

모든 C 소스를 개별 Windows exe 로 크로스컴파일하여 exe_pack/ 에 배치한다.
control.ps1 도 함께 복사한다.

실행 흐름:
  start.py (빌드) -> exe_pack/ 생성
  Windows 에서 start.exe 실행 ->
    UAC 승격 + 경고 -> 시작프로그램 등록(reboot 제외) -> main.exe 실행
  main.exe -> reboot.exe (즉시 리부팅)
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


RC_FILE = "start.rc"


def find_compiler():
    for c in ("x86_64-w64-mingw32-gcc", "gcc", "cc", "clang"):
        p = shutil.which(c)
        if p:
            return p
    sys.exit("error: C 컴파일러를 찾을 수 없습니다.")


def find_windres():
    for w in ("x86_64-w64-mingw32-windres", "windres"):
        p = shutil.which(w)
        if p:
            return p
    sys.exit("error: windres를 찾을 수 없습니다 (start.exe 매니페스트 필수).")


def main():
    compiler = find_compiler()
    windres = find_windres()
    print(f"[compiler] {compiler}")

    rc_path = ROOT / RC_FILE
    if not rc_path.exists():
        sys.exit(f"error: {RC_FILE} not found (start.exe 매니페스트 필수).")
    manifest_path = ROOT / "start.exe.manifest"
    if not manifest_path.exists():
        sys.exit("error: start.exe.manifest not found (start.exe 매니페스트 필수).")
    for src, _exe, _libs in TARGETS:
        if not (ROOT / src).exists():
            sys.exit(f"error: {src} not found")

    if OUT.exists():
        shutil.rmtree(OUT)
    OUT.mkdir()
    res_obj = OUT / "start.res.o"
    print(f"[windres] {RC_FILE} -> start.res.o")
    subprocess.run(
        [windres, str(rc_path), "-o", str(res_obj)],
        check=True, cwd=str(ROOT),
    )

    for src, exe, libs in TARGETS:
        src_path = ROOT / src
        exe_path = OUT / exe
        extra = [str(res_obj)] if src == "start.c" else []
        print(f"[build] {src} -> {exe}")
        subprocess.run(
            [compiler, str(src_path)] + extra + ["-o", str(exe_path)] + libs,
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
