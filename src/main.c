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
#include "c-utils/utils.h"
#include <unistd.h> // 包含 sleep 函数的头文件

typedef enum {
    dev_addr = 0,
    func_num,
    data_addr,
} modbus_data;

int main(int argc, char *argv[])
{
    int ret;
    modbus_t *ctx = NULL;
    modbus_mapping_t *mb_mapping = NULL;

    ilog("tcp ip: %s tcp port: %d\n", TCP_IP, TCP_PORT);
    ctx = modbus_new_tcp(TCP_IP, TCP_PORT);
    if (!ctx) {
        elog("modbus new tcp error\n");
        goto err_modbus_new_tcp;
    }

    ilog("modbus id: %d\n", SLAVE_ID);
    ret = modbus_set_slave(ctx, SLAVE_ID);
    if (ret == -1) {
        goto err_modbus_set_slave;
    }

    modbus_connect(ctx);

    for (;;) {
        uint16_t data[20];
        ret = modbus_read_input_registers(ctx, 1000, 20, data);
        sleep(1);
        if (ret == -1) {
            elog("read input registers error\n");
            continue;
        }

        ilog("read input registers: \n");
        for (int i = 0; i < 20; i++) {
            ilog("data[%d] = %d\n", i, data[i]);
        }
    }

    ilog("Quit the loop: %s\n", modbus_strerror(errno));

    /* For RTU, skipped by TCP (no TCP connect) */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;

err_modbus_mapping_new:
err_modbus_set_slave:
    modbus_close(ctx);
    modbus_free(ctx);
err_modbus_new_tcp:
    return -1;
}
