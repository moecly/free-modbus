/*
 * Copyright © Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <modbus.h>
#include "modbus-configs.h"
#include "c-utils/c-utils.h"

typedef enum {
    dev_addr = 0,
    func_num,
    data_addr,
} modbus_data;

int main(int argc, char *argv[])
{
    int ret;
    int server = -1;
    modbus_t *ctx = NULL;
    modbus_mapping_t *mb_mapping = NULL;
    int rc;

    ilog("tcp ip: %s tcp port: %d\n", TCP_IP, TCP_PORT);
    ctx = modbus_new_tcp(TCP_IP, TCP_PORT);
    if (!ctx) {
        elog("modbus new tcp error\n");
        goto err_modbus_new_tcp;
    }

    ilog("modbus id: %d\n", SLAVE_ID);
    server = modbus_tcp_listen(ctx, SLAVE_ID);

    ilog("start receive\n");
    modbus_tcp_accept(ctx, &server);

    mb_mapping = modbus_mapping_new_start_address(BITS_ADDRESS,
                                                  BITS_NB,
                                                  INPUT_BITS_ADDRESS,
                                                  INPUT_BITS_NB,
                                                  REGISTERS_ADDRESS,
                                                  REGISTERS_NB_MAX,
                                                  INPUT_REGISTERS_ADDRESS,
                                                  INPUT_REGISTERS_NB);
    if (mb_mapping == NULL) {
        elog("modbus maping new error\n");
        goto err_modbus_mapping_new;
    }

    for (int i = 0; i < INPUT_REGISTERS_NB; i++) {
        mb_mapping->tab_input_registers[i] = i;
    }

    for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

        do {
            rc = modbus_receive(ctx, query);
            dlog("have data\n");
            /* Filtered queries return 0 */
        } while (rc == 0);

        if (rc == -1)
            break;

        if (query[func_num] == MODBUS_FC_READ_COILS) {
        }

        if (query[func_num] == MODBUS_FC_READ_DISCRETE_INPUTS) {
        }

        if (query[func_num] == MODBUS_FC_READ_HOLDING_REGISTERS) {
        }

        if (query[func_num] == MODBUS_FC_READ_INPUT_REGISTERS) {
        }

        if (query[func_num] == MODBUS_FC_WRITE_SINGLE_COIL) {
        }

        if (query[func_num] == MODBUS_FC_WRITE_SINGLE_REGISTER) {
        }

        if (query[func_num] == MODBUS_FC_READ_EXCEPTION_STATUS) {
        }

        if (query[func_num] == MODBUS_FC_WRITE_MULTIPLE_COILS) {
        }

        if (query[func_num] == MODBUS_FC_WRITE_MULTIPLE_REGISTERS) {
        }

        if (query[func_num] == MODBUS_FC_REPORT_SLAVE_ID) {
        }

        if (query[func_num] == MODBUS_FC_MASK_WRITE_REGISTER) {
        }

        if (query[func_num] == MODBUS_FC_WRITE_AND_READ_REGISTERS) {
        }

        rc = modbus_reply(ctx, query, rc, mb_mapping);
        if (rc == -1) {
            break;
        }
    }

    ilog("Quit the loop: %s\n", modbus_strerror(errno));

    close(server);
    modbus_mapping_free(mb_mapping);

    /* For RTU, skipped by TCP (no TCP connect) */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;

    close(server);
    modbus_mapping_free(mb_mapping);
err_modbus_mapping_new:
    modbus_close(ctx);
    modbus_free(ctx);
err_modbus_new_tcp:
    return -1;
}
