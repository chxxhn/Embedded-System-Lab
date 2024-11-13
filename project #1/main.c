#include "stm32f10x.h"

#define RCC_APB2ENR (*(volatile unsigned int *)0x40021018)

#define GPIOA_CRL (*(volatile unsigned int *)0x40010800)
#define GPIOB_CRH (*(volatile unsigned int *)0x40010C04)
#define GPIOC_CRL (*(volatile unsigned int *)0x40011000)
#define GPIOC_CRH (*(volatile unsigned int *)0x40011004)
#define GPIOD_CRL (*(volatile unsigned int *)0x40011400)

#define GPIOA_IDR (*(volatile unsigned int *) 0x40010808)
#define GPIOB_IDR (*(volatile unsigned int *)0x40010C08)
#define GPIOC_IDR (*(volatile unsigned int *)0x40011008)


#define GPIOD_BSRR (*(volatile unsigned int *)0x40011410)
#define GPIOD_BRR (*(volatile unsigned int *)0x40011414)

int main(void)
{

// Key 1 PC4
GPIOC_CRL &= 0xFFF0FFFF;
GPIOC_CRL |= 0x00080000;
// Key 2 PB10
GPIOB_CRH &= 0xFFFFF0FF;
GPIOB_CRH |= 0x00000800;
// Key 3 PC13
GPIOC_CRH &= 0xFF0FFFFF;
GPIOC_CRH |= 0x00800000;
// Key 4 PA0
GPIOA_CRL &= 0xFFFFFFF0;
GPIOA_CRL |= 0x00000008;

// clock
// PORT A, B, C, D ON
RCC_APB2ENR |= 0x3C;

// LED
// PD2, PD3, PD4, PD7
GPIOD_CRL &= 0x0FF000FF;
GPIOD_CRL |= 0x30033300;
GPIOD_BSRR |= 0x9C; // LED1,2,3,4 OFF

while(1){
if((GPIOC_IDR & 0x10) == 0){ //Key1
// GPIOD_BSRR |= 0xC0000; // PD2, PD3 ON
GPIOD_BRR |= 0x84; // LED1,4 ON
}
else if((GPIOB_IDR & 0x0400) == 0){ // Key2
GPIOD_BSRR |= 0x84; // LED1,4 OFF
}
else if((GPIOC_IDR & 0x2000) == 0){ //key 3
// GPIOD_BSRR |= 0x900000; // PD4, PD7 ON
GPIOD_BRR |= 0x18; // LED2,3 ON                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
}
else if((GPIOA_IDR & 0x01) == 0){ //key 4
GPIOD_BSRR |= 0x18; // LED2,3 OFF
}
}
return 0;
}