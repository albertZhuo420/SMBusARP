#pragma once

#include "SMBus_ARP.h"
#include <cstdint>

class Utils {
  public:
	static bool is_little_endian()
	{
		uint16_t num  = 0x1;
		uint8_t *byte = reinterpret_cast<uint8_t *>(&num);
		return byte[0] == 1;
	}

	static uint16_t swap_uint16(uint16_t val) { return (val << 8) | (val >> 8); }

	static uint32_t swap_uint32(uint32_t val)
	{
		return ((val << 24) & 0xFF000000) | ((val << 8) & 0x00FF0000) | ((val >> 8) & 0x0000FF00) | ((val >> 24) & 0x000000FF);
	}

	static SMBusUUID read_smbus_uuid(uint8_t *data)
	{
		SMBusUUID uuid;

		// 0x81 0x08 0x1c 0xc4 0x6a 0x13 0x00 0x35 0x1c 0xc4 0x6a 0x13 0xcf 0x41 0x37 0xff

		// 按照小端顺序直接赋值
		uuid.dev_cap	 = data[0];	 // dev_cap = 0x81
		uuid.ver_revison = data[1];	 // ver_revison = 0x08

		uuid.vender_id = (data[2] << 8) | data[3];	// vender_id = 0x1c 0xc4
		uuid.device_id = (data[4] << 8) | data[5];	// device_id = 0x6a 0x13
		uuid.interface = (data[6] << 8) | data[7];	// interface = 0x00 0x35

		uuid.subsystem.vender_id = (data[8] << 8) | data[9];	// subsystem.vender_id = 0x1c 0xc4
		uuid.subsystem.devide_id = (data[10] << 8) | data[11];	// subsystem.devide_id = 0x6a 0x13

		uuid.vender_spec_id = (data[12] << 24) | (data[13] << 16) | (data[14] << 8) | data[15];	 // vender_spec_id = 0xcf 0x41 0x37 0xff

		return uuid;
	}

  private:
	Utils()							= delete;
	~Utils()						= delete;
	Utils(const Utils &)			= delete;
	Utils &operator=(const Utils &) = delete;
};
