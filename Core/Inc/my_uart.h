
#ifndef __MY_UART_H
#define __MY_UART_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h> 

#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "my_uart.h"
#include "main.h"

/*

1) Which register enables the GPIO clock? 
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

2) Which register enables the USART2 clock?
    __HAL_RCC_USART2_CLK_ENABLE
    RCC->APB1ENR |=  RCC_APB1ENR_USART2EN

3) How do I select alternate-function mode for the TX/RX pins?
    (GPIOx_MODER)
    USART2_TX -> PA2 (5:4) bits |
                                |-----> AF7
    USART2_RX -> PA3 (7:6) bits |

    // Clear both pins' mode fields
    GPIOA->MODER &= ~((3U << 4) | (3U << 6));

    // Set both pins to alternate-function mode (binary 10)
    GPIOA->MODER |=  ((2U << 4) | (2U << 6));


4) Which alternate-function number selects USART2?
    Each pin gets 4 pins -> PA2: [11:8]
    Each pin gets 4 pins -> PA2: [15:12]
    

    // Clear the existing alternate-function selections
    GPIOA->AFR[0] &= ~((0xFU << 8) | (0xFU << 12));

    // Select AF7: PA2 = USART2_TX, PA3 = USART2_RX

    // GPIO_AF7_USART2
    GPIOA->AFR[0] |=  ((GPIO_AF7_USART2 << 8) | (GPIO_AF7_USART2 << 12));

5) What clock frequency actually feeds USART2?
    HAL_RCC_GetPCLK1Freq()

6) How is the baud-rate register calculated?
    T_x/R_x baud = f_ck / (16 * USARTDIV)
    A baud rate register (USART_BRR) - 12-bit mantissa and 4-bit fraction.

    uint32_t uart_clock_hz = HAL_RCC_GetPCLK1Freq();
    uint32_t baud = 115200U;
    USART2->BRR = (uart_clock_hz + baud / 2U) / baud;


7) Which bits enable the transmitter, receiver, and peripheral?
    USART2->CR1 |= USART_CR1_TE;
    USART2->CR1 |= USART_CR1_RE;
    USART2->CR1 |= USART_CR1_UE;

*/



/* 
    Interrupt driven 

    USART_CR1 -> USART_SR_RXNE_Msk 
    0: Data is not received 1: Received data is ready to be read.


    USART2_IRQn in NVIC




*/



void my_uart_init(void);
void my_uart_send_byte(uint8_t byte);
bool my_uart_read_byte(uint8_t* byte);
void my_uart_irq_handler(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MY_UART_H */
