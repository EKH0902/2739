# 2739

Windows용 소규모 프로그램 모음과 이를 exe로 빌드·패키징하는 스크립트입니다.

## 구성 프로그램
| 소스 | 설명 | 시작프로그램 등록 |
| --- | --- | --- |
| `main.c` | "Hello, world!" 출력 | O |
| `effect.c` | "effect" 출력 | O |
| `effect2.c` | "effect2" 출력 | O |
| `start.c` | "start" 출력 | O |
| `reboot.c` | 실행 시 즉시 재부팅 (Windows) | **X (수동 실행 전용)** |

> `reboot`은 의도적으로 시작프로그램에서 제외됩니다. (부팅마다 재부팅되는 부트루프 방지)

## 빌드
각 C 프로그램을 Windows `.exe`로 컴파일합니다. 산출물은 `dist/`에 생성됩니다.

```
python build.py
```

- Windows: 네이티브 `gcc`/`cc` 사용
- Linux/기타: `x86_64-w64-mingw32-gcc`(mingw-w64) 크로스컴파일로 Windows exe 생성

## 시작프로그램 등록 (Windows)
`reboot`을 제외한 모든 프로그램을 현재 사용자 `Run` 레지스트리 키에 등록합니다.

```
python install.py            # 등록
python install.py --uninstall  # 해제
```

## 단일 exe로 패키징 (Windows에서 실행)
설치 프로그램과 빌드된 exe들을 하나의 `.exe`로 묶습니다.

```
python build.py
pip install pyinstaller
python package.py
# -> dist/app2739.exe
```

`app2739.exe`를 실행하면 번들된 프로그램들을 영구 설치 경로(`%LOCALAPPDATA%\app2739`)로
복사한 뒤 `reboot`을 제외하고 시작프로그램으로 등록합니다.

> 단일 exe는 PyInstaller 특성상 **Windows에서 빌드해야** Windows용 exe가 나옵니다.
