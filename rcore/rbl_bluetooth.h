#pragma once
#include "stm32_usart.h"

#define TX_BUFFER_SIZE 250
#define TX_TIMEOUT_MS 5

typedef struct pbl_transport_packet_t {
    uint16_t length;
    uint16_t endpoint;
    uint8_t *data;
} __attribute__((__packed__)) pbl_transport_packet;



typedef void (*tx_complete_callback)();

void bluetooth_init(void);
void bluetooth_data_rx_notify(size_t len);
uint32_t bluetooth_send_serial_raw(uint8_t *data, size_t len);
uint8_t bluetooth_send(uint8_t *data, size_t len);
void bluetooth_send_packet(uint16_t endpoint, uint8_t *data, uint16_t len);
uint32_t bluetooth_tx_buf_get_bytes(uint8_t *data, size_t len);
void bluetooth_data_rx(uint8_t *data, size_t len);
void bluetooth_tx_complete_from_isr(void);


uint8_t hw_bluetooth_power_cycle(void);
void hw_bluetooth_enable_cts_irq(void);
void hw_bluetooth_disable_cts_irq(void);
stm32_usart_t *hw_bluetooth_get_usart(void);
uint8_t hw_bluetooth_init(void);
