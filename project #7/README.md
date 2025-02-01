# Project #7 - Timer & PWM을 활용한 LED 및 서보모터 제어

이 프로젝트는 **STM32F107 보드에서 TIM2와 TIM3 타이머를 활용하여 LED 점멸 및 서보모터 제어**를 수행하는 실습입니다.  
TFT-LCD를 이용하여 터치 버튼을 구현하고, 버튼 입력을 통해 **LED ON/OFF 제어 및 서보모터의 각도 변경**을 수행합니다.

---

## 목차
- [목적](#목적)
- [실험 장비](#실험-장비)
- [프로젝트 설정](#프로젝트-설정)
  - [1. 타이머(TIM2) 설정](#1-타이머tim2-설정)
  - [2. PWM(TIM3) 설정](#2-pwmtim3-설정)
  - [3. LCD 및 터치 입력 설정](#3-lcd-및-터치-입력-설정)
- [실험 과정](#실험-과정)
- [실험 결과](#실험-결과)

---

## 목적
- **Timer의 개념과 종류**를 학습하고, TIM2와 TIM3 타이머를 활용한 임베디드 시스템 설계를 실습
- **PWM(Pulse Width Modulation)** 신호를 활용한 서보모터 제어 방법 학습
- **TFT-LCD 터치 입력**을 활용하여 사용자 인터페이스 구현
- **임베디드 시스템의 인터럽트 기반 동작 이해** (TIM2 인터럽트 활용)

---

## 실험 장비
- **STM32F107VCT6 개발 보드**
- **TFT-LCD 디스플레이**
- **서보모터 (SG90)**
- **전원 공급 장치 (5V, 1A)**
- **개발 환경**: Keil uVision / IAR Embedded Workbench (EWARM)

---

## 프로젝트 설정

### 1. **타이머(TIM2) 설정**
- **LED 점멸을 위한 타이머 (TIM2)**
- `TIM2`의 **인터럽트 기능**을 활용하여 **1초마다 LED1 토글**, **5초마다 LED2 토글**이 실행되도록 설정
- `Prescaler`와 `Period`를 이용하여 타이머의 **분주율 설정**
- `TIM2_IRQHandler()` ISR을 통해 인터럽트 발생 시 LED 상태 변경

```c
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_TimeBaseStructure.TIM_Period = 10000;   // 1초 주기
TIM_TimeBaseStructure.TIM_Prescaler = 7200; // 72MHz 분주
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
TIM_Cmd(TIM2, ENABLE);
TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
```

