#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>

#include "I2C_SMBus.h"

int32_t I2C_SMBus::smbus_use_pec(int file)
{
	if (ioctl(file, I2C_PEC, 1) < 0) {
		fprintf(stderr, "Error: Could not set PEC: %s\n", std::strerror(errno));
		return -1;
	}
	return 0;
}

int32_t I2C_SMBus::open_i2c_dev(int i2cbus)
{
	int					  fd;
	std::string			  str_path = "";
	std::filesystem::path fs_path;

	str_path.append("/dev/i2c-").append(std::to_string(i2cbus));
	fs_path = str_path;

	if (false == std::filesystem::exists(fs_path)) {
		str_path.clear();
		str_path.append("/dev/i2c/").append(std::to_string(i2cbus));
		fs_path = str_path;
		if (false == std::filesystem::exists(fs_path)) {
			std::cout << "[ERROR] | Could not open file `/dev/i2c-" << i2cbus << " or `/dev/i2c/" << i2cbus << ": " << "No Device." << "\n";
			return -ENODEV;
		}
	}

	fd = open(str_path.c_str(), O_RDWR);
	if (fd < 0) {
		std::cout << "[ERROR] | Could not open file '" << str_path << "': " << std::strerror(errno) << "\n";
		if (EACCES == errno) {
			std::cout << "[ERROR] | Run as root?" << "\n";
		}
	}

	return fd;
}

int I2C_SMBus::set_slave_address(int fd, int saddr_7bit, bool force)
{
	/* With force, let the user read from/write to the registers
   even when a driver is also running */
	if (ioctl(fd, force ? I2C_SLAVE_FORCE : I2C_SLAVE, saddr_7bit) < 0) {
		std::cerr << "[Error] | Could not set address to 0x" << std::hex << saddr_7bit << ": " << std::strerror(errno) << "\n";
		return -errno;
	}
	// std::cout << "Set Destination slave device's address[0x" << std::hex << saddr_7bit << "] to I2C Controller success." << "\n";

	return 0;
}

int32_t I2C_SMBus::check_funcs(int file, XferSize size, int pec)
{
	unsigned long funcs;

	/* check adapter functionality */
	if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
		fprintf(stderr,
				"Error: Could not get the adapter "
				"functionality matrix: %s\n",
				strerror(errno));
		return -1;
	}

	switch (size) {
		case XferSize::I2C_SMBUS_BYTE_SIZE:
			if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE)) {
				fprintf(stderr, MISSING_FUNC_FMT, "SMBus receive byte");
				return -1;
			}
			if (!(funcs & I2C_FUNC_SMBUS_WRITE_BYTE)) {
				fprintf(stderr, MISSING_FUNC_FMT, "SMBus send byte");
				return -1;
			}
			break;

		case XferSize::I2C_SMBUS_BYTE_DATA_SIZE:
			if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA)) {
				fprintf(stderr, MISSING_FUNC_FMT, "SMBus read byte");
				return -1;
			}
			break;

		case XferSize::I2C_SMBUS_WORD_DATA_SIZE:
			if (!(funcs & I2C_FUNC_SMBUS_READ_WORD_DATA)) {
				fprintf(stderr, MISSING_FUNC_FMT, "SMBus read word");
				return -1;
			}
			break;

		case XferSize::I2C_SMBUS_BLOCK_DATA_SIZE:
			if (!(funcs & I2C_FUNC_SMBUS_READ_BLOCK_DATA)) {
				fprintf(stderr, MISSING_FUNC_FMT, "SMBus block read");
				return -1;
			}
			break;

		case XferSize::I2C_SMBUS_I2C_BLOCK_DATA_SIZE:
			if (!(funcs & I2C_FUNC_SMBUS_READ_I2C_BLOCK)) {
				fprintf(stderr, MISSING_FUNC_FMT, "I2C block read");
				return -1;
			}
			break;
	}

	if (pec && !(funcs & (I2C_FUNC_SMBUS_PEC | I2C_FUNC_I2C))) {
		fprintf(stderr,
				"Warning: Adapter does "
				"not seem to support PEC\n");
	}

	return 0;
}

int32_t I2C_SMBus::i2c_smbus_access(int file, I2CRW rw, uint8_t command, XferSize size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;
	int32_t						err;

	args.read_write = static_cast<uint8_t>(rw);
	args.command	= command;
	args.size		= static_cast<uint32_t>(size);
	args.data		= data;

	err = ioctl(file, I2C_SMBUS, &args);
	if (err == -1) {
		err = -errno;
	}
	return err;
}

int32_t I2C_SMBus::i2c_smbus_write_quick(int file, I2CRW rw)
{
	return I2C_SMBus::i2c_smbus_access(file, rw, 0, XferSize::I2C_SMBUS_QUICK_SIZE, nullptr);
}

int32_t I2C_SMBus::i2c_smbus_read_byte(int file)
{
	union i2c_smbus_data data;
	int					 err;

	err = I2C_SMBus::i2c_smbus_access(file, I2CRW::Read, 0, XferSize::I2C_SMBUS_BYTE_SIZE, &data);
	if (err < 0) {
		return err;
	}

	return 0x0FF & data.byte;
}

