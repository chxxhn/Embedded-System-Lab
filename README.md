# Embedded-System-Lab 🚀

**STM32F107VCT6** 마이크로컨트롤러를 활용한 다양한 임베디드 시스템 프로젝트입니다.
GPIO, UART, Timer, PWM, ADC, DMA, Bluetooth 등을 실습하며, **임베디드 시스템 설계 및 제어 기술을 익히는 것**을 목표로 합니다.

---

## 📌 프로젝트 개요

### 🔹 **Project #1 - GPIO 제어**
**GPIO 핀을 활용하여 LED를 제어하는 프로젝트**
- STM32F107의 GPIO 포트를 활성화하여 LED 점멸 실습
- 버튼 입력을 통해 LED ON/OFF 제어
- 직접 메모리 접근 방식으로 GPIO 레지스터 조작

📂 **코드 경로**: [`Project1_GPIO_Control/`](./Project%20%23#1)

---

### 🔹 **Project #2 - 폴링 방식의 릴레이 모듈 제어**
**폴링 방식을 활용하여 릴레이 모듈을 제어하는 실습**
- GPIO 핀을 이용해 릴레이 스위칭
- 일정 시간 동안 릴레이 ON/OFF 구현

📂 **코드 경로**: [`Project2_Relay_Polling/`](./Project2_Relay_Polling)

---

### 🔹 **Project #3 - Clock Tree 및 UART 통신**
**STM32의 Clock Tree를 설정하고, UART를 이용한 시리얼 통신 실습**
- System Clock 설정 및 MCO 출력
- UART를 이용해 PC와 통신하여 데이터 송수신

📂 **코드 경로**: [`Project3_ClockTree_UART/`](./Project3_ClockTree_UART)

---

### 🔹 **Project #4 - 인터럽트 기반 GPIO 제어 및 UART 통신**
**EXTI 인터럽트를 이용하여 버튼 이벤트 처리 및 UART 통신 구현**
- GPIO 인터럽트(EXTI)를 활용하여 버튼을 누를 때마다 LED 상태 변경
- UART를 통한 데이터 전송 및 이벤트 처리

📂 **코드 경로**: [`Project4_GPIO_Interrupt_UART/`](./Project4_GPIO_Interrupt_UART)

---

### 🔹 **Project #5 - Bluetooth 모듈과 UART 통신**
**STM32F107과 Bluetooth 모듈(FB755AC)을 연결하여 PC 및 스마트폰과 무선 통신**
- UART를 이용한 Bluetooth 데이터 송수신
- 스마트폰 앱과 연동하여 데이터 송수신

📂 **코드 경로**: [`Project5_Bluetooth_UART/`](./Project5_Bluetooth_UART)

---

### 🔹 **Project #6 - LCD 및 ADC 실험**
**STM32 보드에서 TFT-LCD와 ADC를 활용한 조도 센서 실험**
- ADC 값을 TFT-LCD에 실시간으로 출력
- 터치 입력을 활용하여 LCD UI 제어

📂 **코드 경로**: [`Project6_LCD_ADC/`](./Project6_LCD_ADC)

---

### 🔹 **Project #7 - Timer & PWM을 활용한 LED 및 서보모터 제어**
**TIM2 타이머 인터럽트와 TIM3 PWM을 이용하여 서보모터 제어**
- TIM2 인터럽트를 활용한 LED 점멸
- TIM3 PWM을 이용한 서보모터 각도 변경
- TFT-LCD 터치 버튼을 통해 모터 제어

📂 **코드 경로**: [`Project7_Timer_PWM_Servo/`](./Project7_Timer_PWM_Servo)

---

### 🔹 **Project #8 - DMA를 활용한 조도 센서 및 TFT-LCD 제어**
**DMA를 활용하여 ADC 데이터를 자동으로 갱신하고 TFT-LCD에 실시간 표시**
- ADC 변환 값을 DMA를 이용하여 자동으로 메모리 저장
- 조도 센서 값을 읽어 LCD 배경색을 실시간 변경
- 인터럽트 없이 DMA를 통해 빠르고 효율적인 데이터 전송

📂 **코드 경로**: [`Project8_DMA_ADC_LCD/`](./Project8_DMA_ADC_LCD)

---

## 🔧 사용된 기술 스택
- **임베디드 프로그래밍 언어**: C (ARM Cortex-M3 기반)
- **개발 환경**: IAR Embedded Workbench, Keil uVision
- **하드웨어**: STM32F107VCT6 보드, TFT-LCD, 조도 센서, 서보모터, Bluetooth 모듈
- **통신 방식**: UART, Bluetooth, GPIO 인터럽트
- **제어 방식**: Timer, PWM, ADC, DMA 활용

---

## 📚 참고 문서
- 📄 **[STM32F107 Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190.pdf)**
- 📄 **[IAR Embedded Workbench User Guide](https://www.iar.com/ewarm)**

---

