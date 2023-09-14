/* Copyright 2019 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hal_api.h"
#include "board.h"
#include "api.h"
#include "data.h"
//#include "vendor/efr32fg13/efr32fg13p_usart.h"
#define UARTH
#include "uartdlt.h"
#include "utils.h"
#include "DLTProtocol.h"
#include "led.h"
// Declare unique ring-buffer size
#include "app.h"
#include "system.h"
#include "hfperclk.h"
#define BUFFER_SIZE                     512u
#include "ringbuffer.h"

#pragma pack(1)

// Y.F. 16/11/2020 add usart1 
/* Only one USART, this is easy */
static volatile serial_rx_callback_f    m_rx_callback;
static volatile serial_rx_callback_f    ST_m_rx_callback; // Y.F. 15/11/2020


// Buffer for transmissions
static volatile ringbuffer_t            m_usart_tx_buffer;
// Buffer for transmissions FOR ST UART Y.F. 15/11/2020 
static volatile ringbuffer_t            ST_m_usart_tx_buffer;

/** Indicate if USART is enabled */
static volatile uint32_t                m_enabled;
static volatile bool                    m_tx_active;


/** Indicate if USART is enabled */
static volatile uint32_t                ST_m_enabled;
static volatile bool                    ST_m_tx_active;

static void set_baud(USART_TypeDef *usart, uint32_t baud);

void custom_uart_init(USART_TypeDef *usart, uint32_t baud, bool flow_ctrl);

/** Declare the interrupt handlers */
void __attribute__((__interrupt__))     USART_RX_IRQHandler(void);
void __attribute__((__interrupt__))     USART_TX_IRQHandler(void);
/** Declare the interrupt handlers */
void __attribute__((__interrupt__))     USART1_RX_IRQHandler(void);
void __attribute__((__interrupt__))     USART1_TX_IRQHandler(void);

