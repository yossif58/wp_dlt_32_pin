#ifndef _UART_H
#define _UART_H

#undef PUBLIC
#ifdef UARTH
#define  PUBLIC
#else
#define  PUBLIC  extern
#endif
#define DLT_RX_TIMEOUT	3	// timeout after transmission message trough DLT protocol
#define MAX_UART_IN     255
/** User callback for character received event */
typedef void (*serial_rx_callback_f)(uint8_t ch);

typedef enum
{
    UART_FLOW_CONTROL_NONE,
    UART_FLOW_CONTROL_HW,
} uart_flow_control_e;

PUBLIC void WNT_RxParamInit(void);
PUBLIC void ST_RxParamInit(void);
PUBLIC uint32_t                 rx_buffer_idx;
PUBLIC uint8_t                  rx_buffer[MAX_UART_IN];
PUBLIC volatile bool            m_escaped;
PUBLIC volatile uint32_t        m_timeout;

PUBLIC uint8_t                  ST_rx_buffer[MAX_UART_IN];
PUBLIC uint32_t                 ST_rx_buffer_idx;
PUBLIC volatile bool            ST_m_escaped;
PUBLIC volatile uint32_t        ST_m_timeout;

PUBLIC uint32_t Usart_sendBuffer(const void * buffer, uint32_t length);
PUBLIC uint32_t Usart1_sendBuffer(const void * buffer, uint32_t length);

PUBLIC void DltRxParamInit(void);
PUBLIC void ST_RxParamInit(void);
PUBLIC void ST_uart_receive(uint8_t ch);
PUBLIC void custom_uart_init(USART_TypeDef *usart, uint32_t baud, bool flow_ctrl);
PUBLIC void custom_uart_receive(uint8_t ch);


#endif

