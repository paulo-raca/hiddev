#include <hiddev/device/mouse.h>

#define REPORT_ID_MOUSE 1

static const uint8_t mouse_descriptor[] = {
    //  Mouse
    0x05, 0x01,                      // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                      // USAGE (Mouse)
    0xa1, 0x01,                      // COLLECTION (Application)

    // Report ID
    0x85, REPORT_ID_MOUSE,            //     REPORT_ID

    // Buttons
    0x05, 0x09,                      //     USAGE_PAGE (Button) */
    0x19, 0x01,                      //     USAGE_MINIMUM (Button 1) */
    0x29, 0x08,                      //     USAGE_MAXIMUM (Button 8) */
    0x15, 0x00,                      //     LOGICAL_MINIMUM (0) */
    0x25, 0x01,                      //     LOGICAL_MAXIMUM (1) */
    0x95, 0x08,                      //     REPORT_COUNT (8) */
    0x75, 0x01,                      //     REPORT_SIZE (1) */
    0x81, 0x02,                      //     INPUT (Data,Var,Abs) */

    // X, Y, Wheel
    0x05, 0x01,                      //     USAGE_PAGE (Generic Desktop) */
    0x09, 0x30,                      //     USAGE (X) */
    0x09, 0x31,                      //     USAGE (Y) */
    0x09, 0x38,                      //     USAGE (Wheel) */
    0x15, 0x81,                      //     LOGICAL_MINIMUM (-127) */
    0x25, 0x7f,                      //     LOGICAL_MAXIMUM (127) */
    0x75, 0x08,                      //     REPORT_SIZE (8) */
    0x95, 0x03,                      //     REPORT_COUNT (3) */
    0x81, 0x06,                      //     INPUT (Data,Var,Rel) */

    // End
    0xc0                             // END_COLLECTION
};

// Hiddev API

void hiddev::device::Mouse::getDescriptor(const uint8_t* &descriptorBuffer, uint16_t &descriptorSize) {
	descriptorBuffer = mouse_descriptor;
	descriptorSize = sizeof(mouse_descriptor);
}

bool hiddev::device::Mouse::getReport(hiddev::ReportType reportType, uint8_t reportNum, uint8_t* reportBuffer, uint16_t &reportSize) {
	if (reportType == hiddev::ReportType::Input && reportNum == REPORT_ID_MOUSE) {
		reportSize = sizeof(MouseReport);
		MouseReport &report = *(MouseReport*)reportBuffer;
		report.buttons = buttons;
		report.x = 0;
		report.y = 0;
		report.wheel = 0;
		return true;
	}
	return 0;
}

bool hiddev::device::Mouse::isNumberedReport(hiddev::ReportType reportType) {
	return true;
}

uint16_t hiddev::device::Mouse::getReportSize(hiddev::ReportType reportType, uint8_t reportNum) {
	if (reportType == hiddev::ReportType::Input && reportNum == 0)
		return sizeof(MouseReport);
	return 0;
}

// Mouse API

void hiddev::device::Mouse::move(int8_t x, int8_t y, int8_t wheel) {
	MouseReport report = {
		.buttons = buttons,
		.x = x,
		.y = y,
		.wheel = wheel
	};
	sendInputReport(REPORT_ID_MOUSE, (uint8_t*)&report, sizeof(report));
}

void hiddev::device::Mouse::click(MouseButton button) {
	press(button);
	release(button);
}

void hiddev::device::Mouse::press(MouseButton button) {
	this->buttons = (MouseButton)((uint8_t)this->buttons | (uint8_t)button);
	MouseReport report = {
		.buttons = buttons,
		.x = 0,
		.y = 0,
		.wheel = 0
	};
	sendInputReport(REPORT_ID_MOUSE, (uint8_t*)&report, sizeof(report));
}

void hiddev::device::Mouse::release(MouseButton button) {
	this->buttons = (MouseButton)((uint8_t)this->buttons & ~(uint8_t)button);
	MouseReport report = {
		.buttons = buttons,
		.x = 0,
		.y = 0,
		.wheel = 0
	};
	sendInputReport(0, (uint8_t*)&report, sizeof(report));
}

void hiddev::device::Mouse::releaseAll() {
	this->buttons = MouseButton::None;
	MouseReport report = {
		.buttons = buttons,
		.x = 0,
		.y = 0,
		.wheel = 0
	};
	sendInputReport(REPORT_ID_MOUSE, (uint8_t*)&report, sizeof(report));
}

bool hiddev::device::Mouse::isPressed(MouseButton button) {
	return (uint8_t)this->buttons & ~(uint8_t)button;
}
