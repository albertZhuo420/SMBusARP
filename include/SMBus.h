/**
 *	smbus.c - SMBus level access helper functions
 *
 *	Copyright (C) 1995-1997  Simon G. Vogl
 *	Copyright (C) 1998-1999  Frodo Looijaard <frodol@dds.nl>
 *	Copyright (C) 2012-2013  Jean Delvare <jdelvare@suse.de>
 *
 *	This library is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License as published
 *	by the Free Software Foundation; either version 2.1 of the License, or
 *	(at your option) any later version.
 */

#pragma once

#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* Compatibility defines */
#ifndef I2C_SMBUS_I2C_BLOCK_BROKEN
	#define I2C_SMBUS_I2C_BLOCK_BROKEN I2C_SMBUS_I2C_BLOCK_DATA
#endif
#ifndef I2C_FUNC_SMBUS_PEC
	#define I2C_FUNC_SMBUS_PEC I2C_FUNC_SMBUS_HWPEC_CALC
#endif

class SMBus {
  public:
	static int32_t i2c_smbus_access(int file, char read_write, uint8_t command, int size, union i2c_smbus_data *data);
	static int32_t i2c_smbus_write_quick(int file, uint8_t value);
	static int32_t i2c_smbus_read_byte(int file);
	static int32_t i2c_smbus_write_byte(int file, uint8_t value);
	static int32_t i2c_smbus_read_byte_data(int file, uint8_t command);
	static int32_t i2c_smbus_write_byte_data(int file, uint8_t command, uint8_t value);
	static int32_t i2c_smbus_read_word_data(int file, uint8_t command);
	static int32_t i2c_smbus_write_word_data(int file, uint8_t command, uint16_t value);
	static int32_t i2c_smbus_process_call(int file, uint8_t command, uint16_t value);

	/* Returns the number of read bytes */
	static int32_t i2c_smbus_read_block_data(int file, uint8_t command, uint8_t *values);
	static int32_t i2c_smbus_write_block_data(int file, uint8_t command, uint8_t length, const uint8_t *values);

	/* Returns the number of read bytes */
	/* Until kernel 2.6.22, the length is hardcoded to 32 bytes. If you
	   ask for less than 32 bytes, your code will only work with kernels
	   2.6.23 and later. */
	static int32_t i2c_smbus_read_i2c_block_data(int file, uint8_t command, uint8_t length, uint8_t *values);
	static int32_t i2c_smbus_write_i2c_block_data(int file, uint8_t command, uint8_t length, const uint8_t *values);

	/* Returns the number of read bytes */
	static int32_t i2c_smbus_block_process_call(int file, uint8_t command, uint8_t length, uint8_t *values);

  private:
	SMBus()							= delete;
	~SMBus()						= delete;
	SMBus(const SMBus &)			= delete;
	SMBus &operator=(const SMBus &) = delete;
};
