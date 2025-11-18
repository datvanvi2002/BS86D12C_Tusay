#ifndef UART_H
#define UART_H

#include "bs86d12c.h"
#include <stdint.h>
/// @brief Initialize UART0 for TX only on pin PD7 (remapped pin). Configured for 9600 baud, 8N1. Enable transmitter only.
/// @param  None
void uart0_init(void);

/// @brief  Transmit 1 byte via UART0 with pin PD7
/// @param data 1 byte
void uart0_send_byte(unsigned char data);

/// @brief Transmit 1 string via UART0 PD7 pin
/// @param s String to transmit
void uart0_send_string(const char *s);

/// @brief Transmit an unsigned integer number (0..65535) via UART0 PD7 pin
/// @param x Number to transmit
void uart0_send_number(uint32_t x);
#endif // UART_H