void Usart_init(USART_TypeDef *usart,uint32_t baudrate, uart_flow_control_e flow_control)
{
    (void)flow_control;
    blink_func();

    //    /* Enable clocks */
    CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;
 
    if ( usart == USART0)
    {
        //uart_tx_pin
        hal_gpio_set_mode(BOARD_USART_GPIO_PORT,
                          BOARD_USART_TX_PIN,
                          GPIO_MODE_DISABLED);
        hal_gpio_clear(BOARD_USART_GPIO_PORT, BOARD_USART_TX_PIN);
        //uart_rx_pin
        hal_gpio_set_mode(BOARD_USART_GPIO_PORT,
                          BOARD_USART_RX_PIN,
                          GPIO_MODE_DISABLED);
        hal_gpio_clear(BOARD_USART_GPIO_PORT, BOARD_USART_RX_PIN);

        /* Module variables */
        Ringbuffer_reset(m_usart_tx_buffer);
        m_rx_callback = NULL;
        m_tx_active = false;
        m_enabled = 0;

        /* Disable for RX */
        Sys_disableAppIrq(BOARD_UART_RX_IRQn);
        Sys_clearFastAppIrq(BOARD_UART_RX_IRQn);
        /* Disable for TX */
        Sys_disableAppIrq(BOARD_UART_TX_IRQn);
        Sys_clearFastAppIrq(BOARD_UART_TX_IRQn);
           // Must enable clock for configuration period
        enableHFPERCLK(usart,true);   
        /* Set UART output pins */
        USART0->ROUTEPEN = USART_ROUTEPEN_RXPEN |
                                USART_ROUTEPEN_TXPEN;

        /* Set UART route */
        USART0->ROUTELOC0 = BOARD_USART_ROUTELOC_RXLOC |
                                 BOARD_USART_ROUTELOC_TXLOC;
       

    }
     else if ( usart == USART1)   
     {
         //uart_tx_pin
        hal_gpio_set_mode(ST_USART_GPIO_PORT,
                          ST_USART_TX_PIN,
                          GPIO_MODE_DISABLED);
        hal_gpio_clear(ST_USART_GPIO_PORT, ST_USART_TX_PIN);
        //uart_rx_pin
        hal_gpio_set_mode(ST_USART_GPIO_PORT,
                          ST_USART_RX_PIN,
                          GPIO_MODE_DISABLED);
        hal_gpio_clear(ST_USART_GPIO_PORT, ST_USART_RX_PIN);

        /* Module variables */
        Ringbuffer_reset(ST_m_usart_tx_buffer);
        ST_m_rx_callback = NULL;
        ST_m_tx_active = false;
        ST_m_enabled = 0;

        /* Disable for RX */
        Sys_disableAppIrq(ST_UART_RX_IRQn);
        Sys_clearFastAppIrq(ST_UART_RX_IRQn);
        /* Disable for TX */
        Sys_disableAppIrq(ST_UART_TX_IRQn);
        Sys_clearFastAppIrq(ST_UART_TX_IRQn);     
           // Must enable clock for configuration period
        enableHFPERCLK(usart,true);   
        /* Set UART output pins */
        USART1->ROUTEPEN = USART_ROUTEPEN_RXPEN |
                                USART_ROUTEPEN_TXPEN;

        /* Set UART route */
        USART1->ROUTELOC0 = ST_USART_ROUTELOC_RXLOC |
                                 ST_USART_ROUTELOC_TXLOC;

       

     }
      
       
     
        /* Initialize UART for asynch mode with baudrate baud */
        /* Disable transceiver */
        usart->CMD = 0;
        usart->CTRL = 0;
        usart->I2SCTRL = 0;
        /* Disables PRS */
        usart->INPUT = 0;
        /* Set frame params: 8bit, nopar, 1stop */
        
        
        if ( usart == USART0)   
        {
          /* Set frame params: 8bit, nopar, 1stop */
          usart->FRAME = USART_FRAME_DATABITS_EIGHT |
                               USART_FRAME_PARITY_EVEN |  // y.f. 23/04/2020 dlt parity even
                               USART_FRAME_STOPBITS_ONE;
      
        
        }
        else
        { // Y.F. S8/4/2021 ST->SILICON PARITY NONE 
          /* Set frame params: 8bit, nopar, 1stop */
          usart->FRAME = USART_FRAME_DATABITS_EIGHT |
          USART_FRAME_PARITY_NONE | 
          USART_FRAME_STOPBITS_ONE;
         
        }
        set_baud(usart,baudrate);
        /* Disable all interrupt sources */
        usart->IEN = 0;
        /* Clear all irq flags */
        usart->IFC = _USART_IFC_MASK;
        /* Enable transmitter */
        usart->CMD = USART_CMD_TXEN;
        if ( usart == USART0)
        {
          /* APP IRQ */
          Sys_clearFastAppIrq(BOARD_UART_TX_IRQn);
          Sys_enableFastAppIrq(BOARD_UART_TX_IRQn,
                               APP_LIB_SYSTEM_IRQ_PRIO_HI,
                               USART_TX_IRQHandler);
        }
        else if ( usart == USART1) 
        {
          /* APP IRQ */
          Sys_clearFastAppIrq(ST_UART_TX_IRQn);
          Sys_enableFastAppIrq(ST_UART_TX_IRQn,
                               APP_LIB_SYSTEM_IRQ_PRIO_HI,
                               USART1_TX_IRQHandler);
        }
        // Configuration done: disable clock
       enableHFPERCLK(usart,false);
}

