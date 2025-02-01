/**
 * STM32F10x 모터 제어 및 센서 인터페이스 프로그램
 * 기능:
 * 1. 초음파 거리 센서(HC-SR04)를 이용하여 거리 측정
 * 2. 압력 센서를 사용하여 트리거 동작
 * 3. LCD 디스플레이로 상태 표시 (열림, 닫힘, 행복한 얼굴, 화난 얼굴 등)
 * 4. 모터의 정방향 및 역방향 제어
 * 5. NVIC 인터럽트를 통한 UART 통신 처리
 */
#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"

// HC-SR04 초음파 센서(거리센서) 핀 설정
#define TRIG_PIN GPIO_Pin_0   // TRIG 핀: PA0
#define ECHO_PIN GPIO_Pin_1   // ECHO 핀: PA1
#define TRIG_PORT GPIOA
#define ECHO_PORT GPIOA

// 모터 핀 (PD1, PD2 사용)
#define GPIOC_BSRR (*(volatile unsigned int *)0x40011010)

// ADC(압력 센서) 설정
#define PRESSURE_SENSOR_PIN GPIO_Pin_2 // PA2
#define ADC_CHANNEL ADC_Channel_2     // ADC 채널 변경

// 함수 선언
void GPIO_Init_Config(void);
void Timer_Config(void);
void Motor_Config(void);
void Delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void delay(void);

//거리센서 관련 제어 함수
float Get_Distance(void);

//압력 센서 제어 관련 함수
void RCC_Configure(void);
void GPIO_Configure(void);
void ADC_Configure(void);
uint16_t Read_ADC_Value(void);
void NVIC_Configure(void); // 압력 센서 인터럽트 설정

//LCD 제어 관련 함수
void LCD_DrawAngryFace(void);
void LCD_DrawHappyFace(void);
void LCD_drawOpen(void);
void LCD_drawClose(void);

//블루투스 제어 관련 함수
void bt_init(void);
void bt_rcc_configure(void);
void bt_gpio_configure(void);
void bt_usart1_configure(void);
void bt_usart2_configure(void);
void bt_nvic_configure(void); // UART1/2 인터럽트 설정
void bt_send_to_user(char *message);

// 모터 제어 관련 함수
void motor_stop(void);
void motor_forward(void);
void motor_reverse(void);

//전역 변수 선언
int flag = 0; // 모터 상태를 나타내는 플래그 변수 (0: 뚜껑닫힘, 1: 뚜껑열림)
volatile uint16_t pressure_value = 0; // 압력 값 저장 (디버깅으로 확인 가능)
uint16_t is_connected = 0; // UART 연결 여부
uint16_t user_input = 0; // UART 사용자 입력 값

int main(void) {
    SystemInit();           // 시스템 초기화
    GPIO_Init_Config();     // GPIO 초기화 (거리 센서)
    Timer_Config();         // 타이머 초기화
    Motor_Config();         // 모터 GPIO 설정
    NVIC_Configure();       // NVIC 인터럽트 설정

    RCC_Configure();        // 클럭 설정
    GPIO_Configure();       // GPIO 설정 (압력 센서)
    ADC_Configure();        // ADC 설정

    bt_init();              // 블루투스 초기화
    LCD_Init();             // LCD 초기화

    float distance; // 거리 측정(디버깅 확인용)
   
    while (1) {
        //거리 측정
        distance = Get_Distance(); 
        printf("Distance: %.2f cm\r\n", distance);

        // 뚜껑 열림 처리
        if (distance > 0 && distance <= 5) {
            if (flag == 0) { // 모터가 OFF 상태일 때만 ON으로 변경
                flag = 1;    // 상태를 "열림"으로 설정
                printf("Motor ON\r\n");
                LCD_drawOpen();
                motor_forward(); // 모터 정방향 동작
            }
        }
        // 뚜껑 닫힘 처리
        else {
            if (flag == 1) { // 모터가 ON 상태일 때만 OFF로 변경
                flag = 0;    // 상태를 "닫힘"으로 설정
                printf("Motor OFF\r\n");
                LCD_drawClose();
                motor_reverse(); // 모터 역방향 동작
                
                // 압력 센서
                pressure_value = Read_ADC_Value();
                printf("Pressure Value: %d\r\n", pressure_value);

                // 압력 값에 따라 블루투스 메시지 전송 및 LCD 출력
                if (pressure_value >= 1500) {
                    LCD_DrawAngryFace(); // 화난 표정 출력
                    printf("LCD angry face\r\n");
                    bt_send_to_user("Remove trash\n"); // 블루투스 메시지 전송
                } else {
                    LCD_DrawHappyFace(); // 웃는 표정 출력
                    printf("LCD happy face\r\n");
                }
                delay();
            }
        }

        delay();
    }
}

