
/* Includes */
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "my_uart.h"


void my_uart_init(void) 
{
    /* Enable GPIOA and USART2 clocks, then read back for startup delay. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;

    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    (void)RCC->APB1ENR;

    /* Initial setup: USART2 disabled, interrupts disabled.
       8 data bits, no parity, oversampling by 16. */
    USART2->CR1 = 0U;

    /* One stop bit, asynchronous mode. */
    USART2->CR2 = 0U;

    /* No hardware flow control or DMA. */
    USART2->CR3 = 0U;

    /* PA2 (TX), PA3 (RX): alternate-function mode. */
    GPIOA->MODER &= ~((3U << 4) | (3U << 6));
    GPIOA->MODER |=  ((2U << 4) | (2U << 6));

    /* AF7 selects USART2 on PA2 and PA3. */
    GPIOA->AFR[0] &= ~((0xFU << 8) | (0xFU << 12));
    GPIOA->AFR[0] |=  ((7U << 8) | (7U << 12));

    /* Push-pull output type. */
    GPIOA->OTYPER &= ~((1U << 2) | (1U << 3));

    /* Medium GPIO output speed. */
    GPIOA->OSPEEDR &= ~((3U << 4) | (3U << 6));
    GPIOA->OSPEEDR |=  ((1U << 4) | (1U << 6));

    /* No pull on TX; pull-up on RX keeps a disconnected input idle-high. */
    GPIOA->PUPDR &= ~((3U << 4) | (3U << 6));
    GPIOA->PUPDR |=   (1U << 6);

    /* Baud rate: 115200, with oversampling by 16. */
    uint32_t uart_clock_hz = HAL_RCC_GetPCLK1Freq();
    uint32_t baud = 115200U;

    USART2->BRR = (uart_clock_hz + baud / 2U) / baud;

    /* Enable transmitter and receiver, then USART2. */
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;
}


void my_uart_send_byte(uint8_t byte)
{

    while ((USART2->SR & USART_SR_TXE_Msk) == 0U) {

    }

    USART2->DR = byte;
}

uint8_t my_uart_read_byte(void) 
{

    while ((USART2->SR & USART_SR_RXNE_Msk) == 0U) {

    }

    return (uint8_t)USART2->DR;
}