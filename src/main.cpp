#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

#include "SMBus_ARP.h"
#include "Utils.hpp"

#define Debug_Line()                                                                                                                                 \
	do {                                                                                                                                             \
		std::cout << std::dec << "| ===== " << __LINE__ << " ===== |\n";                                                                             \
	} while (0)

int main(void)
{
	if (Utils::is_little_endian()) {
		std::cout << "| ===== Little Endian ===== |\n";
	}
	else {
		std::cout << "| ===== Big Endian ===== |\n";
	}

	SMBus_ARP arp;

	if (arp.init_smbus_arp(0)) {
		Debug_Line();
		std::exit(1);
	}

	if (arp.prepare_to_arp()) {
		Debug_Line();
		std::exit(1);
	}

	if (arp.reset_device()) {
		Debug_Line();
		std::exit(1);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	Debug_Line();

	std::vector<uint8_t> uuid_vec;
	uint8_t				 slave_dev_addr_7bit = 0;

	if (arp.get_UUID(uuid_vec, slave_dev_addr_7bit)) {
		Debug_Line();
		std::exit(1);
	}

	for (const auto &val : uuid_vec) {
		std::cout << std::hex << std::showbase << static_cast<uint32_t>(val) << " ";
	}
	std::cout << "\n";

	SMBusUUID uuid = Utils::read_smbus_uuid(uuid_vec.data());
	std::cout << std::hex << std::showbase << "Device Capabilities: " << static_cast<uint32_t>(uuid.dev_cap) << "\n";
	std::cout << std::hex << std::showbase << "Device Capabilities: " << static_cast<uint32_t>(uuid.vender_spec_id) << "\n";
	std::cout << std::hex << std::showbase << "Device Capabilities: " << static_cast<uint32_t>(slave_dev_addr_7bit) << "\n";

	uint8_t assgn_addr_7bit = 0x64;
	uuid_vec.push_back(assgn_addr_7bit << 1);

	if (arp.assign_address(uuid_vec)) {
		Debug_Line();
		std::exit(1);
	}

	// 验证是否分配成功
	std::vector<uint8_t> uuid_assign;
	if (arp.get_UUID(assgn_addr_7bit, uuid_assign, slave_dev_addr_7bit)) {
		Debug_Line();
		std::exit(1);
	}

	if (assgn_addr_7bit == slave_dev_addr_7bit) {
		std::cout << "Assign Address Success.\n";
	}
	else {
		std::cout << std::hex << std::showbase << "Assign Address failed, expect Addr(7 bit) is " << static_cast<uint32_t>(assgn_addr_7bit)
				  << ", but the received Addr(7 bit) is " << static_cast<uint32_t>(slave_dev_addr_7bit) << "\n";
	}

	return 0;
}
