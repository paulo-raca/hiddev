#ifdef __linux__

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <thread>
#include <hiddev/uhid.h>

// #define LOG(fmt, ...) fprintf(stderr, "uhid: " fmt "\n", ##__VA_ARGS__)
#define LOG(fmt, ...)

static hiddev::ReportType mapReportType(uint8_t reportType) {
	if (reportType == UHID_INPUT_REPORT)
		return hiddev::ReportType::Input;
	if (reportType == UHID_OUTPUT_REPORT)
		return hiddev::ReportType::Output;
	if (reportType == UHID_FEATURE_REPORT)
		return hiddev::ReportType::Feature;
	return hiddev::ReportType::Invalid;
}





hiddev::UHid::UHid(hiddev::Device &device)
: Driver(device), fd(-1)
{ }

hiddev::UHid::~UHid() {
	close();
}

bool hiddev::UHid::open() {
	if (fd >=0 ) {
		//Already open
		return false;
	}

	const uint8_t* hidDescriptor = nullptr;
	uint16_t hidDescriptorLength = 0;
	device.getDescriptor(hidDescriptor, hidDescriptorLength);

	fd = ::open("/dev/uhid", O_RDWR);
	if (fd < 0) {
		LOG("Failed to open dev/uhid");
		return false;
	}

	struct uhid_event ev;
	ev.type = UHID_CREATE2;
	memset(&ev.u.create2, 0, sizeof(ev.u.create2));
	setDeviceAttributes(ev.u.create2);
	ev.u.create2.rd_size = hidDescriptorLength;
	memcpy(ev.u.create2.rd_data, hidDescriptor, hidDescriptorLength);

	//Send CREATE2 event
	if (write(fd, &ev, sizeof(ev)) != sizeof(ev)) {
		LOG("Failed to send UHID_CREATE2 event: Wrong size");
		close();
		return false;
	}

	LOG("Opened");
	return true;
}

bool hiddev::UHid::close() {
	if (fd < 0)
		return false;

	::close(fd);
	fd = -1;
	LOG("Closed");
	return true;
}

int hiddev::UHid::getFD() {
	return fd;
}

hiddev::UHid::operator bool() {
	return fd >= 0;
}

void hiddev::UHid::setDeviceAttributes(struct uhid_create2_req &attributes) {
	strcpy((char*)attributes.name, "U2F Device");
}

bool hiddev::UHid::sendInputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
	if (fd < 0)
		return false;

	struct uhid_event ev;
	ev.type = UHID_INPUT2;
	if (device.isNumberedReport(ReportType::Input)) {
		ev.u.input2.size = reportSize+1;
		ev.u.input2.data[0] = reportNum;
		memcpy(ev.u.input2.data + 1, reportBuffer, reportSize);
	} else {
		ev.u.input2.size = reportSize;
		memcpy(ev.u.input2.data, reportBuffer, reportSize);
	}

	if (write(fd, &ev, sizeof(ev)) != sizeof(ev)) {
		LOG("Failed to send UHID_INPUT2 event: Wrong size");
		close();
		return false;
	}

	return true;
}

bool hiddev::UHid::handleMessage() {
	if (fd < 0)
		return false;

	struct uhid_event ev;
	if (read(fd, &ev, sizeof(ev)) != sizeof(ev)) {
		LOG("Failed to read event: Wrong size");
		close();
		return false;
	}

	switch (ev.type) {
		case UHID_START: {
			LOG("Flags: %lld", ev.u.start.dev_flags);
			// This can be ignored
			return true;
		}
		case UHID_STOP: {
			LOG("UHID_STOP");
			// This can be ignored
			return true;
		}

		case UHID_OPEN: {
			LOG("UHID_OPEN");
			device.start();
			return true;
		}

		case UHID_CLOSE: {
			LOG("UHID_CLOSE");
			device.stop();
			return true;
		}

		case UHID_OUTPUT: {
			LOG("UHID_OUTPUT");
			ReportType reportType = mapReportType(ev.u.output.rtype);
			uint8_t reportNum = ev.u.output.data[0];
			uint8_t* reportBuffer = ev.u.output.data + 1;
			uint16_t reportLength = ev.u.output.size - 1;

			device.receivedOutputReport(reportType, reportNum, reportBuffer, reportLength);
			return true;
		}

		case UHID_GET_REPORT: {
			LOG("UHID_GET_REPORT");
			uint32_t id =  ev.u.get_report.id;
			uint8_t reportNum = ev.u.get_report.rnum;
			ReportType reportType = mapReportType(ev.u.get_report.rtype);
			bool isNumberedReport = device.isNumberedReport(reportType);

			uint8_t* reportBuffer = isNumberedReport ? ev.u.get_report_reply.data + 1 : ev.u.get_report_reply.data;
			uint16_t reportLength = 0;
			bool ret = device.getReport(reportType, reportNum, reportBuffer, reportLength);

			ev.type = UHID_GET_REPORT_REPLY;
			ev.u.get_report_reply.id = id;
			if (!ret) {
				ev.u.get_report_reply.err = EIO;
				ev.u.get_report_reply.size = 0;
			} else {
				ev.u.get_report_reply.err = 0;
				if (isNumberedReport) {
					ev.u.get_report_reply.data[0] = reportNum;
					ev.u.get_report_reply.size = reportLength + 1;
				} else {
					ev.u.get_report_reply.size = reportLength;
				}
			}

			if (write(fd, &ev, sizeof(ev)) != sizeof(ev)) {
				LOG("Failed to send UHID_GET_REPORT_REPLY event: Wrong size");
				close();
				return false;
			}
			return true;
		}

		case UHID_SET_REPORT: {
			LOG("UHID_SET_REPORT");
			uint32_t id =  ev.u.set_report.id;
			uint8_t reportNum = ev.u.set_report.rnum;
			ReportType reportType = mapReportType(ev.u.set_report.rtype);
			const uint8_t* reportBuffer = ev.u.set_report.data + 1;
			uint16_t reportLength = ev.u.set_report.size - 1;

			bool ret = device.setReport(reportType, reportNum, reportBuffer, reportLength);

			ev.type = UHID_SET_REPORT_REPLY;
			ev.u.set_report_reply.id = id;
			if (!ret) {
				ev.u.get_report_reply.err = EIO;
			} else {
				ev.u.get_report_reply.err = 0;
			}

			if (write(fd, &ev, sizeof(ev)) != sizeof(ev)) {
				LOG("Failed to send UHID_SET_REPORT_REPLY event: Wrong size");
				close();
				return false;
			}
			return true;
		}
	}
}

bool hiddev::UHid::run() {
	if (!open())
		return false;

	while (handleMessage());
	close();
	return true;
}

std::future<bool> hiddev::UHid::runAsync() {
	return std::async( [](hiddev::UHid* uhid){ return uhid->run(); }, this);
}

#endif  // ifdef __linux__
