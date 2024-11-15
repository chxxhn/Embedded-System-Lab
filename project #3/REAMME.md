# Project #3 - Clock Tree 및 UART 통신
이 프로젝트는 STM32F107 마이크로컨트롤러에서 Clock Tree와 UART 통신을 이해하고 설정하는 실습입니다. 사용자 정의 클럭을 설정하고, UART 통신을 통해 PC와의 연결을 실현하며, 설정된 System Clock을 오실로스코프로 확인하는 방법을 학습합니다.

---

## 목차
- [목적](#목적)
- [실험 장비](#실험-장비)
- [프로젝트 설정](#프로젝트-설정)
  - [1. 개발 환경 구성](#1-개발-환경-구성)
  - [2. GPIO 및 레지스터 설정](#2-gpio-및-레지스터-설정)
  - [3. 코드 작성](#3-코드-작성)
- [실험 과정](#실험-과정)
- [실험 결과](#실험-결과)

---

## 목적
- Clock Tree의 이해 및 사용자 Clock 설정.
- UART 통신 원리 학습 및 설정 방법 이해.
- MCO를 통해 System Clock을 오실로스코프로 확인.
- UART 통신을 통해 터미널(Putty)에 "Hello Team02" 메시지 출력.

## 실험 장비
- **마이크로컨트롤러**: STM32F107VCT6
- **개발 환경**: IAR Embedded Workbench (EW)
- **장비**: 오실로스코프, UART 통신을 위한 시리얼 통신 프로그램 (Putty)

## 프로젝트 설정
### 1. 개발 환경 구성
project #1와 동일한 방식으로 프로젝트를 생성하고 파일 구조를 설정합니다.

### 2. GPIO 및 레지스터 설정
- **MCO 설정**: System Clock을 출력하도록 설정.
- **UART 설정**: 지정된 Baud Rate로 설정.
- **Baud Rate**: 9600bps

### 3. 코드 작성
Clock과 UART를 설정하고, 버튼을 누를 때마다 putty에 "Hello Team02"메시지가 출력되도록 작성합니다.

```c
// 주요 코드 요약
void SendData(char *message) {
    while (*message) {
        USART1->DR = *message++;  // 데이터 레지스터에 문자 전송
        while (!(USART1->SR & USART_SR_TC));  // 전송 완료 대기
    }
}

// 메시지 출력
if (KEY4 눌림) {
    SendData("Hello Team02\r\n");  // Putty에 메시지 출력 및 줄 바꿈
}
```

## 실험 과정
1. **프로젝트 생성 및 파일 구조 설정**: 4주차와 동일한 방식으로 프로젝트를 생성하고 파일 구조를 설정합니다.
2. **Clock 설정**: PLL과 HCLK, PCLK2를 설정하여 원하는 System Clock을 구성합니다.
3. **UART 설정**: Baud rate를 지정하여 UART 통신이 설정되도록 합니다.
4. **코드 작성**: TODO 코드 부분에 맞춰 Clock과 UART를 설정하고, 버튼을 누를 때마다 "Hello Team02" 메시지를 전송합니다.
5. **오실로스코프 확인**: MCO 핀을 오실로스코프에 연결하여 설정된 System Clock 주파수를 확인합니다.
6. **시리얼 통신 확인**: Putty합니다.


