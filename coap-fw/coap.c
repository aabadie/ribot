/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <coap.h>

#include "board.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

#include "debug.h"
#define ENABLE_DEBUG (1)

#define SPEED_FACTOR 0.8
#define ANGLE_FACTOR 0.5

#define MAX_RESPONSE_LEN 500
static uint8_t response[MAX_RESPONSE_LEN] = { 0 };

static char message[64] = { 0 };
static char status[10] = { 0 };

extern void _send_coap_post(uint8_t* uri_path, uint8_t *data);

static int handle_get_well_known_core(coap_rw_buffer_t *scratch,
                                      const coap_packet_t *inpkt,
                                      coap_packet_t *outpkt,
                                      uint8_t id_hi, uint8_t id_lo);

static int handle_get_board(coap_rw_buffer_t *scratch,
                            const coap_packet_t *inpkt,
                            coap_packet_t *outpkt,
                            uint8_t id_hi, uint8_t id_lo);

static int handle_get_mcu(coap_rw_buffer_t *scratch,
                          const coap_packet_t *inpkt,
                          coap_packet_t *outpkt,
                          uint8_t id_hi, uint8_t id_lo);

static int handle_get_ribot(coap_rw_buffer_t *scratch,
                            const coap_packet_t *inpkt,
                            coap_packet_t *outpkt,
                            uint8_t id_hi, uint8_t id_lo);

static int handle_put_ribot(coap_rw_buffer_t *scratch,
                            const coap_packet_t *inpkt,
                            coap_packet_t *outpkt,
                            uint8_t id_hi, uint8_t id_lo);

static const coap_endpoint_path_t path_well_known_core =
{ 2, { ".well-known", "core" } };

static const coap_endpoint_path_t path_board =
{ 1, { "board" } };

static const coap_endpoint_path_t path_mcu =
{ 1, { "mcu" } };

static const coap_endpoint_path_t path_ribot =
{ 1, { "ribot" } };

const coap_endpoint_t endpoints[] =
{
    { COAP_METHOD_GET,	handle_get_well_known_core,
      &path_well_known_core, "ct=40" },
    { COAP_METHOD_GET,	handle_get_board,
      &path_board,	   "ct=0"  },
    { COAP_METHOD_GET,	handle_get_mcu,
      &path_mcu,	   "ct=0"  },
    { COAP_METHOD_GET,	handle_get_ribot,
      &path_ribot,	   "ct=0"  },
    { COAP_METHOD_PUT,	handle_put_ribot,
      &path_ribot,	   "ct=0"  },
    /* marks the end of the endpoints array: */
    { (coap_method_t)0, NULL, NULL, NULL }
};

