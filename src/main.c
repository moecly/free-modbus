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
    int rc;

    ilog("serial configs: serial_port = %s serial_baud = %d serial_parity = %c serial_data_bit = %d serial_stop_bit = %d\n", SERIAL_PORT, SERIAL_BAUD, SERIAL_PARITY, SERIAL_DATA_BIT, SERIAL_STOP_BIT);
    ctx = modbus_new_rtu(SERIAL_PORT, SERIAL_BAUD, SERIAL_PARITY, SERIAL_DATA_BIT, SERIAL_STOP_BIT);
    if (!ctx) {
        elog("modbus new rtu error\n");
        goto err_modbus_new_rtu;
    }

    ilog("modbus id: %d\n", SLAVE_ID);
    ret = modbus_set_slave(ctx, SLAVE_ID);
    if (ret == -1) {
        elog("modbus set slave id error\n");
        goto err_modbus_set_slave;
    }

    ret = modbus_connect(ctx);
    if (ret == -1) {
        elog("modbus connect error\n");
        goto err_modbus_connect;
    }

    for (;;) {
        uint16_t data[20];

        ret = modbus_read_input_registers(ctx, 1000, 20, data);
        sleep(1);

        if (ret == -1) {
            elog("read input registers err\n");
            continue;
        }

        ilog("read input registers: \n");
        for (int i = 0; i < 20; i++) {
            ilog("data[%d] = %d\n", i, data[i]);
        }
    }

    /* For RTU, skipped by TCP (no TCP connect) */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;

err_modbus_connect:
err_modbus_set_slave:
    modbus_close(ctx);
    modbus_free(ctx);
err_modbus_new_rtu:
    return -1;
}
