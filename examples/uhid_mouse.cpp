#include <hiddev/device/mouse.h>
#include <hiddev/uhid.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>


#define LOG(fmt, ...) fprintf(stderr, "mouse: " fmt "\n", ##__VA_ARGS__)

int main() {
	hiddev::device::Mouse mouse;
	hiddev::UHid uhid(mouse);
	std::future<bool> future = uhid.runAsync();

    int oldX = 0;
    int oldY = 0;
    for (int i=0; ; i++) {
		int newX = 10*cos(i/100.0);
		int newY = 10*sin(i/100.0);
		mouse.move(newX - oldX, newY - oldY);
		oldX = newX;
		oldY = newY;
		usleep(1000);
	}
	return 0;
}
