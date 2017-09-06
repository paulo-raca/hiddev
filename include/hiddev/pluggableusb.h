#pragma once

#ifdef ARDUINO
#include <Arduino.h>

#ifdef PLUGGABLE_USB_ENABLED

#include <hiddev/core.h>
#include "PluggableUSB.h"




namespace hiddev {
	class PluggableUSB : public ::PluggableUSBModule, public Driver {
	public:
		PluggableUSB(Device &device);
		bool sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) override;

	protected:
		// PluggableUSBModule functions
		uint8_t getShortName(char* name) override;
		int getInterface(uint8_t* interfaceCount) override;
		int getDescriptor(USBSetup& setup) override;
		bool setup(USBSetup& setup) override;
	};
};

#endif  // ifdef PLUGGABLE_USB_ENABLED
#endif  // ifdef ARDUINO
