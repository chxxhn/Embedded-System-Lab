# Project #8 - DMA를 활용한 조도 센서 및 TFT-LCD 제어

이 프로젝트는 **STM32F107 보드에서 DMA를 활용하여 조도 센서의 아날로그 데이터를 디지털 변환하고, TFT-LCD에 실시간으로 표시하는 실험**입니다.  
ADC와 DMA를 연동하여 인터럽트 없이 데이터를 주기적으로 갱신하며, 조도 값 변화에 따라 **LCD 배경색을 자동 변경**합니다.

---

## 목차
- [목적](#목적)
- [실험 장비](#실험-장비)
- [프로젝트 설정](#프로젝트-설정)
  - [1. ADC 및 DMA 설정](#1-adc-및-dma-설정)
  - [2. LCD 및 터치 입력 설정](#2-lcd-및-터치-입력-설정)
- [실험 과정](#실험-과정)
- [실험 결과](#실험-결과)

---

## 목적
- **ADC(Analog-to-Digital Converter)** 개념 및 활용법 학습
- **DMA(Direct Memory Access) 기술**을 이용한 데이터 전송 및 성능 최적화
- **TFT-LCD 제어 방법 학습 및 실시간 데이터 시각화**
- **조도 센서를 활용한 환경 변화 감지 및 화면 반응형 UI 구현**

---

## 실험 장비
- **STM32F107VCT6 개발 보드**
- **TFT-LCD 디스플레이**
- **조도 센서 (Light Sensor)**
- **전원 공급 장치 (5V, 1A)**
- **개발 환경**: Keil uVision / IAR Embedded Workbench (EWARM)

---

## 프로젝트 설정

### 1. **ADC 및 DMA 설정**
- **ADC1을 활성화하여 조도 센서 값을 디지털 변환**
- **DMA1을 설정하여 변환된 ADC 값을 자동으로 메모리(변수)로 저장**
- **ADC1 채널 8을 사용하여 GPIOB 핀에서 조도 센서 입력을 수집**
- ADC 변환이 완료되면, DMA가 직접 `ADC_Value` 변수에 값을 저장

```c
void AdcInit(void) {
    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_Init(ADC1, &ADC_InitStructure);  

    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
```

- **DMA를 설정하여 ADC 데이터를 메모리에 자동 저장**
- 데이터가 계속 변환되므로, 별도의 인터럽트 없이 실시간 데이터 처리가 가능
  
```c
void DMA_Configure(void) {
    DMA_InitTypeDef DMA_InitStructure;

    DMA_DeInit(DMA1_Channel1);
    DMA_StructInit(&DMA_InitStructure);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_Value;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;

    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}
```

### 2. **LCD 및 터치 입력 설정**
- **TFT-LCD에 팀명 및 현재 조도 센서 값 표시**
- ADC 변환이 완료되면, DMA가 직접 `ADC_Value` 변수에 값을 저장
- 조도 값 변화에 따라 **LCD 배경색 자동 변경**
  - 조도 값이 3400 이하일 경우 **배경색: GRAY**
  - 조도 값이 4050 이하일 경우 **배경색: WHITE**

 ```c
LCD_Init();
Touch_Configuration();
Touch_Adjust();
LCD_Clear(GRAY);

// 초기 화면 출력
LCD_ShowString(40, 50, "TUE_Team05", WHITE, GRAY);
LCD_ShowNum(40, 100, ADC_Value, 4, WHITE, GRAY);
```

---

## 실험 과정
1. **RCC & GPIO 초기화**
   - `RccInit()`을 사용하여 **ADC1, GPIOB, DMA1 클럭 활성화**
   - `GpioInit()`을 통해 **조도 센서 입력을 위한 GPIO 설정**

2. **ADC 및 DMA 설정**
   - `AdcInit()`에서 **ADC1을 설정하여 조도 센서 값을 읽을 수 있도록 구성**
   - `DMA_Configure()`를 통해 **ADC 값을 메모리(ADC_Value 변수)로 직접 저장**
   - **Interrupt 없이 DMA를 이용한 ADC 데이터 수집**이 가능하도록 설정

3. **TFT-LCD 초기화 및 UI 표시**
   - `LCD_Init()` 및 `Touch_Configuration()`을 호출하여 **LCD 설정**
   - 기본 배경색 **GRAY**, 팀명("TUE_Team05")과 ADC 값을 출력

4. **조도 센서 값을 TFT-LCD에 출력**
   - ADC 값을 읽어 **3400 이하일 경우 LCD 배경을 GRAY**, **4050 이상일 경우 WHITE로 변경**
   - 배경색이 변경되면 텍스트 색상도 자동으로 조절하여 가독성을 유지

5. **무한 루프에서 지속적으로 조도 값 업데이트**
   - `while(1)` 루프에서 ADC 값을 주기적으로 읽고, **LCD에 갱신**
   - LCD 업데이트 후 `Delay()`를 이용하여 일정 시간 대기

---

## 실험 결과
- **LCD 출력:**
  - `"TUE_Team05"`가 LCD에 표시되며, **현재 조도 센서 값도 함께 출력**
  - 조도 값이 업데이트될 때마다 LCD에 새로운 값이 반영됨

- **조도 센서 값 변화에 따른 배경색 변경:**
  - **조도 값 ≤ 3400** → **LCD 배경색: GRAY, 글자 색상: WHITE**
  - **조도 값 ≥ 4050** → **LCD 배경색: WHITE, 글자 색상: GRAY**
  - **중간 값 (3400 ~ 4050)에서는 배경색 유지**

- **DMA를 이용한 ADC 값 처리:**
  - 인터럽트 없이 **DMA를 통해 ADC 값을 자동으로 메모리에 저장**
  - 실시간으로 조도 센서 값을 읽어 LCD에 표시하는 데 성공

- **안정적인 LCD 동작 확인:**
  - 스마트폰 플래시를 조도 센서에 비추면 **즉시 배경색이 변경**
  - 조도를 다시 낮추면 **다시 원래의 배경색으로 복귀**

---

