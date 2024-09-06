#include <iostream>
#include <chrono>
#include <thread>

#include "SMBus_ARP.h"

#define Debug_Line()                                                                                                                                 \
	do {                                                                                                                                             \
		std::cout << std::dec << "| ===== " << __LINE__ << " ===== |\n";                                                                             \
	} while (0)

int main(void)
{
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

	Debug_Line();
	std::vector<uint8_t> uuid_vec;

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	if (arp.get_UUID(uuid_vec)) {
		Debug_Line();
		std::exit(1);
	}

	for (const auto &val : uuid_vec) {
		std::cout << std::hex << std::showbase << static_cast<uint32_t>(val) << " ";
	}
	std::cout << "\n";

	return 0;
}

/**
 *
 *
 * [root@stm32mp1]:~$ i2cdetect -y 3
 *      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
 * 00:                         -- -- -- -- -- -- -- --
 * 10: UU -- -- -- -- -- -- -- -- -- -- 1b -- 1d -- --
 * 20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 30: -- -- -- -- -- -- 36 -- -- -- -- -- -- -- -- --
 * 40: -- -- -- -- -- -- -- -- -- -- UU -- -- -- -- --
 * 50: -- UU -- 53 -- -- -- -- -- -- -- -- -- -- -- --
 * 60: -- 61 -- -- -- -- -- -- -- -- 6a -- -- -- -- --
 * 70: -- -- -- -- -- -- -- --
 *
 * [root@stm32mp1]:~$ i2cdetect -y 3
 *      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
 * 00:                         -- -- -- -- -- -- -- --
 * 10: UU -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 40: -- -- -- -- -- -- -- -- -- -- UU -- -- -- -- --
 * 50: -- UU -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * 70: -- -- -- -- -- -- -- --
 *
 * [root@stm32mp1]:~$
 *
 */
