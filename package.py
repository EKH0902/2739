#!/usr/bin/env python3
"""통합 단일 실행 파일 패키징 스크립트.

모든 프로그램(main / effect / effect2 / start / control / reboot)의 동작을
하나로 통합한 `unify.c` 를 컴파일하여 단일 exe `exe_pack/2739.exe` 를 생성한다.
UAC 승격이나 경고 없이 즉시 실행된다.

Windows exe 는 mingw-w64(x86_64-w64-mingw32-gcc)로 크로스컴파일한다.
"""

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
OUT = ROOT / "exe_pack"
SOURCE = ROOT / "unify.c"
EXE_NAME = "2739.exe"


def find_compiler() -> str:
    for candidate in ("x86_64-w64-mingw32-gcc", "gcc", "cc", "clang"):
        path = shutil.which(candidate)
        if path:
            return path
    sys.exit("error: C 컴파일러를 찾을 수 없습니다 "
             "(x86_64-w64-mingw32-gcc, gcc, cc, clang 확인).")


def main() -> int:
    if not SOURCE.exists():
        sys.exit(f"error: 소스 파일이 없습니다: {SOURCE}")

    compiler = find_compiler()
    print(f"[compiler] {compiler}")

    if OUT.exists():
        shutil.rmtree(OUT)
    OUT.mkdir()

    exe = OUT / EXE_NAME
    print(f"[build] {EXE_NAME}")
    subprocess.run([compiler, str(SOURCE), "-o", str(exe)], check=True)

    print(f"[done] 통합 패키징 완료 -> {exe}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