int32_t I2C_SMBus::i2c_smbus_write_byte(int file, uint8_t value)
{
	return I2C_SMBus::i2c_smbus_access(file, I2CRW::Write, value, XferSize::I2C_SMBUS_BYTE_SIZE, nullptr);
}

int32_t I2C_SMBus::i2c_smbus_read_byte_data(int file, uint8_t command)
{
	union i2c_smbus_data data;
	int					 err;

	err = I2C_SMBus::i2c_smbus_access(file, I2CRW::Read, command, XferSize::I2C_SMBUS_BYTE_DATA_SIZE, &data);
	if (err < 0) {
		return err;
	}

	return 0x0FF & data.byte;
}

int32_t I2C_SMBus::i2c_smbus_write_byte_data(int file, uint8_t command, uint8_t value)
{
	union i2c_smbus_data data;
	data.byte = value;
	return I2C_SMBus::i2c_smbus_access(file, I2CRW::Write, command, XferSize::I2C_SMBUS_BYTE_DATA_SIZE, &data);
}

int32_t I2C_SMBus::i2c_smbus_read_word_data(int file, uint8_t command)
{
	union i2c_smbus_data data;
	int					 err;

	err = I2C_SMBus::i2c_smbus_access(file, I2CRW::Read, command, XferSize::I2C_SMBUS_WORD_DATA_SIZE, &data);
	if (err < 0) {
		return err;
	}

	return 0x0FFFF & data.word;
}

int32_t I2C_SMBus::i2c_smbus_write_word_data(int file, uint8_t command, uint16_t value)
{
	union i2c_smbus_data data;
	data.word = value;
	return I2C_SMBus::i2c_smbus_access(file, I2CRW::Write, command, XferSize::I2C_SMBUS_WORD_DATA_SIZE, &data);
}

int32_t I2C_SMBus::i2c_smbus_process_call(int file, uint8_t command, uint16_t value)
{
	union i2c_smbus_data data;
	data.word = value;
	if (I2C_SMBus::i2c_smbus_access(file, I2CRW::Write, command, XferSize::I2C_SMBUS_PROC_CALL_SIZE, &data)) {
		return -1;
	}
	else {
		return 0x0FFFF & data.word;
	}
}

/* Returns the number of read bytes */
int32_t I2C_SMBus::i2c_smbus_read_block_data(int file, uint8_t command, uint8_t *values)
{
	union i2c_smbus_data data;
	int					 i, err;

	err = I2C_SMBus::i2c_smbus_access(file, I2CRW::Read, command, XferSize::I2C_SMBUS_BLOCK_DATA_SIZE, &data);
	if (err < 0) {
		return err;
	}

	for (i = 1; i <= data.block[0]; i++) {
		values[i - 1] = data.block[i];
	}
	return data.block[0];
}

int32_t I2C_SMBus::i2c_smbus_write_block_data(int file, uint8_t command, uint8_t length, const uint8_t *values)
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
	return I2C_SMBus::i2c_smbus_access(file, I2CRW::Write, command, XferSize::I2C_SMBUS_BLOCK_DATA_SIZE, &data);
}

/* Returns the number of read bytes */
/* Until kernel 2.6.22, the length is hardcoded to 32 bytes. If you
   ask for less than 32 bytes, your code will only work with kernels
   2.6.23 and later. */
int32_t I2C_SMBus::i2c_smbus_read_i2c_block_data(int file, uint8_t command, uint8_t length, uint8_t *values)
{
	union i2c_smbus_data data;
	int					 i, err;

	if (length > I2C_SMBUS_BLOCK_MAX) {
		length = I2C_SMBUS_BLOCK_MAX;
	}
	data.block[0] = length;

	err = I2C_SMBus::i2c_smbus_access(file,
									  I2CRW::Read,
									  command,
									  length == 32 ? XferSize::I2C_SMBUS_I2C_BLOCK_BROKEN_SIZE : XferSize::I2C_SMBUS_I2C_BLOCK_DATA_SIZE,
									  &data);
	if (err < 0) {
		return err;
	}

	for (i = 1; i <= data.block[0]; i++) {
		values[i - 1] = data.block[i];
	}
	return data.block[0];
}

int32_t I2C_SMBus::i2c_smbus_write_i2c_block_data(int file, uint8_t command, uint8_t length, const uint8_t *values)
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
	return I2C_SMBus::i2c_smbus_access(file, I2CRW::Write, command, XferSize::I2C_SMBUS_I2C_BLOCK_BROKEN_SIZE, &data);
}

/* Returns the number of read bytes */
int32_t I2C_SMBus::i2c_smbus_block_process_call(int file, uint8_t command, uint8_t length, uint8_t *values)
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

	err = I2C_SMBus::i2c_smbus_access(file, I2CRW::Write, command, XferSize::I2C_SMBUS_BLOCK_PROC_CALL_SIZE, &data);
	if (err < 0) {
		return err;
	}

	for (i = 1; i <= data.block[0]; i++) {
		values[i - 1] = data.block[i];
	}
	return data.block[0];
}
