# 임베디드 시스템 실험 - GPIO 제어 프로젝트

---

## 목차
- [프로젝트 개요](#프로젝트-개요)
- [목표](#목표)
- [실험 장비](#실험-장비)
- [프로젝트 설정](#프로젝트-설정)
  - [1. 개발 환경 구성](#1-개발-환경-구성)
  - [2. GPIO 및 레지스터 설정](#2-gpio-및-레지스터-설정)
  - [3. 코드 작성](#3-코드-작성)
- [실험 과정](#실험-과정)
- [실험 결과](#실험-결과)

---

## 프로젝트 개요
이 프로젝트는 STM32F107VCT6 마이크로컨트롤러를 사용하여 GPIO(범용 입출력) 제어를 통해 LED를 제어하는 실습입니다. 실습을 통해 임베디드 시스템 설계의 기본 원리를 이해하고, GPIO 포트를 조작하여 임베디드 펌웨어를 개발하는 경험을 쌓는 것을 목표로 합니다.

## 목표
- IAR Embedded Workbench를 활용한 개발 환경 구축 방법 이해
- 데이터시트와 레퍼런스 매뉴얼을 통해 레지스터 설정 및 제어법 습득
- GPIO를 사용하여 LED를 제어하는 실습 수행

## 실험 장비
- **마이크로컨트롤러**: STM32F107VCT6
- **개발 환경**: IAR Embedded Workbench
- **전원 공급**: 5V 1A (USB 또는 어댑터 사용, 5V 5A 어댑터는 사용 금지)

## 프로젝트 설정

### 1. 개발 환경 구성
1. **IAR Embedded Workbench 설치**: STM32F107VC 마이크로컨트롤러에 맞게 설정합니다.
2. **프로젝트 생성**:
   - 원하는 경로에 새로운 프로젝트 폴더를 생성하고, 서포트 파일을 추가합니다.
   - IAR Embedded Workbench에서 빈 프로젝트를 생성하여 `.c`, `.h` 파일을 추가합니다.

### 2. GPIO 및 레지스터 설정
1. **RCC 설정**: GPIO 포트에 클록을 활성화합니다. `RCC` 레지스터의 base 주소에 clock enable 값을 추가하여 포트 활성화가 가능합니다.
2. **GPIO 포트 및 핀 설정**: 각 포트의 시작 주소와 offset 값을 활용하여 LED와 버튼에 맞는 GPIO 포트를 설정합니다.
3. **LED 제어 로직**: 버튼 입력을 감지하여 각 버튼에 해당하는 LED를 켜고 끄는 제어 로직을 작성합니다. 반복문을 통해 지속적으로 입력 상태를 확인하고 LED 상태를 업데이트합니다.

### 3. 코드 작성
- `main.c` 파일 내에 레지스터와 포트 주소를 정의하고, 이를 통해 LED 제어 로직을 작성합니다.
- **레지스터 접근 예시**: RCC base 주소에 offset을 더하여 특정 레지스터에 접근하고, 비트 연산을 통해 포트의 입력과 출력을 제어합니다.

```c
// GPIO 포트 설정 예시 코드
volatile unsigned int *RCC_APB2ENR = (unsigned int *)0x40021018;
*RCC_APB2ENR |= 0x3C; // 포트 A, B, C, D 활성화

volatile unsigned int *GPIOC_ODR = (unsigned int *)0x4001100C;
*GPIOC_ODR |= 0x00000001; // LED ON
*GPIOC_ODR &= ~0x00000001; // LED OFF
```

### 4. 실험 과정
1. 하드웨어 연결: JTAG과 5V 전원을 연결하여 보드가 올바르게 작동하도록 설정합니다.
2. 디버깅 모드: IAR에서 디버깅 모드로 보드에 코드를 다운로드하고 실행 상태를 확인합니다.
3. LED 동작 확인: 버튼을 누르면 해당하는 LED가 켜지고, 다른 버튼을 누르면 LED가 꺼지는지 확인합니다.

## 실험 결과
- **KEY1 버튼**: LED1, LED4 ON
- **KEY2 버튼**: LED1, LED4 OFF
- **KEY3 버튼**: LED2, LED3 ON
- **KEY4 버튼**: LED2, LED3 OFF
