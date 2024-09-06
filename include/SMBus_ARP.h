#pragma once

#include "I2C_SMBus.h"

enum class ARPCmdType {
	General,
	Directed
};

class SMBus_ARP {
  public:
	SMBus_ARP();
	~SMBus_ARP();

	int prepareToARP();
	int resetDevice();
	int getUUID(ARPCmdType cmdType);
	int assignAddress();

	int resetDeviceARP();
	int notifyARPMaster();


};
