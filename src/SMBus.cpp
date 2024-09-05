#include "SMBusARP.h"

int32_t SMBus::i2c_smbus_access(int file, char read_write, uint8_t command, int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;
	int32_t						err;

	args.read_write = read_write;
	args.command	= command;
	args.size		= size;
	args.data		= data;

	err = ioctl(file, I2C_SMBUS, &args);
	if (err == -1) {
		err = -errno;
	}
	return err;
}

int32_t SMBus::i2c_smbus_write_quick(int file, uint8_t value)
{
	return SMBus::i2c_smbus_access(file, value, 0, I2C_SMBUS_QUICK, NULL);
}

int32_t SMBus::i2c_smbus_read_byte(int file)
{
	union i2c_smbus_data data;
	int					 err;

	err = SMBus::i2c_smbus_access(file, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data);
	if (err < 0) {
		return err;
	}

	return 0x0FF & data.byte;
}

int32_t SMBus::i2c_smbus_write_byte(int file, uint8_t value)
{
	return SMBus::i2c_smbus_access(file, I2C_SMBUS_WRITE, value, I2C_SMBUS_BYTE, NULL);
}

int32_t SMBus::i2c_smbus_read_byte_data(int file, uint8_t command)
{
	union i2c_smbus_data data;
	int					 err;

	err = SMBus::i2c_smbus_access(file, I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA, &data);
	if (err < 0) {
		return err;
	}

	return 0x0FF & data.byte;
}

int32_t SMBus::i2c_smbus_write_byte_data(int file, uint8_t command, uint8_t value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return SMBus::i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &data);
}

int32_t SMBus::i2c_smbus_read_word_data(int file, uint8_t command)
{
	union i2c_smbus_data data;
	int					 err;

	err = SMBus::i2c_smbus_access(file, I2C_SMBUS_READ, command, I2C_SMBUS_WORD_DATA, &data);
	if (err < 0) {
		return err;
	}

	return 0x0FFFF & data.word;
}

int32_t SMBus::i2c_smbus_write_word_data(int file, uint8_t command, uint16_t value)
{
	union i2c_smbus_data data;
	data.word = value;
	return SMBus::i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_WORD_DATA, &data);
}

int32_t SMBus::i2c_smbus_process_call(int file, uint8_t command, uint16_t value)
{
	union i2c_smbus_data data;
	data.word = value;
	if (SMBus::i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_PROC_CALL, &data)) {
		return -1;
	}
	else {
		return 0x0FFFF & data.word;
	}
}

/* Returns the number of read bytes */
int32_t SMBus::i2c_smbus_read_block_data(int file, uint8_t command, uint8_t *values)
{
	union i2c_smbus_data data;
	int					 i, err;

	err = SMBus::i2c_smbus_access(file, I2C_SMBUS_READ, command, I2C_SMBUS_BLOCK_DATA, &data);
	if (err < 0) {
		return err;
	}

	for (i = 1; i <= data.block[0]; i++) {
		values[i - 1] = data.block[i];
	}
	return data.block[0];
}

int32_t SMBus::i2c_smbus_write_block_data(int file, uint8_t command, uint8_t length, const uint8_t *values)
{
	union i2c_smbus_data data;
	int					 i;
	if (length > I2C_SMBUS_BLOCK_MAX) {
		length = I2C_SMBUS_BLOCK_MAX;
	}
	for (i = 1; i <= length; i++) {
		data.block[i] = values[i - 1];
	}
	data.block[0] = length;
	return SMBus::i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BLOCK_DATA, &data);
}

/* Returns the number of read bytes */
/* Until kernel 2.6.22, the length is hardcoded to 32 bytes. If you
   ask for less than 32 bytes, your code will only work with kernels
   2.6.23 and later. */
int32_t SMBus::i2c_smbus_read_i2c_block_data(int file, uint8_t command, uint8_t length, uint8_t *values)
{
	union i2c_smbus_data data;
	int					 i, err;

	if (length > I2C_SMBUS_BLOCK_MAX) {
		length = I2C_SMBUS_BLOCK_MAX;
	}
	data.block[0] = length;

	err = SMBus::i2c_smbus_access(file, I2C_SMBUS_READ, command, length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN : I2C_SMBUS_I2C_BLOCK_DATA, &data);
	if (err < 0) {
		return err;
	}

	for (i = 1; i <= data.block[0]; i++) {
		values[i - 1] = data.block[i];
	}
	return data.block[0];
}

int32_t SMBus::i2c_smbus_write_i2c_block_data(int file, uint8_t command, uint8_t length, const uint8_t *values)
{
	union i2c_smbus_data data;
	int					 i;
	if (length > I2C_SMBUS_BLOCK_MAX) {
		length = I2C_SMBUS_BLOCK_MAX;
	}
	for (i = 1; i <= length; i++) {
		data.block[i] = values[i - 1];
	}
	data.block[0] = length;
	return SMBus::i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_I2C_BLOCK_BROKEN, &data);
}

/* Returns the number of read bytes */
int32_t SMBus::i2c_smbus_block_process_call(int file, uint8_t command, uint8_t length, uint8_t *values)
{
	union i2c_smbus_data data;
	int					 i, err;

	if (length > I2C_SMBUS_BLOCK_MAX) {
		length = I2C_SMBUS_BLOCK_MAX;
	}
	for (i = 1; i <= length; i++) {
		data.block[i] = values[i - 1];
	}
	data.block[0] = length;

	err = SMBus::i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BLOCK_PROC_CALL, &data);
	if (err < 0) {
		return err;
	}

	for (i = 1; i <= data.block[0]; i++) {
		values[i - 1] = data.block[i];
	}
	return data.block[0];
}
