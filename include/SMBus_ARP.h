#pragma once

#include <vector>

inline const int SMBus_ARP_ADDR_7Bit = 0x61;
inline const int SMBus_DEV_UUID_SIZE = 0x10;
// inline const int SMBus_PEC_SIZE		 = 0x1;

enum class ARPCmdType { General, Directed };

enum SMBusARPCmd : uint8_t {
	Prepare_to_ARP = 0x1,
	Reset_Device   = 0x2,
	Get_UUID	   = 0x3,
};

struct SMBusUUID {
	SMBusUUID()
	{
		this->vender_spec_id	  = 0;
		this->subsystem.devide_id = 0;
		this->subsystem.vender_id = 0;
		this->interface			  = 0;
		this->device_id			  = 0;
		this->vender_id			  = 0;
		this->ver_revison		  = 0;
		this->dev_cap			  = 0;
	}

	uint32_t vender_spec_id;
	struct {
		uint16_t devide_id;
		uint16_t vender_id;
	} subsystem;

	uint16_t interface;
	uint16_t device_id;
	uint16_t vender_id;

	uint8_t ver_revison;
	uint8_t dev_cap;
} __attribute__((__packed__));

class SMBus_ARP {
  public:
	SMBus_ARP();
	~SMBus_ARP();

	int init_smbus_arp(int i2cbus);
	int set_slave_address(int saddr_7bit);
	int get_slave_address_7bit(void);

	int prepare_to_arp(void);
	int reset_device(void);
	int get_UUID(std::vector<uint8_t> &uuid_val, uint8_t &slave_addr_7bit);
	int get_UUID(uint8_t target_addr_7bit, std::vector<uint8_t> &uuid_vec, uint8_t &slave_addr_7bit);
	int assign_address();

	int resetDeviceARP();
	int notifyARPMaster();

  private:
	void init_used_addr_pool(int pool_size);

  private:
	int current_slave_addr;
	int fd;
};
