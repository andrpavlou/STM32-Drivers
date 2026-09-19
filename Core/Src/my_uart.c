
/* Includes */
#include "my_uart.h"
#include "ring_buffer.h"


static RingBuffer rx_ring;
static RingBuffer tx_ring;




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

    /* No pull on TX; pull up on RX keeps a disconnected input idle high. */
    GPIOA->PUPDR &= ~((3U << 4) | (3U << 6));
    GPIOA->PUPDR |=   (1U << 6);

    /* Baud rate: 115200 + oversampling by 16. */
    uint32_t uart_clock_hz = HAL_RCC_GetPCLK1Freq();
    uint32_t baud = 115200U;

    USART2->BRR = (uart_clock_hz + baud / 2U) / baud;


    ring_buffer_init(&rx_ring);
    ring_buffer_init(&tx_ring);

    if (!atomic_is_lock_free(&rx_ring.head) ||
        !atomic_is_lock_free(&rx_ring.tail) || 
        !atomic_is_lock_free(&tx_ring.head) ||
        !atomic_is_lock_free(&tx_ring.tail)) {
        Error_Handler();
        return;
    }

    NVIC_SetPriority(USART2_IRQn, 5U);
    NVIC_ClearPendingIRQ(USART2_IRQn);
    NVIC_EnableIRQ(USART2_IRQn);

    /* Enable transmitter, receiver, interrupts. */
    USART2->CR1 |=  USART_CR1_TE | 
                    USART_CR1_RE |
                    USART_CR1_RXNEIE;
    
    /* Enable USART2*/
    USART2->CR1 |= USART_CR1_UE;
}


bool my_uart_send_byte(uint8_t byte)
{
    if (ring_buffer_push(&tx_ring,  byte) == false) {
        return false;
    }

    /* Ask the interrupt handler to start draining the TX ring. */
    USART2->CR1 |= USART_CR1_TXEIE;

    return true;
}

bool my_uart_read_byte(uint8_t *byte) 
{
    return ring_buffer_pop(&rx_ring, byte);
}


void my_uart_irq_handler(void)
{
    uint32_t status = USART2->SR;
    uint8_t byte = 0;

    const uint32_t errors = USART_SR_ORE |
                            USART_SR_FE |
                            USART_SR_NE |
                            USART_SR_PE;

    /* Handle received data or a receive error. */
    if ((status & (USART_SR_RXNE | errors)) != 0U) {
        /* Reading SR followed by DR clears receive error flags.
           Reading DR also clears RXNE. */
        byte = (uint8_t)USART2->DR;


        if (((status & USART_SR_RXNE) != 0U) &&
            ((status & errors) == 0U)) {
            (void)ring_buffer_push(&rx_ring, byte);
        }

    }

    if (((status & USART_SR_TXE) != 0U) && 
        ((USART2->CR1 & USART_CR1_TXEIE) != 0U)) {
        
        if (ring_buffer_pop(&tx_ring, &byte)) {
            USART2->DR = byte;
        } else {
            /* Nothing queued: stop transmit-empty interrupts. */
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }

    }


}