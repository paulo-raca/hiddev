#include <hiddev/pluggableusb.h>

#ifdef PLUGGABLE_USB_ENABLED
#include "HID.h"

#define LOG(x) Serial.println(x "\n")

static const uint8_t epType[] = {EP_TYPE_INTERRUPT_IN, EP_TYPE_INTERRUPT_OUT};

hiddev::PluggableUSB::PluggableUSB(hiddev::Device &device)
	: hiddev::Driver(device), ::PluggableUSBModule(2, 1, epType)
{
	::PluggableUSB().plug(this);
}

bool hiddev::PluggableUSB::sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
	if (device.isNumberedReport(hiddev::ReportType::Input)) {
		if (USB_Send(pluggedEndpoint, &reportNum, sizeof(reportNum)) < 0)
			return false;
	}
	if (USB_Send(pluggedEndpoint | TRANSFER_RELEASE, reportBuffer, reportSize) < 0) {
		return false;
	}

	return true;
}

uint8_t hiddev::PluggableUSB::getShortName(char* name) {
	strcpy(name, "hiddev");
	return 6;
}

int hiddev::PluggableUSB::getInterface(uint8_t* interfaceCount) {
	*interfaceCount += 1; // uses 1

	const uint8_t* descriptor = nullptr;
	uint16_t descriptorSize = 0;
	device.getDescriptor(descriptor, descriptorSize);

	HIDDescriptor hidInterface = {
		D_INTERFACE(pluggedInterface, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
		D_HIDREPORT(descriptorSize),
		D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01)
	};

	return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
}

int hiddev::PluggableUSB::getDescriptor(USBSetup& setup) {
	// Check if this is a HID Class Descriptor request
	if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE)
		return 0;
	if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE)
		return 0;

	// In a HID Class Descriptor wIndex contains the interface number
	if (setup.wIndex != pluggedInterface)
		return 0;

	// Send HID descriptor
	const uint8_t* descriptor = nullptr;
	uint16_t descriptorSize = 0;
	device.getDescriptor(descriptor, descriptorSize);
	USB_SendControl(0, descriptor, descriptorSize);

	// Reset the protocol on reenumeration. Normally the host should not assume the state of the protocol
	// due to the USB specs, but Windows and Linux just assumes its in report mode.
	device.setProtocol(hiddev::Protocol::Report);

	return descriptorSize;
}

bool hiddev::PluggableUSB::setup(USBSetup& setup) {
	if (pluggedInterface != setup.wIndex)
		return false;

	uint8_t request = setup.bRequest;
	uint8_t requestType = setup.bmRequestType;

	if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE) {
		if (request == HID_GET_REPORT) {
			hiddev::ReportType reportType = (hiddev::ReportType)setup.wValueH;
			uint8_t reportNum = setup.wValueL;
			uint16_t reportSize = setup.wLength;
			bool isNumberedReport = device.isNumberedReport(reportType);
			uint8_t report[reportSize];

			if (reportSize != device.getReportSize(reportType, reportNum) + (isNumberedReport ? 1 : 0)) {
				LOG("GET_REPORT: inconsistent reportSize");
				return false;
			}

			if (!device.getReport(reportType, reportNum, report, reportSize)) {
				LOG("GET_REPORT: failed");
				return false;
			}

			LOG("GET_REPORT");
			if (isNumberedReport) {
				USB_SendControl(0, reportNum, sizeof(reportNum));
			}
			USB_SendControl(0, report, reportSize);
			return true;

		} else if (request == HID_GET_PROTOCOL) {
			hiddev::Protocol protocol;
			if (!device.getProtocol(protocol)) {
				LOG("GET_PROTOCOL: failed");
				return false;
			}

			LOG("GET_PROTOCOL");
			uint8_t rawProtocol = (uint8_t)protocol;
			USB_SendControl(0, &rawProtocol, sizeof(rawProtocol));
			return true;

		} else if (request == HID_GET_IDLE) {
			uint16_t idle;
			if (!device.getIdle(idle)) {
				LOG("GET_IDLE: failed");
				return false;
			}

			LOG("GET_IDLE");
			// On USB, each IDLE unit is worth 4ms,
			//and the max value is limited to 255 = 1020ms
			uint8_t rawIdle = min(255, (idle + 3) / 4);
			USB_SendControl(0, &rawIdle, sizeof(rawIdle));
			return true;
		}

	} else if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE) {
		if (request == HID_SET_PROTOCOL) {
			if (!device.setProtocol((hiddev::Protocol)setup.wValueL)) {
				LOG("SET_PROTOCOL: failed");
				return false;
			};
			LOG("SET_PROTOCOL");
			return true;

		} else if (request == HID_SET_IDLE) {
			if (!device.setIdle(setup.wValueL * 4)) {
				LOG("SET_IDLE: failed");
				return false;
			}
			// On USB, each IDLE unit is worth 4ms
			LOG("SET_IDLE");
			return true;

		} else if (request == HID_SET_REPORT) {
			hiddev::ReportType reportType = (hiddev::ReportType)setup.wValueH;
			uint8_t reportNum = setup.wValueL;
			const uint16_t reportSize = setup.wLength;
			uint8_t report[reportSize];

			bool isNumberedReport = device.isNumberedReport(reportType);
			if (reportSize != device.getReportSize(reportType, reportNum) + (isNumberedReport ? 1 : 0)) {
				LOG("SET_REPORT: inconsistent reportSize");
				return true;
			}

			USB_RecvControl(report, reportSize);

			if (isNumberedReport && reportNum != report[0]) {
				LOG("SET_REPORT: Inconsistent reportNum");
				return true;
			}

			if (!device.setReport(reportType, reportNum, isNumberedReport ? report + 1 : report, isNumberedReport ? reportSize - 1 : reportSize)) {
				LOG("SET_REPORT: failed");
				return true;
			}
			LOG("SET_REPORT");
			return true;
		}
	}

	return false;
}

#endif  // ifdef PLUGGABLE_USB_ENABLED
