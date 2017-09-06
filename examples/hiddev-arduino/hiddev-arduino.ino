#include<hiddev.h>
#include<hiddev/device/mouse.h>

hiddev::device::Mouse mouse;
hiddev::PluggableUSB UsbDriver(mouse);

void setup() {
  
}

void loop() {
    int oldX = 0;
    int oldY = 0;
    for (int i=0; ; i++) {
    int newX = 10*cos(i/100.0);
    int newY = 10*sin(i/100.0);
    //mouse.move(newX - oldX, newY - oldY);
    oldX = newX;
    oldY = newY;
    delay(1);
  }
}

