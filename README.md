# 2739

Windows용 소규모 C 프로그램 모음과 Python 실행 스크립트입니다.

## 구성
| 파일 | 설명 |
| --- | --- |
| `main.c` | "Hello, world!" 출력 |
| `effect.c` | "effect" 출력 |
| `effect2.c` | "effect2" 출력 |
| `start.c` | "start" 출력 |
| `reboot.c` | 실행 시 즉시 재부팅 (Windows) — **수동 실행 전용** |
| `start.py` | Python 실행 스크립트 |
| `license.md` | 사용 약관 (EULA) |

> `reboot`은 실행 즉시 컴퓨터를 재부팅합니다. 저장하지 않은 데이터가 손실될 수 있으니 주의하세요.

## 빌드 & 실행
각 C 프로그램은 표준 C 컴파일러로 빌드합니다.

```bash
# 예: main.c 빌드 후 실행
gcc main.c -o main
./main
```

Windows용 exe로 크로스컴파일(Linux)하려면 mingw-w64를 사용합니다.

```bash
x86_64-w64-mingw32-gcc main.c -o main.exe
```

## 라이선스
본 소프트웨어는 독점 사용 약관(EULA)을 따릅니다. 자세한 내용은 [`license.md`](license.md)를 참고하세요.
소프트웨어를 실행하면 약관에 동의한 것으로 간주됩니다.