void Usart_setEnabled(bool enabled)
{
    Sys_enterCriticalSection();
    if(enabled)
    {
        /* Detect if someone is enabling UART but not disabling it ever */
        if(m_enabled == 0)
        {
            // Enable clock
            enableHFPERCLK(USART0,true);
            // Disable deep sleep
            DS_Disable(DS_SOURCE_USART);
            // Set output
            hal_gpio_set_mode(BOARD_USART_GPIO_PORT,
                              BOARD_USART_TX_PIN,
                              GPIO_MODE_OUT_PP);
        }
        m_enabled++;
    }
    else
    {
        if (m_enabled > 0)
        {
            m_enabled--;
        }
        if(m_enabled == 0)
        {
            // Set light pullup
            hal_gpio_set_mode(BOARD_USART_GPIO_PORT,
                              BOARD_USART_TX_PIN,
                              GPIO_MODE_IN_PULL);
            hal_gpio_set(BOARD_USART_GPIO_PORT,
                         BOARD_USART_TX_PIN);
            // Enable deep sleep
            DS_Enable(DS_SOURCE_USART);
            // Disable clock
            enableHFPERCLK(USART0,false);
        }
    }
    Sys_exitCriticalSection();
}
void Usart1_setEnabled(bool enabled)
{
    Sys_enterCriticalSection();
    if(enabled)
    {
        /* Detect if someone is enabling UART but not disabling it ever */
        if(ST_m_enabled == 0)
        {
            // Enable clock
            enableHFPERCLK(USART1,true);
            // Disable deep sleep
            DS_Disable(DS_SOURCE_USART);
            // Set output
            hal_gpio_set_mode(ST_USART_GPIO_PORT,
                              ST_USART_TX_PIN,
                              GPIO_MODE_OUT_PP);
        }
        ST_m_enabled++;
    }
    else
    {
        if (ST_m_enabled > 0)
        {
            ST_m_enabled--;
        }
        if(ST_m_enabled == 0)
        {
            // Set light pullup
            hal_gpio_set_mode(ST_USART_GPIO_PORT,
                              ST_USART_TX_PIN,
                              GPIO_MODE_IN_PULL);
            hal_gpio_set(ST_USART_GPIO_PORT,
                         ST_USART_TX_PIN);
            // Enable deep sleep
            DS_Enable(DS_SOURCE_USART);
            // Disable clock
            enableHFPERCLK(USART1,false);
        }
    }
    Sys_exitCriticalSection();
}

void Usart_receiverOn(USART_TypeDef *usart)
{
    BITBAND_Peripheral(&(usart->CMD), _USART_CMD_RXEN_SHIFT, 1);
}

void Usart_receiverOff(USART_TypeDef *usart)
{
    BITBAND_Peripheral(&(usart->CMD), _USART_CMD_RXDIS_SHIFT, 1);
}

bool Usart_setFlowControl(uart_flow_control_e flow)
{
    (void)flow;
    return false;
}

uint32_t Usart_sendBuffer(const void * buffer, uint32_t length)
{
    bool empty = false;
    uint32_t size_in = length;
    uint8_t * data_out = (uint8_t *)buffer;
    Sys_enterCriticalSection();
    if (Ringbuffer_free(m_usart_tx_buffer) < length)
    {
        size_in = 0;
        goto buffer_too_large;
    }
    empty = (Ringbuffer_usage(m_usart_tx_buffer) == 0);
    while(length--)
    {
        Ringbuffer_getHeadByte(m_usart_tx_buffer) = *data_out++;
        Ringbuffer_incrHead(m_usart_tx_buffer, 1);
    }
    if (empty)
    {
        Usart_setEnabled(true);
        BITBAND_Peripheral(&(BOARD_USART->IEN), _USART_IEN_TXC_SHIFT, 1);
        BOARD_USART->TXDATA = Ringbuffer_getTailByte(m_usart_tx_buffer);
        m_tx_active = true;
    }
buffer_too_large:
      
    Sys_exitCriticalSection();
    return size_in;
}



uint32_t Usart1_sendBuffer(const void * buffer, uint32_t length)
{
    bool empty = false;
    uint32_t size_in = length;
    uint8_t * data_out = (uint8_t *)buffer;
    Sys_enterCriticalSection();
    if (Ringbuffer_free(ST_m_usart_tx_buffer) < length)
    {
        size_in = 0;
        goto buffer_too_large;
    }
    empty = (Ringbuffer_usage(ST_m_usart_tx_buffer) == 0);
    while(length--)
    {
        Ringbuffer_getHeadByte(ST_m_usart_tx_buffer) = *data_out++;
        Ringbuffer_incrHead(ST_m_usart_tx_buffer, 1);
    }
    if (empty)
    {
        Usart1_setEnabled(true);
        BITBAND_Peripheral(&(USART1->IEN), _USART_IEN_TXC_SHIFT, 1);
        USART1->TXDATA = Ringbuffer_getTailByte(ST_m_usart_tx_buffer);
        ST_m_tx_active = true;
    }
buffer_too_large:
    Sys_exitCriticalSection();
    return size_in;
}

