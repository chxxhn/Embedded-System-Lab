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

### 2. **PWM(TIM3) 설정**
- **서보모터 제어를 위한 PWM 신호 출력 (TIM3)**
- `TIM3`의 PWM 신호를 이용하여 **서보모터의 각도를 조절**
- **PWM 신호의 듀티 사이클을 조절하여 0도, 90도, -90도 회전 구현**
- `CCR` 값을 변경하여 각도를 조절하며, **1.0ms(0도), 1.5ms(90도), 2.0ms(-90도)로 설정**

```c
TIM_OCInitTypeDef TIM_OCInitStructure;
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 1500;  // 기본 위치 (90도)
TIM_OC3Init(TIM3, &TIM_OCInitStructure);
```

### 3. **LCD 및 터치 입력 설정**
- **TFT-LCD에 팀명 및 LED 상태 출력**
- 터치 버튼을 생성하여 `ON/OFF` 제어
- **ON 버튼 클릭 시 TIM2 및 TIM3 활성화**, **OFF 버튼 클릭 시 LED 및 서보모터 동작 중지**
- 터치 입력 좌표를 읽어 버튼 영역과 비교 후 동작 수행
  
```c
Touch_GetXY(&x, &y, 1);
Convert_Pos(x, y, &nx, &ny);
if (nx >= 50 && nx <= 100 && ny >= 50 && ny <= 100) {
    btn = !btn;
    LCD_ShowString(0, 20, btn ? " ON" : "OFF", RED, WHITE);
}
```

---

## 실험 과정
1. **RCC & GPIO 초기화**
   - TIM2, TIM3을 활성화하고 GPIO를 설정하여 **LED 및 서보모터 핀을 초기화**
   - `AFIO` 설정을 활성화하여 PWM 신호 출력 가능하도록 설정

2. **타이머(TIM2) 설정**
   - `TIM2`를 사용하여 **1초마다 LED1 점멸, 5초마다 LED2 점멸** 기능 구현
   - `TIM2_IRQHandler()`를 이용하여 **인터럽트 발생 시 LED 상태 변경**

3. **PWM(TIM3) 설정**
   - `TIM3`의 PWM 신호를 이용하여 **서보모터 각도를 변경**
   - ON 버튼 터치 시 **1초마다 서보모터가 한쪽 방향으로 이동**
   - OFF 버튼 터치 시 **1초마다 반대 방향으로 이동**

4. **TFT-LCD 및 터치 이벤트 처리**
   - LCD에 **팀명(WED_Team02) 및 LED 상태 출력**
   - 터치 입력을 받아 **LED ON/OFF 및 서보모터 제어**
   - 터치된 위치를 `x, y` 좌표로 변환 후 버튼 영역과 비교하여 동작 수행

---

## 실험 결과
- **LCD 출력:**  
  - `"WED_Team02"`가 LCD에 표시됨  
  - **버튼 터치 시 `"ON"/"OFF"` 상태 전환 가능**
  
- **LED 점멸:**  
  - **ON 버튼 클릭 시** LED1이 **1초마다 토글**, LED2가 **5초마다 토글**
  - **OFF 버튼 클릭 시** LED가 모두 꺼짐
  
- **서보모터 동작:**  
  - **ON 버튼 클릭 시** 서보모터가 **1초마다 100씩 이동**
  - **OFF 버튼 클릭 시** 서보모터가 **1초마다 반대 방향으로 이동**
  
- **인터럽트 처리:**  
  - TIM2 인터럽트를 통해 **정확한 시간 간격으로 LED 토글 수행**
  
- **PWM 제어:**  
  - TIM3을 활용하여 **서보모터의 각도를 부드럽게 변경 가능**

---





