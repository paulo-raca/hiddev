#pragma once

#ifdef __linux__

#include <hiddev/core.h>
#include <linux/uhid.h>
#include <future>

namespace hiddev {

	class UHid : public Driver {
		int fd;

	protected:
		virtual void setDeviceAttributes(struct uhid_create2_req &attributes);

	public:
		UHid(Device &device);
		~UHid();

		bool open();
		bool close();
		int getFD();
		operator bool();

		bool sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) override;

		bool handleMessage();
		bool run(); //open, handleMessages, close
		std::future<bool> runAsync();
	};
};

#endif  // ifdef __linux__