static int handle_get_well_known_core(coap_rw_buffer_t *scratch,
                                      const coap_packet_t *inpkt, coap_packet_t *outpkt,
                                      uint8_t id_hi, uint8_t id_lo)
{
    char *rsp = (char *)response;
    /* resetting the content of response message */
    memset(response, 0, sizeof(response));
    size_t len = sizeof(response);
    const coap_endpoint_t *ep = endpoints;
    int i;
    
    len--; // Null-terminated string
    
    while (NULL != ep->handler) {
        if (NULL == ep->core_attr) {
            ep++;
            continue;
        }
        
        if (0 < strlen(rsp)) {
            strncat(rsp, ",", len);
            len--;
        }
        
        strncat(rsp, "<", len);
        len--;
        
        for (i = 0; i < ep->path->count; i++) {
            strncat(rsp, "/", len);
            len--;
            
            strncat(rsp, ep->path->elems[i], len);
            len -= strlen(ep->path->elems[i]);
        }
        
        strncat(rsp, ">;", len);
        len -= 2;
        
        strncat(rsp, ep->core_attr, len);
        len -= strlen(ep->core_attr);
        
        ep++;
    }
    
    return coap_make_response(scratch, outpkt, (const uint8_t *)rsp,
                              strlen(rsp), id_hi, id_lo, &inpkt->tok,
                              COAP_RSPCODE_CONTENT,
                              COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

static int handle_get_board(coap_rw_buffer_t *scratch,
                            const coap_packet_t *inpkt,
                            coap_packet_t *outpkt,
                            uint8_t id_hi, uint8_t id_lo)
{
    const char *riot_name = RIOT_BOARD;
    size_t len = strlen(RIOT_BOARD);
    
    memcpy(response, riot_name, len);
    
    return coap_make_response(scratch, outpkt, (const uint8_t *)response, len,
                              id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT,
                              COAP_CONTENTTYPE_TEXT_PLAIN);
}

static int handle_get_mcu(coap_rw_buffer_t *scratch,
                          const coap_packet_t *inpkt,
                          coap_packet_t *outpkt,
                          uint8_t id_hi, uint8_t id_lo)
{
    const char *riot_mcu = RIOT_MCU;
    size_t  len = strlen(RIOT_MCU);
    
    memcpy(response, riot_mcu, len);
    
    return coap_make_response(scratch, outpkt, (const uint8_t *)response, len,
                              id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT,
                              COAP_CONTENTTYPE_TEXT_PLAIN);
}

static int handle_get_ribot(coap_rw_buffer_t *scratch,
                            const coap_packet_t *inpkt,
                            coap_packet_t *outpkt,
                            uint8_t id_hi, uint8_t id_lo)
{
    if (strlen(status) == 0) {
        sprintf(status, "Ready");
    }
    
    memcpy(response, status, strlen(status));
    
    return coap_make_response(scratch, outpkt, (const uint8_t *)response, 1,
                              id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT,
                              COAP_CONTENTTYPE_TEXT_PLAIN);
}


static void parse_message(int16_t * dir_speed_ratio, int16_t * angle_speed_ratio, int16_t * crc)
{
    char * strtokIndx;
  
    /* Left speed ratio */
    strtokIndx = strtok(message, ":");
    *angle_speed_ratio = atoi(strtokIndx);
  
    /* Right speed ratio */
    strtokIndx = strtok(NULL, ":");
    *dir_speed_ratio = atoi(strtokIndx);
  
    /* CRC */
    strtokIndx = strtok(NULL, "\n");
    *crc = atoi(strtokIndx);
    
#ifdef ENABLE_DEBUG
    DEBUG("Right speed ratio: %i\n", (int)*right_speed_ratio);
    DEBUG("Left speed ratio : %i\n", (int)*left_speed_ratio);
    DEBUG("CRC              : %i\n", (int)*crc);
#endif
}

static void update_control(int16_t dir_speed_ratio, int16_t angle_speed_ratio)
{
    int16_t right_speed_ratio = (int16_t)(SPEED_FACTOR * (dir_speed_ratio - angle_speed_ratio * ANGLE_FACTOR));
    int16_t left_speed_ratio = (int16_t)(SPEED_FACTOR * (dir_speed_ratio + angle_speed_ratio * ANGLE_FACTOR));
    
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
    uint8_t left_dir = left_speed_ratio > 0 ? 0 : 1;
    
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
    pwm_set(PWM_DEV(RIGHT_PWM_DEV), RIGHT_PWM_CHAN, right_speed);
    pwm_set(PWM_DEV(LEFT_PWM_DEV), LEFT_PWM_CHAN, left_speed);
}

static int handle_put_ribot(coap_rw_buffer_t *scratch,
                            const coap_packet_t *inpkt,
                            coap_packet_t *outpkt,
                            uint8_t id_hi, uint8_t id_lo)
{
    coap_responsecode_t resp = COAP_RSPCODE_CHANGED;
    memset(message, 0, sizeof(message));
    sprintf(message, "%s", (char*)inpkt->payload.p);
    
    printf("%s\n", message);
    int16_t angle_speed_ratio, dir_speed_ratio, crc;
    parse_message(&dir_speed_ratio, &angle_speed_ratio, &crc);
    
    if (crc == (angle_speed_ratio + dir_speed_ratio)) {
        if (angle_speed_ratio == 0 && dir_speed_ratio == 0) {
            sprintf(status, "ribot:Ready");
        }
        else {
            sprintf(status, "ribot:Moving");
        }
        update_control(dir_speed_ratio, angle_speed_ratio);
    }
    else {
        resp = COAP_RSPCODE_BAD_REQUEST;
    }
    
    /* Reply to server */
    int result = coap_make_response(scratch, outpkt, NULL, 0,
                                    id_hi, id_lo,
                                    &inpkt->tok, resp,
                                    COAP_CONTENTTYPE_TEXT_PLAIN);
    
    /* Send post notification to server */
    _send_coap_post((uint8_t*)"server", (uint8_t*)status);
    
    return result;
}
