QUADCOPTER
==========

A quadcopter controller on a Raspberry PI
------------------------------------------

This project is the development of a code to control a quadcopter
with a Raspberry Pi.

Following the Quadcopter V1, I want to use a 4 cahnnel RC remote to have better
control on the drone. In fact, the tablet shows quite poor sensitivity that is
an handicap for good quality fligths.

Therefore in theis version of the Quadcopter, the RPI will still hold the PID,
but a micorcontroller is used to interface the RC receiver and the ESCs. The Rpi
aks the microcontroller for RC commands and sends motor speed values through I2C.
An Arduino Micro has been chosen as microcontroller, for its compact size and
sufficient abilites for the job.

The Raspberry PI hosts:
- PID controller
- Web interface to access the Camera (work in progress)
- Communication with the MPU6050 through I2C.
- Communication to Arduino Micro through SPI


This project is greatly inspired and using source code from :
- https://github.com/richardghirst/PiBits
for ServoBlaster that I use to control the ESCs and the MPU6050 Digital motion processing.

- https://github.com/big5824/Picopter
for the Timing and the code structure

- https://code.google.com/p/owenquad/
for the Android app

Many thanks to these people.

Documentation
-------------

Principles
![Princples Diagram](https://github.com/vjaunet/QUADCOPTER_V2/blob/master/principles.png "Principles Diagram")

Usage
------

When powered, the Quad is in a locked status so that the ESCs do not turn the
motor on. To unlock, simply put the RC sticks to the lower center angle. It can
be locked again by doing the same operation. This is handled by the Arduino 
for more safety.


Hardware
--------

This projects includes :
- 4 brushless motors (TURNIGY 2204-14T 19g Outrunner)
- 4 Electronic Speed Controllers (Turnigy Multistar 20 Amp)
- 1 LiPo 3s 3700 mAh battery
- 1 sparkfun MPU6050 breakout board
- 1 QuadCopter frame
- 1 4 channels RC Remote + Receiver
- 1 arduino micro
- 1 logic level converter

Wiring
------

MPU6050 -> RPI :
-VDD -> 3.3V
-GND -> GND
-SDA -> SDA
-SCL -> SCL
-VIO -> 3.3V

Arduino -> RPI :
-VCC  -> 5V from external Regulator
-GND  -> GND
-MISO -> Logic Level converter ->MISO
-MOSI -> Logic Level converter ->MOSI
-SCK  -> Logic Level converter ->SCK

ESCs and RC Receiver on Arduino:
-Using PinChange interrupts
-Check in the arduino code