void delay(void) {
    for (volatile int i = 0; i < 1000000; i++);
}

// 모터 GPIO 설정
void Motor_Config(void) {
    // GPIOD 클럭 활성화
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // GPIOD 핀 설정 구조체 초기화
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2; // 모터 제어 핀: PD1, PD2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       // Push-Pull 출력 모드
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 출력 속도 50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);                 // GPIO 설정 적용

    
    motor_stop(); // 초기 상태 설정 (정지)
}

void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

//모터 정방향 동작
void motor_forward(void) {
    // 정방향: PD1 Low, PD2 High
    GPIO_ResetBits(GPIOD, GPIO_Pin_1);
    GPIO_SetBits(GPIOD, GPIO_Pin_2);
    printf("Motor forward\n");
    delay_ms(2000); // 2초 동안 동작
    motor_stop();
}

//모터 역방향 동작
void motor_reverse(void) {
    // 역방향: PD1 High, PD2 Low
    GPIO_SetBits(GPIOD, GPIO_Pin_1);
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    printf("Motor reverse\n");
    delay_ms(2000); // 2초 동안 동작
    motor_stop();
}

//모터 정지 동작
void motor_stop(void) {
    // 정지: PD1 Low, PD2 Low
    GPIO_ResetBits(GPIOD, GPIO_Pin_1);
    GPIO_ResetBits(GPIOD, GPIO_Pin_2);
    printf("Motor stop\n");
}

// GPIO 초기화 (HC-SR04)
void GPIO_Init_Config(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOA 클럭 활성화

    GPIO_InitTypeDef GPIO_InitStruct;

    // TRIG 핀 (출력)
    GPIO_InitStruct.GPIO_Pin = TRIG_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TRIG_PORT, &GPIO_InitStruct);

    // ECHO 핀 (입력)
    GPIO_InitStruct.GPIO_Pin = ECHO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD; // 풀다운 입력
    GPIO_Init(ECHO_PORT, &GPIO_InitStruct);
}


// 타이머 초기화
void Timer_Config(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // TIM2 클럭 활성화

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);

    TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;       // 1MHz 타이머 (1us 단위)
    TIM_TimeBaseInitStruct.TIM_Period = 0xFFFFFFFF;      // 최대 값
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

    TIM_Cmd(TIM2, ENABLE); // 타이머 시작
}

// us 단위 딜레이
void Delay_us(uint32_t us) {
    TIM_SetCounter(TIM2, 0); // 타이머 카운터 초기화
    while (TIM_GetCounter(TIM2) < us);
}

// 거리 측정 함수
float Get_Distance(void) {
    uint32_t start_time, end_time;

    // TRIG 핀 HIGH로 10us 신호 송출
    GPIO_SetBits(TRIG_PORT, TRIG_PIN);
    Delay_us(10);
    GPIO_ResetBits(TRIG_PORT, TRIG_PIN);

    // ECHO 핀 HIGH 상태 대기
    while (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == Bit_RESET);
    start_time = TIM_GetCounter(TIM2);

    // ECHO 핀 LOW 상태 대기
    while (GPIO_ReadInputDataBit(ECHO_PORT, ECHO_PIN) == Bit_SET);
    end_time = TIM_GetCounter(TIM2);

    // 거리 계산
    return ((end_time - start_time) * 0.0343f) / 2.0f;
}

