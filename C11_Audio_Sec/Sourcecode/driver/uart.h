#ifndef _UART_H
#define _UART_H

#define _ENABLE_UART0_INTERRUPT /*enable UART0 interrupt*/
#define _ENABLE_UART1_INTERRUPT /*enable UART1 interrupt*/  

extern void Uart0Send(unsigned char value);
extern void Uart1Send(unsigned char value);
extern void SetUart0Baudrate(unsigned long baudrate);

extern void  UartInit(void);

#define echo(m) Uart0Send(m)
#endif

