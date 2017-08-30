#include "hiddev/hiddev.h"


//  =================== HID DRIVER ===================


hiddev::HidDriver::HidDriver(HidDevice &device)
: device(device)
{
	device.driver = this;
}

hiddev::HidDriver::~HidDriver() {
	device.driver = nullptr;
}


//  =================== HID DEVICE ===================


void hiddev::HidDevice::start() {
	// Default to no-op
}

void hiddev::HidDevice::stop() {
	// Default to no-op
}

bool hiddev::HidDevice::sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
	if (!driver)
		return false;
	return driver->sendInputReport(reportNum, reportBuffer, reportSize);
}

bool hiddev::HidDevice::receivedOutputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
	// Not supported
}

bool hiddev::HidDevice::getReport(hiddev::ReportType reportType, uint8_t reportNum, const uint8_t* &reportBuffer, uint16_t &reportSize) {
	// Not supported
	reportSize = 0;
	reportBuffer = nullptr;
	return false;
}

bool hiddev::HidDevice::setReport(hiddev::ReportType reportType, uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
	// Not supported
	return false;
}

bool hiddev::HidDevice::getIdle(uint16_t &period_ms) {
	// Not supported, but default to 0 (Infinite)
	period_ms = 0;
	return false;
}

bool hiddev::HidDevice::setIdle(uint16_t period_ms) {
	// Not supported
	return false;
}

bool hiddev::HidDevice::getProtocol(hiddev::Protocol &protocol) {
	// Not supported, but default to Report protocol
	protocol = Protocol::Report;
	return false;
}

bool hiddev::HidDevice::setProtocol(hiddev::Protocol protocol) {
	// Not supported
	return false;
}

bool hiddev::HidDevice::isNumberedReport(hiddev::ReportType reportType) {
	// Defaults to non-numbered reports
	return false;
}