void RCC_Configure(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = PRESSURE_SENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_Configure(void) {
    ADC_InitTypeDef ADC_InitStructure;

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_CHANNEL, 1, ADC_SampleTime_239Cycles5);

    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

uint16_t Read_ADC_Value(void) {
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    return ADC_GetConversionValue(ADC1);
}

//ADC1 인터럽트 설정
void NVIC_Configure(void) {
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_EnableIRQ(ADC1_2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void bt_init(void)
{
    bt_rcc_configure();
    bt_gpio_configure();
    bt_usart1_configure();
    bt_usart2_configure();
    bt_nvic_configure();
}

void bt_rcc_configure(void)
{
    /* USART2 TX/RX port clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    /* USART2 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Alternate Function IO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void bt_gpio_configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // USART1
    // TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART2
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    // TX (PD5)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    // RX (PD6)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void bt_usart1_configure(void)
{
    USART_InitTypeDef USART1_InitStructure;

    // Enable the USART1 peripheral
    USART_Cmd(USART1, ENABLE);
    USART1_InitStructure.USART_BaudRate = 9600;
    USART1_InitStructure.USART_StopBits = USART_StopBits_1;
    USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART1_InitStructure.USART_Parity = USART_Parity_No;
    USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART1_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void bt_usart2_configure(void)
{
    USART_InitTypeDef USART2_InitStructure;

    // Enable the USART2 peripheral
    USART_Cmd(USART2, ENABLE);
    
    USART2_InitStructure.USART_BaudRate = 9600;
    USART2_InitStructure.USART_StopBits = USART_StopBits_1;
    USART2_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART2_InitStructure.USART_Parity = USART_Parity_No;
    USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART2_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void bt_nvic_configure(void)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // USART1 인터럽트
    // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // USART2 인터럽트 설정
    // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//USART1 인터럽트 핸들러
//UART1 수신 처리
void USART1_IRQHandler()
{
    uint16_t word;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        word = USART_ReceiveData(USART1); //데이터 수신
        USART_SendData(USART2, word); //수신 데이터 전송
        USART_ClearITPendingBit(USART1, USART_IT_RXNE); //인터럽트 플래그 클리어
    }
}

//USART2 인터럽트 핸들러
//UART2 수신 처리
void USART2_IRQHandler()
{
    uint16_t word;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        word = USART_ReceiveData(USART2); //데이터 수신
        is_connected = 1; //연결 상태 업데이트
        USART_SendData(USART1, word); // 수신 데이터 전송
        user_input = word; // 사용자 입력 저장
        USART_ClearITPendingBit(USART2, USART_IT_RXNE); // 인터럽트 플래그 클리어
    }
}

uint16_t bt_get_user_input(void)
{
    uint16_t result = user_input;
    user_input = 122;
    return result;
}

void bt_send_to_user(char *message)
{
    for (int i = 0; message[i] != '\0'; i++)
    {
        while ((USART2->SR & USART_SR_TXE) == 0);
        USART_SendData(USART2, message[i]);
    }
    for (int i = 0; message[i] != '\0'; i++)
    {
        while ((USART1->SR & USART_SR_TXE) == 0); 
        USART_SendData(USART1, message[i]);
    }
}


void LCD_DrawAngyFace(void) { // 슬픈 얼굴 출력
    LCD_Clear(WHITE);

    // 중앙 좌표 계산 
    uint16_t center_x = 120; 
    uint16_t center_y = 160; 

    // 눈 좌표와 반지름
    uint16_t eye_radius = 12; 
    uint16_t left_eye_x = center_x - 40; // 왼쪽 눈 X 좌표
    uint16_t right_eye_x = center_x + 40; // 오른쪽 눈 X 좌표
    uint16_t eye_y = center_y - 40;     // 눈 Y 좌표

    // 입 좌표
    uint16_t mouth_start_x = center_x - 50; // 입 시작 X 좌표
    uint16_t mouth_end_x = center_x + 50;  // 입 끝 X 좌표
    uint16_t mouth_center_y = center_y + 40; // 입 중심 Y 좌표
    uint16_t mouth_curve_height = 30;      // 입 곡선 높이

    // 눈 그리기 (굵게)
    for (int i = 0; i < 3; i++) { // 3번 반복으로 굵게 그리기
        LCD_DrawCircle(left_eye_x, eye_y, eye_radius - i);
        LCD_DrawCircle(right_eye_x, eye_y, eye_radius - i);
    }

    // 입 그리기 (상하 뒤집기 - 위로 볼록)
    for (int i = 0; i < 3; i++) { // 3번 반복으로 굵게 그리기
        for (uint16_t x = mouth_start_x; x < mouth_end_x; x += 2) {
            uint16_t y1 = mouth_center_y - (mouth_curve_height * (x - mouth_start_x) * (mouth_end_x - x)) /
                                          ((mouth_end_x - mouth_start_x) * (mouth_end_x - mouth_start_x) / 4);
            uint16_t y2 = mouth_center_y - (mouth_curve_height * (x + 2 - mouth_start_x) * (mouth_end_x - (x + 2))) /
                                          ((mouth_end_x - mouth_start_x) * (mouth_end_x - mouth_start_x) / 4);
            LCD_DrawLine(x, y1 - i, x + 2, y2 - i); // 위로 i 만큼 이동하며 굵게 그리기
        }
    }
}


void LCD_DrawHappyFace(void) { //웃는 얼굴 출력
    LCD_Clear(WHITE); // Clear the screen to white

    // 중앙 좌표 계산 (LCD 화면의 크기를 240x320으로 가정)
    uint16_t center_x = 120; 
    uint16_t center_y = 160; 

    // 눈 좌표와 반지름
    uint16_t eye_radius = 12;           
    uint16_t left_eye_x = center_x - 40; 
    uint16_t right_eye_x = center_x + 40; 
    uint16_t eye_y = center_y - 40;     

    // 입 좌표
    uint16_t mouth_start_x = center_x - 50; 
    uint16_t mouth_end_x = center_x + 50;  
    uint16_t mouth_center_y = center_y + 40; 
    uint16_t mouth_curve_height = 30;

    // 눈 그리기 (굵게)
    for (int i = 0; i < 3; i++) { 
        LCD_DrawCircle(left_eye_x, eye_y, eye_radius - i);
        LCD_DrawCircle(right_eye_x, eye_y, eye_radius - i);
    }

    // 입 그리기 (굵게)
    for (int i = 0; i < 3; i++) { 
        for (uint16_t x = mouth_start_x; x < mouth_end_x; x += 2) {
            uint16_t y1 = mouth_center_y + (mouth_curve_height * (x - mouth_start_x) * (mouth_end_x - x)) /
                                          ((mouth_end_x - mouth_start_x) * (mouth_end_x - mouth_start_x) / 4);
            uint16_t y2 = mouth_center_y + (mouth_curve_height * (x + 2 - mouth_start_x) * (mouth_end_x - (x + 2))) /
                                          ((mouth_end_x - mouth_start_x) * (mouth_end_x - mouth_start_x) / 4);
            LCD_DrawLine(x, y1 - i, x + 2, y2 - i); 
        }
    }
}


void LCD_drawOpen(void) { // OPEN 글자 출력
    LCD_Clear(WHITE); // 화면 초기화

    uint16_t center_x = 120;
    uint16_t center_y = 160;

    // 글자 간격 및 크기
    uint16_t char_spacing = 50;   
    uint16_t char_width = 20;    
    uint16_t char_height = 40;    
    uint16_t thickness = 6;      
    uint16_t circle_radius = 10;  
    uint16_t color = BLACK;    

    // 'O' 그리기
    for (int i = 0; i < thickness; i++) {
        LCD_DrawRectangle(center_x - (3 * char_spacing / 2) - char_width + i,
                          center_y - char_height + i,
                          center_x - (3 * char_spacing / 2) + char_width - i,
                          center_y + char_height - i);
    }

    // 'P' 그리기
    // 세로 막대
    LCD_Fill(center_x - (char_spacing / 2) - thickness / 2,
             center_y - char_height,
             center_x - (char_spacing / 2) + thickness / 2,
             center_y + char_height,
             color);

    // 원 그리기
    for (int i = 0; i < thickness; i++) {
        LCD_DrawRectangle(center_x - (char_spacing / 2),
                          center_y - char_height + i,
                          center_x - (char_spacing / 2) + char_width,
                          center_y - char_height / 2 - i);
    }

    // 'E' 그리기
    LCD_Fill(center_x + (char_spacing / 2) - thickness / 2,
             center_y - char_height,
             center_x + (char_spacing / 2) + thickness / 2,
             center_y + char_height,
             color); // 세로줄
    LCD_Fill(center_x + (char_spacing / 2),
             center_y - char_height,
             center_x + (char_spacing / 2) + char_width,
             center_y - char_height + thickness,
             color); // 상단 가로줄
    LCD_Fill(center_x + (char_spacing / 2),
             center_y - thickness / 2,
             center_x + (char_spacing / 2) + char_width,
             center_y + thickness / 2,
             color); // 중간 가로줄
    LCD_Fill(center_x + (char_spacing / 2),
             center_y + char_height - thickness,
             center_x + (char_spacing / 2) + char_width,
             center_y + char_height,
             color); // 하단 가로줄

    // 'N' 그리기
    LCD_Fill(center_x + (3 * char_spacing / 2) - thickness / 2,
             center_y - char_height,
             center_x + (3 * char_spacing / 2) + thickness / 2,
             center_y + char_height,
             color); // 왼쪽 세로줄
    for (int i = 0; i < thickness; i++) {
        LCD_DrawLine(center_x + (3 * char_spacing / 2) + i,
                     center_y - char_height,
                     center_x + (3 * char_spacing / 2) + char_width - i,
                     center_y + char_height); // 대각선
    }
    LCD_Fill(center_x + (3 * char_spacing / 2) + char_width - thickness / 2,
             center_y - char_height,
             center_x + (3 * char_spacing / 2) + char_width + thickness / 2,
             center_y + char_height,
             color); // 오른쪽 세로줄
}

void LCD_drawClose(void) { // CLOSE 글자 출력
    LCD_Clear(WHITE);  // 화면을 흰색으로 초기화

    uint16_t center_x = 120;
    uint16_t center_y = 160;

    // 글자 간격 및 크기
    uint16_t char_spacing = 50;  // 글자 간 간격
    uint16_t char_width = 20;    // 글자 폭
    uint16_t char_height = 40;   // 글자 높이
    uint16_t thickness = 4;      // 선 두께
    uint16_t color = BLACK;      // 글자 색상

    // 'C' 그리기 (오른쪽으로 이동)
    uint16_t c_offset = 10; 
    for (int i = 0; i < thickness; i++) {
        LCD_DrawLine(center_x - (2 * char_spacing) - char_width + c_offset,
                     center_y - char_height + i,
                     center_x - (2 * char_spacing) + c_offset,
                     center_y - char_height + i);  // 상단 가로
        LCD_DrawLine(center_x - (2 * char_spacing) - char_width + c_offset,
                     center_y + char_height - i,
                     center_x - (2 * char_spacing) + c_offset,
                     center_y + char_height - i);  // 하단 가로
        LCD_DrawLine(center_x - (2 * char_spacing) - char_width + i + c_offset,
                     center_y - char_height,
                     center_x - (2 * char_spacing) - char_width + i + c_offset,
                     center_y + char_height);  // 왼쪽 세로
    }

    // 'L' 그리기
    LCD_Fill(center_x - char_spacing - thickness / 2,
             center_y - char_height,
             center_x - char_spacing + thickness / 2,
             center_y + char_height,
             color);  // 세로
    LCD_Fill(center_x - char_spacing,
             center_y + char_height - thickness,
             center_x - char_spacing + char_width,
             center_y + char_height,
             color);  // 하단 가로

    // 'O' 그리기
    for (int i = 0; i < thickness; i++) {
        LCD_DrawRectangle(center_x - char_width + i,
                          center_y - char_height + i,
                          center_x + char_width - i,
                          center_y + char_height - i);
    }

    // 'S' 그리기
    LCD_Fill(center_x + char_spacing - char_width,
            center_y - char_height,
            center_x + char_spacing,
            center_y - char_height + thickness,
            color);  // 상단 가로
    LCD_Fill(center_x + char_spacing - char_width,
            center_y - thickness / 2,
            center_x + char_spacing,
            center_y + thickness / 2,
            color);  // 중간 가로
    LCD_Fill(center_x + char_spacing - char_width,
            center_y + char_height - thickness,
            center_x + char_spacing,
            center_y + char_height,
            color);  // 하단 가로
    LCD_Fill(center_x + char_spacing - char_width,
            center_y - char_height,
            center_x + char_spacing - char_width + thickness,
            center_y - thickness / 2,
            color);  // 왼쪽 상단 세로
    LCD_Fill(center_x + char_spacing - thickness,
            center_y + thickness / 2,
            center_x + char_spacing,
            center_y + char_height,
            color);  // 오른쪽 하단 세로


    // 'E' 그리기
    LCD_Fill(center_x + (2 * char_spacing) - char_width,
             center_y - char_height + thickness,
             center_x + (2 * char_spacing) - char_width + thickness,
             center_y + char_height - thickness,
             color);  // 세로줄
    LCD_Fill(center_x + (2 * char_spacing) - char_width,
             center_y - char_height + thickness,
             center_x + (2 * char_spacing),
             center_y - char_height + (2 * thickness),
             color);  // 상단 가로줄
    LCD_Fill(center_x + (2 * char_spacing) - char_width,
             center_y - thickness / 2,
             center_x + (2 * char_spacing),
             center_y + thickness / 2,
             color);  // 중간 가로줄
    LCD_Fill(center_x + (2 * char_spacing) - char_width,
             center_y + char_height - (2 * thickness),
             center_x + (2 * char_spacing),
             center_y + char_height - thickness,
             color);  // 하단 가로줄
}