#include "stm32f10x.h"

#define RCC_APB2ENR (*(volatile unsigned int *)0x40021018)

#define GPIOA_CRL (*(volatile unsigned int *)0x40010800)
#define GPIOA_IDR (*(volatile unsigned int *)0x40010808)

#define GPIOB_CRH (*(volatile unsigned int *)0x40010C04)
#define GPIOB_IDR (*(volatile unsigned int *)0x40010C08)

#define GPIOC_CRL (*(volatile unsigned int *)0x40011000)
#define GPIOC_CRH (*(volatile unsigned int *)0x40011004)
#define GPIOC_IDR (*(volatile unsigned int *)0x40011008)
#define GPIOC_BSRR (*(volatile unsigned int *)0x40011010)

#define GPIOD_CRL (*(volatile unsigned int *)0x40011400)
#define GPIOD_BSRR (*(volatile unsigned int *)0x40011410)
#define GPIOD_BRR (*(volatile unsigned int *)0x40011414)


void delay()
{
    for (int i = 0; i < 10000000; i++);
}

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


    // clock
    // PORT A, B, C, D ON
    RCC_APB2ENR |= 0x3C;

    // Relay Module
    // PC8, PC9
    GPIOC_CRH &= 0xFFFFFF00;
    GPIOC_CRH |= 0x00000033;

    while (1)
    {
        if ((GPIOC_IDR & 0x10) == 0) // Key1
        {
            GPIOC_BSRR |= 0x300; // PC8 set, PC9 set
            delay();
            GPIOC_BSRR |= 0x3000000; // PC8 reset, PC9 reset
        }
        
        else if ((GPIOB_IDR & 0x0400) == 0) // Key2
        {
            GPIOC_BSRR |= 0x2000100; // PC8 set, PC9 reset
            delay();   
            GPIOC_BSRR |= 0x3000000; // PC8 reset, PC9 reset
        }
        
        else if ((GPIOC_IDR & 0x2000) == 0) // key 3
        {
            GPIOC_BSRR |= 0x1000200; // PC8 reset, PC9 set
            delay();
            GPIOC_BSRR |= 0x3000000; // PC8 reset, PC9 reset
        }
    }
    return 0;
}