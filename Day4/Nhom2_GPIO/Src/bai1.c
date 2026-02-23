#include "stm32f401re.h"
#include "stm32f401re_gpio.h"
#include "stm32f401re_rcc.h"

/* ===================== MACRO ===================== */
#define LED_PORT        GPIOA
#define LED_PIN         GPIO_Pin_5

#define BUTTON_PORT     GPIOC
#define BUTTON_PIN      GPIO_Pin_13

/* ===================== PROTOTYPE ===================== */
void GPIO_LED_Init(void);
void GPIO_Button_Init(void);
void LED_On(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void LED_Off(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t Button_Read(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

/* ===================== MAIN ===================== */
int main(void)
{
    GPIO_LED_Init();
    GPIO_Button_Init();

    while (1)
    {
        /* Nhấn nút PC13 → LED sáng */
        if (Button_Read(BUTTON_PORT, BUTTON_PIN) == 0)
        {
            LED_On(LED_PORT, LED_PIN);
        }
        else
        {
            LED_Off(LED_PORT, LED_PIN);
        }
    }
}

/* ===================== LED INIT ===================== */
void GPIO_LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = LED_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

    GPIO_Init(LED_PORT, &GPIO_InitStructure);
}

/* ===================== BUTTON INIT ===================== */
void GPIO_Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = BUTTON_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

    GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);
}

/* ===================== LED CONTROL ===================== */
void LED_On(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BSRRL = GPIO_Pin;
}

void LED_Off(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BSRRH = GPIO_Pin;
}

/* ===================== BUTTON READ ===================== */
uint8_t Button_Read(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    return (uint8_t)((GPIOx->IDR >> __builtin_ctz(GPIO_Pin)) & 0x01);
}
