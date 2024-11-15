#include "stm32f10x.h"
#include "stm32f10x_exti.h"	 
#include "stm32f10x_gpio.h"	
#include "stm32f10x_usart.h" 
#include "stm32f10x_rcc.h"	

#include "misc.h"			

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void USART1_Init(void);
void NVIC_Configure(void);

void EXTI15_10_IRQHandler(void);

void Delay(void);

void sendDataUART1(uint16_t data);

int wave = 1;

void RCC_Configure(void)
{
    // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // KEY1(PC4)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // KEY2(PB10)

	/* UART TX/RX port clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    /* Button S1, S2, S3 port clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* LED port clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}


void GPIO_Configure(void)
{
	GPIO_InitTypeDef KEY1_3_InitStructure;					  
	KEY1_3_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_13; 
	KEY1_3_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			  
	GPIO_Init(GPIOC, &KEY1_3_InitStructure);				

	GPIO_InitTypeDef KEY2_InitStructure;		 
	KEY2_InitStructure.GPIO_Pin = GPIO_Pin_10;	  
	KEY2_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_Init(GPIOB, &KEY2_InitStructure);		 

	GPIO_InitTypeDef LED_InitStructure;												
	LED_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7; 
	LED_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;								
	LED_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;									
	GPIO_Init(GPIOD, &LED_InitStructure);									

	/* UART 핀 설정 */
	// TX 핀 (PA9)을 위한 GPIO 설정
	GPIO_InitTypeDef TX_InitStructure;				
	TX_InitStructure.GPIO_Pin = GPIO_Pin_9;			
	TX_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	TX_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOA, &TX_InitStructure);			

	// RX 핀 (PA10)을 위한 GPIO 설정
	GPIO_InitTypeDef RX_InitStructure;				
	RX_InitStructure.GPIO_Pin = GPIO_Pin_10;		
	RX_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	RX_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		
	GPIO_Init(GPIOA, &RX_InitStructure);			
}

// EXTI 설정 함수
void EXTI_Configure(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	/* Button S1 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Button S2 */
	EXTI_InitTypeDef Key2_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
	Key2_InitStructure.EXTI_Line = EXTI_Line10;
	Key2_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	Key2_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	Key2_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&Key2_InitStructure);

	/* Button S3 */
	EXTI_InitTypeDef Key3_InitStructure;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);
	Key3_InitStructure.EXTI_Line = EXTI_Line13;
	Key3_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	Key3_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	Key3_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&Key3_InitStructure);
}

void USART1_Init(void)
{
	USART_InitTypeDef USART1_InitStructure;

	USART_Cmd(USART1, ENABLE);

	USART1_InitStructure.USART_BaudRate = 9600;										
	USART1_InitStructure.USART_WordLength = USART_WordLength_8b;					
	USART1_InitStructure.USART_StopBits = USART_StopBits_1;							 
	USART1_InitStructure.USART_Parity = USART_Parity_No;							
	USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				 
	USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_Init(USART1, &USART1_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void NVIC_Configure(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitTypeDef KEY1_InitStructure;
	KEY1_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	KEY1_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	KEY1_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	KEY1_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&KEY1_InitStructure);

	NVIC_InitTypeDef KEY2_InitStructure;
	KEY2_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	KEY2_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	KEY2_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	KEY2_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&KEY2_InitStructure);

	NVIC_InitTypeDef KEY3_InitStructure;
	KEY3_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	KEY3_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	KEY3_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	KEY3_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&KEY3_InitStructure);

	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void USART1_IRQHandler()
{
	uint16_t word;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		word = USART_ReceiveData(USART1);

		if (word == 'a')
		{
			sendDataUART1('a');
			wave = 1;
		}
		else if (word == 'b')
		{
			sendDataUART1('b');
			wave = -1;
		}

		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void EXTI15_10_IRQHandler(void)
{

	if (EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == Bit_RESET)
		{
			wave = -1; 
		}
		EXTI_ClearITPendingBit(EXTI_Line10); 
	}

	if (EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == Bit_RESET)
		{
			char msg[] = "Team02\r\n";
			for (int i = 0; i < sizeof(msg); i++)
			{
				sendDataUART1(msg[i]); 
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line13); 
	}
}

void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == Bit_RESET)
		{
			wave = 1; 
		}
		EXTI_ClearITPendingBit(EXTI_Line4); 
	}
}
// TODO: Create Joystick interrupt handler functions

void Delay(void)
{
	int i;

	for (i = 0; i < 2000000; i++){}
}


void sendDataUART1(uint16_t data)
{
    /* Wait till TC is set */
	while ((USART1->SR & USART_SR_TC) == 0);
	USART_SendData(USART1, data);
}

int main(void)
{
	SystemInit(); 

	RCC_Configure(); 

	GPIO_Configure(); 

	EXTI_Configure(); 

	USART1_Init(); 

	NVIC_Configure(); 

	//uint16_t led[] = {GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_7}; 

	int i = 1;
	while (1)
	{
        // TODO: implement 
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
        GPIO_ResetBits(GPIOD,GPIO_Pin_3);
        GPIO_ResetBits(GPIOD,GPIO_Pin_4);
        GPIO_ResetBits(GPIOD,GPIO_Pin_7);

		i += wave;	 
		i = (i + 4) % 4; 

		//GPIO_ResetBits(GPIOD, led[i]);

		if (i == 0) {
			GPIOD->BSRR = ~GPIO_Pin_2;
		}
		if (i == 1) {
			GPIOD->BSRR = ~GPIO_Pin_3;
		}
		if (i == 2) {
			GPIOD->BSRR = ~GPIO_Pin_4;
		}
		if (i == 3) {
			GPIOD->BSRR = ~GPIO_Pin_7;
		}

		Delay();						 
	}

	return 0;
}
