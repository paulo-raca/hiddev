#include <hiddev/core.h>

//  =================== HID DRIVER ===================


hiddev::Driver::Driver(Device &device)
: device(device)
{
	device.driver = this;
}

hiddev::Driver::~Driver() {
	device.driver = nullptr;
}


//  =================== HID DEVICE ===================


void hiddev::Device::start() {
	// Default to no-op
}

void hiddev::Device::stop() {
	// Default to no-op
}

bool hiddev::Device::sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
	if (!driver)
		return false;
	return driver->sendInputReport(reportNum, reportBuffer, reportSize);
}

bool hiddev::Device::receivedOutputReport(ReportType reportType, uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
	// Not supported
}

bool hiddev::Device::getReport(hiddev::ReportType reportType, uint8_t reportNum, uint8_t* reportBuffer, uint16_t &reportSize) {
	// Not supported
	reportSize = 0;
	return false;
}

bool hiddev::Device::setReport(hiddev::ReportType reportType, uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
	// Not supported
	return false;
}

bool hiddev::Device::getIdle(uint16_t &period_ms) {
	// Not supported, but default to 0 (Infinite)
	period_ms = 0;
	return false;
}

bool hiddev::Device::setIdle(uint16_t period_ms) {
	// Not supported
	return false;
}

bool hiddev::Device::getProtocol(hiddev::Protocol &protocol) {
	// Not supported, but default to Report protocol
	protocol = Protocol::Report;
	return false;
}

bool hiddev::Device::setProtocol(hiddev::Protocol protocol) {
	// Not supported
	return false;
}

bool hiddev::Device::isNumberedReport(hiddev::ReportType reportType) {
	// Defaults to non-numbered reports
	return false;
}
