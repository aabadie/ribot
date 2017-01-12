/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @brief Uart to uart communication program
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "periph_conf.h"
#include "msg.h"
#include "thread.h"
#include "ringbuffer.h"
#include "periph/uart.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG                (1)
#include "debug.h"

/* UART communication */
#define PC_UART                     UART_DEV(0)
#define BT_UART                     UART_DEV(1)
#define BAUDRATE                    (115200U)
#define UART_BUFSIZE                (128U)

/* PWM */
#define MODE                        PWM_LEFT
#define FREQU                       (1000U)
#define STEPS                       (256U)

/* DIRECTION GPIO */
#define RIGHT_DIRECTION_PIN         GPIO_PIN(PORT_B, 5)
#define LEFT_DIRECTION_PIN          GPIO_PIN(PORT_A, 8)

typedef struct {
    char rx_mem[UART_BUFSIZE];
    ringbuffer_t rx_buf;
} uart_ctx_t;
static uart_ctx_t ctx;

static kernel_pid_t main_thread_pid;

static char message[64] = { 0 };

static void pc_rx_cb(void *uart, uint8_t c) {}

static void bt_rx_cb(void *dev, uint8_t data)
{
    ringbuffer_add_one(&(ctx.rx_buf), data);
    if (data == '\n') {
        msg_t msg;
        msg.content.value = (uint32_t)dev;
        msg_send(&msg, main_thread_pid);
    }
}

static void parse_message(int16_t * right_speed_ratio, int16_t * left_speed_ratio, int16_t * crc)
{
    char * strtokIndx;
  
    /* Left speed ratio */
    strtokIndx = strtok(message, ":");
    *left_speed_ratio = atoi(strtokIndx);
  
    /* Right speed ratio */
    strtokIndx = strtok(NULL, ":");
    *right_speed_ratio = atoi(strtokIndx);
  
    /* CRC */
    strtokIndx = strtok(NULL, "\n");
    *crc = atoi(strtokIndx);
    
#ifdef ENABLE_DEBUG
    DEBUG("Right speed ratio: %i\n", (int)*right_speed_ratio);
    DEBUG("Left speed ratio : %i\n", (int)*left_speed_ratio);
    DEBUG("CRC              : %i\n", (int)*crc);
#endif
}

static void update_control(int16_t right_speed_ratio, int16_t left_speed_ratio)
{
    if (right_speed_ratio < -255) {
        right_speed_ratio = -255;
    }
    if (right_speed_ratio > 255) {
        right_speed_ratio = 255;
    }
    if (left_speed_ratio < -255) {
        left_speed_ratio = -255;
    }
    if (left_speed_ratio > 255) {
        left_speed_ratio = 255;
    }
    
    uint8_t right_dir = right_speed_ratio > 0 ? 1 : 0;
    uint8_t left_dir = left_speed_ratio > 0 ? 0 : 1;;
    
    if (right_speed_ratio < 0) {
        right_speed_ratio *= -1;
    }
    if (left_speed_ratio < 0) {
        left_speed_ratio *= -1;
    }
    uint8_t right_speed = (uint8_t)right_speed_ratio;
    uint8_t left_speed = (uint8_t)left_speed_ratio;
    
#ifdef ENABLE_DEBUG
    DEBUG("Right speed    : %d\n", right_speed);
    DEBUG("Right direction: %d\n", right_dir);
    DEBUG("Left speed     : %d\n", left_speed);
    DEBUG("Left direction : %d\n", left_dir);
#endif

    gpio_write(RIGHT_DIRECTION_PIN, right_dir);
    gpio_write(LEFT_DIRECTION_PIN, left_dir);
    pwm_set(PWM_DEV(1), 0, right_speed); // D5
    pwm_set(PWM_DEV(0), 2, left_speed);  // D6
}

int main(void)
{
    printf("RiBot Control - Powered by RIOT\n");
    printf("===============================\n");

    if (UART_NUMOF < 2) {
        printf("[Error] Not enough available UART devices (%d), 2 are required.\n",
               UART_NUMOF);
        return 1;
    }
    
    /* Initialize serial devices */
    /* UART0 : PC terminal */
    uart_init(PC_UART, BAUDRATE, pc_rx_cb, (void *)PC_UART);
	
    /* UART2 : BT terminal */
    uart_init(BT_UART, BAUDRATE, bt_rx_cb, (void *)BT_UART);
    
    /* initialize ringbuffer */
    ringbuffer_init(&(ctx.rx_buf), ctx.rx_mem, UART_BUFSIZE);

    /* PWM initialization */
    for (unsigned i = 0; i < PWM_NUMOF; i++) {
        if (pwm_init(PWM_DEV(i), MODE, FREQU, STEPS) == 0) {
            printf("Error initializing PWM_%d\n", i);
            return 1;
        }
    }
    
    /* Direction GPIO */
    if (gpio_init(RIGHT_DIRECTION_PIN, GPIO_OD_PU) < 0) {
        printf("Error to initialize right direction pin\n");
        return 1;
    }
    if (gpio_init(LEFT_DIRECTION_PIN, GPIO_OD_PU) < 0) {
        printf("Error to initialize left direction pin\n");
        return 1;
    }
    
    /* Get main thread pid */
    main_thread_pid = thread_getpid();
  
    /* Local variable containing the character received on the bt uart */
    msg_t msg;
    for (;;) {
        /* Next line blocks the loop until a message is received on the idle 
       thread */
        msg_receive(&msg);
        char c;
        uint8_t pos = 0;
        printf("Command received: ");
        do {
            c = (int)ringbuffer_get_one(&(ctx.rx_buf));
            message[pos] = c;
            pos++;
        } while (c != '\n');
        
        pos = 0;
#ifdef ENABLE_DEBUG
        DEBUG("%s\n", message);
#endif
        
        int16_t right_speed_ratio, left_speed_ratio, crc;
        parse_message(&right_speed_ratio, &left_speed_ratio, &crc);
        
        if (crc == (left_speed_ratio + right_speed_ratio)) {
#ifdef ENABLE_DEBUG
            DEBUG("Update control\n");
#endif
            update_control(right_speed_ratio, left_speed_ratio);
        }
        
        ringbuffer_remove(&(ctx.rx_buf), sizeof(message));
    }
    
    return 0;
}
