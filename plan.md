# 계획 및 작업 로그 (Plan & Work Log)

이 문서는 `2739` 저장소의 작업 내역·세부 수정·계획을 누적 기록하는 로그입니다.
작업이 진행될 때마다 아래에 항목을 추가합니다.

## 작업 로그

### 2026-07-16
- 저장소 초기화(빈 저장소): `main` 브랜치 생성 (PR #1, 머지됨)
- 스타터 소스 파일 추가 (PR #1, 머지됨): `main.c`, `effect.c`, `effect2.c`, `reboot.c`, `start.c`, `start.py`, `license.md`
- `start.py`가 `main.c`를 빌드·실행하도록 구현 (PR #3)
- Windows exe 빌드/시작프로그램 등록/패키징 파이프라인 추가 (PR #4)
  - `effect.c`/`effect2.c`/`start.c`에 `main()` 추가해 독립 실행 파일화
  - `reboot.c`를 실제 Windows 재부팅 코드로 구현 (수동 실행 전용, 시작프로그램 제외)
  - `build.py`, `install.py`, `package.py` 추가
- `license.md`에 한국어 번역본 추가 (PR #5, 머지됨)
- `license.md`를 독점 사용 약관(EULA)으로 전면 교체 (PR #6, 머지됨)
  - 약관 미동의 시 사용 전면 불가, 실행 시 동의 간주
  - 실행/사용 결과는 사용자 부담, 저작권자 면책
  - 2차 배포·수정 등 금지
- `README.md` 및 `plan.md` 추가

## 계획 (TODO)
- (앞으로 할 작업을 여기에 추가)

## 메모
- 이 로그는 작업/수정/계획을 계속 누적 기록합니다.