void Usart_enableReceiver(serial_rx_callback_f rx_callback)
{
    uint32_t __attribute__((unused))dummy;
    Sys_enterCriticalSection();
    /* Set callback */
    m_rx_callback = rx_callback;
    // Enable clock
    enableHFPERCLK(USART0,true);
    if(rx_callback)
    {
        Sys_enableFastAppIrq(BOARD_UART_RX_IRQn,
                             APP_LIB_SYSTEM_IRQ_PRIO_HI,
                             USART_RX_IRQHandler);
        BITBAND_Peripheral(&(BOARD_USART->IEN), _USART_IEN_RXDATAV_SHIFT, 1);
        // Set light pull-up resistor
        hal_gpio_set_mode(BOARD_USART_GPIO_PORT,
                          BOARD_USART_RX_PIN,
                          GPIO_MODE_IN_PULL);
        hal_gpio_set(BOARD_USART_GPIO_PORT,
                     BOARD_USART_RX_PIN);

    }
    else
    {
        Sys_disableAppIrq(BOARD_UART_RX_IRQn);
        BITBAND_Peripheral(&(BOARD_USART->IEN), _USART_IEN_RXDATAV_SHIFT, 0);
        hal_gpio_set_mode(BOARD_USART_GPIO_PORT,
                          BOARD_USART_RX_PIN,
                          GPIO_MODE_DISABLED);
        // Disable pull-up for disabled GPIO:s
        hal_gpio_clear(BOARD_USART_GPIO_PORT,
                       BOARD_USART_RX_PIN);
    }
    /* Clear all interrupts */
    dummy = BOARD_USART->RXDATA;
    BOARD_USART->IFC = USART_IFC_RXFULL;
    // Disable clock
    enableHFPERCLK(USART0,false);
    Sys_clearFastAppIrq(BOARD_UART_RX_IRQn);
    Sys_exitCriticalSection();
}
void Usart1_enableReceiver(serial_rx_callback_f rx_callback)
{
    uint32_t __attribute__((unused))dummy;
    Sys_enterCriticalSection();
    /* Set callback */
    ST_m_rx_callback = rx_callback;
    // Enable clock
    enableHFPERCLK(USART1,true);
    if(rx_callback)
    {
        Sys_enableFastAppIrq(ST_UART_RX_IRQn,
                             APP_LIB_SYSTEM_IRQ_PRIO_HI,
                             USART1_RX_IRQHandler);
        BITBAND_Peripheral(&(USART1->IEN), _USART_IEN_RXDATAV_SHIFT, 1);
        // Set light pull-up resistor
        hal_gpio_set_mode(ST_USART_GPIO_PORT,
                          ST_USART_RX_PIN,
                          GPIO_MODE_IN_PULL);
        hal_gpio_set(ST_USART_GPIO_PORT,
                     ST_USART_RX_PIN);
    }
    else
    {
        Sys_disableAppIrq(USART1_RX_IRQn);
        BITBAND_Peripheral(&(USART1->IEN), _USART_IEN_RXDATAV_SHIFT, 0);
        hal_gpio_set_mode(ST_USART_GPIO_PORT,
                          ST_USART_RX_PIN,
                          GPIO_MODE_DISABLED);
        // Disable pull-up for disabled GPIO:s
        hal_gpio_clear(ST_USART_GPIO_PORT,
                       ST_USART_RX_PIN);
    }
    /* Clear all interrupts */
    dummy = USART1->RXDATA;
    USART1->IFC = USART_IFC_RXFULL;
    // Disable clock
    enableHFPERCLK(USART1,false);
    Sys_clearFastAppIrq(USART1_RX_IRQn);
    Sys_exitCriticalSection();
}
void __attribute__((__interrupt__)) USART_RX_IRQHandler(void)
{
//    DBG_ENTER_IRQ_USART();
    /* Data received */
    uint16_t ch = BOARD_USART->RXDATA;
    /* RXFULL must be explicitly cleared */
    BOARD_USART->IFC = USART_IFC_RXFULL;
    if (m_rx_callback != NULL)
    {
        m_rx_callback((uint8_t) (ch));
    }
//    DBG_EXIT_IRQ_USART();
}
void __attribute__((__interrupt__)) USART1_RX_IRQHandler(void)
{
//    DBG_ENTER_IRQ_USART();
    /* Data received */
    uint16_t ch = USART1->RXDATA;
    /* RXFULL must be explicitly cleared */
    USART1->IFC = USART_IFC_RXFULL;
    if (ST_m_rx_callback != NULL)
    {
        ST_m_rx_callback((uint8_t) (ch));
    }
//    DBG_EXIT_IRQ_USART();
}

