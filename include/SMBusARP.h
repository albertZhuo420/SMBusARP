#pragma once

#include "SMBus.h"

enum class ARPCmdType {
	General,
	Directed
};

class SMBusARP {
  public:
	SMBusARP();
	~SMBusARP();

	int prepareToARP();
	int resetDevice();
	int getUUID(ARPCmdType cmdType);
	int assignAddress();

	int resetDeviceARP();
	int notifyARPMaster();


};
