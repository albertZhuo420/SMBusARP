#include <cstring>
#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include "I2C_SMBus.h"
#include "SMBus_ARP.h"

SMBus_ARP::SMBus_ARP()
{
	this->fd				 = -1;
	this->current_slave_addr = -1;
}

SMBus_ARP::~SMBus_ARP()
{
	if (this->fd) {
		close(this->fd);
	}
}
int SMBus_ARP::init_smbus_arp(int i2cbus)
{
	this->fd = I2C_SMBus::open_i2c_dev(i2cbus);
	if (this->fd < 0) {
		throw std::invalid_argument("Invalid Argument");
	}

	if (I2C_SMBus::is_support_pec(this->fd)) {
		if (I2C_SMBus::smbus_use_pec(this->fd)) {
			return -1;
		}
	}

	return 0;
}

int SMBus_ARP::set_slave_address(int saddr_7bit)
{
	int ret = I2C_SMBus::set_slave_address(this->fd, saddr_7bit, false);
	if (ret < 0) {
		return ret;
	}

	this->current_slave_addr = saddr_7bit;

	return 0;
}

int SMBus_ARP::get_slave_address_7bit(void)
{
	return this->current_slave_addr;
}

int SMBus_ARP::prepare_to_arp(void)
{
	int ret = 0;
	if (SMBus_ARP_ADDR_7Bit != this->current_slave_addr) {
		if (ret = this->set_slave_address(SMBus_ARP_ADDR_7Bit)) {
			return ret;
		}
	}

	if (I2C_SMBus::i2c_smbus_write_byte(this->fd, static_cast<uint8_t>(SMBusARPCmd::Prepare_to_ARP))) {
		std::cout << std::strerror(errno) << "\n";
		return -1;
	}

	return 0;
}

int SMBus_ARP::reset_device(void)
{
	if (SMBus_ARP_ADDR_7Bit != this->current_slave_addr) {
		std::cerr << std::hex << std::showbase << "[Error] << The Current Slave Address is invalid | " << this->current_slave_addr << "\n";
		return -1;
	}

	int ret = I2C_SMBus::i2c_smbus_write_byte(this->fd, static_cast<uint8_t>(SMBusARPCmd::Reset_Device));
	if (ret < 0) {
		std::cout << "| reset_device() error: " << std::strerror(errno) << "\n";
		return ret;
	}

	return 0;
}

int SMBus_ARP::get_UUID(std::vector<uint8_t> &uuid_vec, uint8_t &slave_addr_7bit)
{
	if (SMBus_ARP_ADDR_7Bit != this->current_slave_addr) {
		std::cerr << std::hex << std::showbase << "[Error] << The Current Slave Address is invalid | " << this->current_slave_addr << "\n";
		return -1;
	}

	uint8_t uuid_arr[SMBus_DEV_UUID_SIZE + 1] {0};

	int ret = I2C_SMBus::i2c_smbus_read_block_data(this->fd, static_cast<uint8_t>(SMBusARPCmd::Get_UUID), uuid_arr);

	if (ret < 0) {
		std::cout << "| get_UUID() error: " << std::strerror(errno) << "\n";
		return ret;
	}

	if (ret != (SMBus_DEV_UUID_SIZE + 1)) {
		std::cout << std::hex << std::showbase << "[Warning] The return UUID data size is " << ret << "\n";
	}

	for (int i = 0; i < ret - 1; i++) {
		uuid_vec.push_back(uuid_arr[i]);
	}

	slave_addr_7bit = uuid_arr[SMBus_DEV_UUID_SIZE];

	return 0;
}

int SMBus_ARP::get_UUID(uint8_t target_addr_7bit, std::vector<uint8_t> &uuid_vec, uint8_t &slave_addr_7bit)
{
	if (SMBus_ARP_ADDR_7Bit != this->current_slave_addr) {
		std::cerr << std::hex << std::showbase << "[Error] << The Current Slave Address is invalid | " << this->current_slave_addr << "\n";
		return -1;
	}

	// this->set_slave_address(SMBus_ARP_ADDR_7Bit);

	uint8_t uuid_arr[SMBus_DEV_UUID_SIZE + 1] {0};

	int ret = I2C_SMBus::i2c_smbus_read_block_data(this->fd, ((target_addr_7bit << 1) | 0x1), uuid_arr);

	if (ret < 0) {
		std::cout << "| get_UUID() error: " << std::strerror(errno) << "\n";
		return ret;
	}

	if (ret != (SMBus_DEV_UUID_SIZE + 1)) {
		std::cout << std::hex << std::showbase << "[Warning] The return UUID data size is " << ret << "\n";
	}

	for (int i = 0; i < ret - 1; i++) {
		uuid_vec.push_back(uuid_arr[i]);
	}

	slave_addr_7bit = uuid_arr[SMBus_DEV_UUID_SIZE];

	return 0;
}

int SMBus_ARP::assign_address()
{
	
	return 0;
}

int SMBus_ARP::resetDeviceARP()
{
	return 0;
}

int SMBus_ARP::notifyARPMaster()
{
	return 0;
}

void SMBus_ARP::init_used_addr_pool(int pool_size)
{
	/**
	 * @TODO
	 */
	return;
}
