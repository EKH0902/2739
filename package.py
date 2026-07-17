#!/usr/bin/env python3
"""테스트 버전 실행 파일 패키징 스크립트.

reboot 를 제외한 모든 프로그램의 "테스트 버전"(권한 요청/UAC 없이 단순히
문자열만 출력)을 exe_pack/ 에 개별 exe 로 빌드하고, 전부를 하나로 묶은
exe_pack/2739(1).exe 도 생성한다.

Windows exe 는 mingw-w64(x86_64-w64-mingw32-gcc)로 크로스컴파일하며,
없으면 시스템 gcc 로 대체 빌드한다.
"""

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
OUT = ROOT / "exe_pack"

# 프로그램 이름 -> 테스트 버전 출력 문자열 (reboot 제외)
PROGRAMS = {
    "main": "Hello, world!",
    "effect": "effect",
    "effect2": "effect2",
    "start": "start",
    "control": "control",
}

COMBINED = "2739(1).exe"


def find_compiler() -> str:
    for candidate in ("x86_64-w64-mingw32-gcc", "gcc", "cc", "clang"):
        path = shutil.which(candidate)
        if path:
            return path
    sys.exit("error: C 컴파일러를 찾을 수 없습니다 "
             "(x86_64-w64-mingw32-gcc, gcc, cc, clang 확인).")


def compile_source(compiler: str, source: Path, exe: Path) -> None:
    subprocess.run([compiler, str(source), "-o", str(exe)], check=True)


def build_individual(compiler: str, name: str, text: str) -> Path:
    source = OUT / f"_{name}.c"
    source.write_text(
        "#include <stdio.h>\n"
        f'int main(void) {{ printf("{text}\\n"); return 0; }}\n',
        encoding="ascii",
    )
    exe = OUT / f"{name}.exe"
    try:
        print(f"[build] {name}.exe")
        compile_source(compiler, source, exe)
    finally:
        source.unlink(missing_ok=True)
    return exe


def build_combined(compiler: str) -> Path:
    lines = ["#include <stdio.h>", "", "int main(void) {"]
    for name, text in PROGRAMS.items():
        lines.append(f'    printf("[{name}] {text}\\n");')
    lines += ["    return 0;", "}", ""]

    source = OUT / "_combined.c"
    source.write_text("\n".join(lines), encoding="ascii")
    exe = OUT / COMBINED
    try:
        print(f"[build] {COMBINED}")
        compile_source(compiler, source, exe)
    finally:
        source.unlink(missing_ok=True)
    return exe


def main() -> int:
    compiler = find_compiler()
    print(f"[compiler] {compiler}")

    if OUT.exists():
        shutil.rmtree(OUT)
    OUT.mkdir()

    for name, text in PROGRAMS.items():
        build_individual(compiler, name, text)

    build_combined(compiler)

    print(f"[done] 패키징 완료 -> {OUT}")
    for item in sorted(OUT.iterdir()):
        print(f"  - {item.name}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
