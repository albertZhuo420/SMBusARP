#pragma once

#include <vector>

enum class ARPCmdType { General, Directed };

inline const int SMBus_ARP_ADDR_7Bit = 0x61;
inline const int SMBus_DEV_UUID_SIZE = 0x10;
inline const int SMBus_PEC_SIZE		 = 0x1;
// #define SMBus_ARP_ADDR_7Bit (0x61)
// #define SMBus_DEV_UUID_SIZE (0x10)
// #define SMBus_PEC_SIZE		(0x1)

enum SMBusARPCmd : uint8_t {
	Prepare_to_ARP = 0x1,
	Reset_Device   = 0x2,
	Get_UUID	   = 0x3,
};

class SMBus_ARP {
  public:
	SMBus_ARP();
	~SMBus_ARP();

	int init_smbus_arp(int i2cbus);
	int set_slave_address(int saddr_7bit);
	int get_slave_address_7bit(void);

	int prepare_to_arp(void);
	int reset_device(void);
	int get_UUID(std::vector<uint8_t> &uuid_val);
	int get_UUID(uint8_t target_addr_7bit, std::vector<uint8_t> &uuid_vec);
	int assignAddress();

	int resetDeviceARP();
	int notifyARPMaster();

  private:
	void init_used_addr_pool(int pool_size);

  private:
	int current_slave_addr;
	int fd;
};