void __attribute__((__interrupt__)) USART_TX_IRQHandler(void)
{
//    DBG_ENTER_IRQ_USART();
    BOARD_USART->IFC = _USART_IFC_TXC_MASK;
    /* byte has been sent -> move tail */
    Ringbuffer_incrTail(m_usart_tx_buffer, 1);
    if (Ringbuffer_usage(m_usart_tx_buffer) != 0)
    {
        BOARD_USART->TXDATA = Ringbuffer_getTailByte(m_usart_tx_buffer);
    }
    else
    {
        /* when buffer becomes empty, reset indexes */
        BITBAND_Peripheral(&(BOARD_USART->IEN), _USART_IEN_TXC_SHIFT, 0);
        Usart_setEnabled(false);
        m_tx_active = false;
    }
//    DBG_EXIT_IRQ_USART();
}


void __attribute__((__interrupt__)) USART1_TX_IRQHandler(void)
{
//    DBG_ENTER_IRQ_USART();
    USART1->IFC = _USART_IFC_TXC_MASK;
    /* byte has been sent -> move tail */
    Ringbuffer_incrTail(ST_m_usart_tx_buffer, 1);
    if (Ringbuffer_usage(ST_m_usart_tx_buffer) != 0)
    {
        USART1->TXDATA = Ringbuffer_getTailByte(ST_m_usart_tx_buffer);
    }
    else
    {
        /* when buffer becomes empty, reset indexes */
        BITBAND_Peripheral(&(USART1->IEN), _USART_IEN_TXC_SHIFT, 0);
        Usart1_setEnabled(false);
        ST_m_tx_active = false;
    }
//    DBG_EXIT_IRQ_USART();
}
static void set_baud(USART_TypeDef *usart, uint32_t baud)
{
    volatile uint32_t baud_gen;
    /* Calculate baudrate: see em_usart.c in emlib for reference */
    baud_gen = 32 * HFPERCLK_FREQ + (4 * baud) / 2;
    baud_gen /= (4 * baud);
    baud_gen -= 32;
    baud_gen *= 8;
    baud_gen &= _USART_CLKDIV_DIV_MASK;

    /* Set oversampling bit (8) */
    usart->CTRL  &= ~_USART_CTRL_OVS_MASK;
    usart->CTRL  |= USART_CTRL_OVS_X4;
    usart->CLKDIV = baud_gen;
}

void    DltRxParamInit(void)
{
            m_timeout= 0;
            m_escaped=false;
            rx_buffer_idx =0;
            DltRxTimeout=0;
}

void    ST_RxParamInit(void)
{
            ST_m_timeout= 0;
            ST_m_escaped=false;
            ST_rx_buffer_idx =0;
            ST_RxTimeout=0;
}

void WNT_RxParamInit(void)
{
  // y.f. future used 15/11/2020
}

void custom_uart_init(USART_TypeDef *usart, uint32_t baud, bool flow_ctrl)
{
    uart_flow_control_e flow;
    if (usart == USART0)
      DltRxParamInit();
    else
      if (usart == USART1)
       ST_RxParamInit();  
    else
        if (usart == USART2)
        WNT_RxParamInit();
    flow_ctrl ? (flow = UART_FLOW_CONTROL_HW) :
                (flow = UART_FLOW_CONTROL_NONE);
    Usart_init(usart,baud, flow);
    if (usart == USART0)
    {
       Usart_enableReceiver(custom_uart_receive);
       Usart_setEnabled(true);
    }
    else if (usart == USART1)
    {
       Usart1_enableReceiver(ST_uart_receive);
       Usart1_setEnabled(true);
    }
    Usart_receiverOn(usart);
   
}

void custom_uart_receive(uint8_t ch)
{
      rx_buffer[rx_buffer_idx] = ch;  
      rx_buffer_idx =( rx_buffer_idx+1) % MAX_UART_IN;
      m_escaped = true;  // start timeout for end of message timeout of 20ms
      m_timeout = 0;
      
  
}
void ST_uart_receive(uint8_t ch)
{
      ST_rx_buffer[ST_rx_buffer_idx] = ch;  
      ST_rx_buffer_idx =( ST_rx_buffer_idx+1) % MAX_UART_IN;
      ST_m_escaped = true;  // start timeout for end of message timeout of 20ms
      ST_m_timeout = 0;
  
}













////////////////////////////////////////////////////////////////////////////////////////