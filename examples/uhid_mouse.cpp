#include "hiddev/uhid.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <future>


#define LOG(fmt, ...) fprintf(stderr, "mouse: " fmt "\n", ##__VA_ARGS__)

static const uint8_t mouse_descriptor[] = {
		/*  Mouse relative */
		0x05, 0x01,                      /* USAGE_PAGE (Generic Desktop)	  54 */
		0x09, 0x02,                      /* USAGE (Mouse) */
		0xa1, 0x01,                      /* COLLECTION (Application) */

		/* 8 Buttons */
		0x05, 0x09,                      /*     USAGE_PAGE (Button) */
		0x19, 0x01,                      /*     USAGE_MINIMUM (Button 1) */
		0x29, 0x08,                      /*     USAGE_MAXIMUM (Button 8) */
		0x15, 0x00,                      /*     LOGICAL_MINIMUM (0) */
		0x25, 0x01,                      /*     LOGICAL_MAXIMUM (1) */
		0x95, 0x08,                      /*     REPORT_COUNT (8) */
		0x75, 0x01,                      /*     REPORT_SIZE (1) */
		0x81, 0x02,                      /*     INPUT (Data,Var,Abs) */

		/* X, Y, Wheel */
		0x05, 0x01,                      /*     USAGE_PAGE (Generic Desktop) */
		0x09, 0x30,                      /*     USAGE (X) */
		0x09, 0x31,                      /*     USAGE (Y) */
		0x09, 0x38,                      /*     USAGE (Wheel) */
		0x15, 0x81,                      /*     LOGICAL_MINIMUM (-127) */
		0x25, 0x7f,                      /*     LOGICAL_MAXIMUM (127) */
		0x75, 0x08,                      /*     REPORT_SIZE (8) */
		0x95, 0x03,                      /*     REPORT_COUNT (3) */
		0x81, 0x06,                      /*     INPUT (Data,Var,Rel) */

		/* End */
		0xc0                            /* END_COLLECTION */
	};

struct MouseReport {
	uint8_t buttons;
	int8_t xAxis;
	int8_t yAxis;
	int8_t wheel;
};



class Mouse: public hiddev::Device {
	virtual void getDescriptor(const uint8_t* &descriptorBuffer, uint16_t &descriptorSize) {
		descriptorBuffer = mouse_descriptor;
		descriptorSize = sizeof(mouse_descriptor);
		LOG("get descriptor");
	}

	virtual void start() {
		LOG("start");
	}
	virtual void stop() {
		LOG("stop");
	}

	virtual bool receivedOutputReport(uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
		LOG("Received Report: %d bytes", reportSize);
		return false;
	}

	virtual bool getReport(hiddev::ReportType reportType, uint8_t reportNum, const uint8_t* &reportBuffer, uint16_t &reportSize) {
		LOG("Get Report: %d %d", (int)reportType, reportNum);
		return false;
	}

	virtual bool setReport(hiddev::ReportType reportType, uint8_t reportNum, const uint8_t* reportBuffer, uint16_t reportSize) {
		LOG("Set Report: %d %d", (int)reportType, reportNum);
		return false;
	}
};



int main() {
	printf("main");
	Mouse mouse;
	hiddev::UHid uhid(mouse);
	std::future<bool> future = uhid.runAsync();
	printf("Yay!");

	MouseReport mouseReport;
	mouseReport.buttons = 0;
	mouseReport.wheel = 0;
	for (int i=0; ; i++) {
		mouseReport.xAxis = 10*sin(i/20.0);
		mouseReport.yAxis = 10*cos(i/20.0);
		usleep(10000);
		mouse.sendInputReport(0, (const uint8_t*) &mouseReport, sizeof(mouseReport));
	}
	return 0;
}
