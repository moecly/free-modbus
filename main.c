/*
 * Copyright © Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "modbus-rtu.h"
#include <stdint.h>
#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "c-utils/c-utils.h"
#include "modbus-configs.h"
#include "modbus-private.h"
#include <modbus.h>

typedef enum {
  dev_addr = 0,
  func_num,
  data_addr,
} modbus_data;

int main(int argc, char *argv[]) {
  int ret;
  modbus_t *ctx = NULL;
  modbus_mapping_t *mb_mapping = NULL;
  int rc;

  ilog("serial configs: serial_port = %s serial_baud = %d serial_parity = %c "
       "serial_data_bit = %d serial_stop_bit = %d\n",
       SERIAL_PORT, SERIAL_BAUD, SERIAL_PARITY, SERIAL_DATA_BIT,
       SERIAL_STOP_BIT);
  ctx = modbus_new_rtu(SERIAL_PORT, SERIAL_BAUD, SERIAL_PARITY, SERIAL_DATA_BIT,
                       SERIAL_STOP_BIT);
  if (!ctx) {
    elog("modbus new rtu error\n");
    goto err_modbus_new_rtu;
  }

  ctx->debug = 0;
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

#ifdef USE_RS485
  modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
#endif // USE_RS485
  mb_mapping = modbus_mapping_new_start_address(
      BITS_ADDRESS, BITS_NB, INPUT_BITS_ADDRESS, INPUT_BITS_NB,
      REGISTERS_ADDRESS, REGISTERS_NB_MAX, INPUT_REGISTERS_ADDRESS,
      INPUT_REGISTERS_NB);
  if (mb_mapping == NULL) {
    elog("modbus maping new error\n");
    goto err_modbus_mapping_new;
  }

  for (int i = 0; i < INPUT_REGISTERS_NB; i++) {
    mb_mapping->tab_input_registers[i] = i;
  }

  ilog("start receive\n");
  for (;;) {
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

    do {
      rc = modbus_receive(ctx, query);
      /* Filtered queries return 0 */
    } while (rc == 0);

    // if (rc == -1)
    //   break;

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

  dlog("errno: %d\n", errno);
  ilog("Quit the loop: %s\n", modbus_strerror(errno));

  modbus_mapping_free(mb_mapping);

  /* For RTU, skipped by TCP (no TCP connect) */
  modbus_close(ctx);
  modbus_free(ctx);

  return 0;

  modbus_mapping_free(mb_mapping);
err_modbus_mapping_new:
err_modbus_connect:
err_modbus_set_slave:
  modbus_close(ctx);
  modbus_free(ctx);
err_modbus_new_rtu:
  return -1;
}
