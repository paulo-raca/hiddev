#pragma once

#ifdef __linux__

#include "hiddev.h"
#include <linux/uhid.h>

namespace hiddev {

	class UhidDriver : public HidDriver {
		int fd;
	public:
		UhidDriver(HidDevice &device, bool open=true);
		~UhidDriver();


		bool open();
		bool close();
		int getFD();
		operator bool();

		virtual void setDeviceAttributes(struct uhid_create2_req &attributes);

		virtual bool sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize);

		bool handleMessage();
		bool handleMessageLoop();
	};
};

#endif  // ifdef __linux__
