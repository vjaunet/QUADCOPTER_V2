// Rpi send data to Arduino over I2C
// Arduino sends ESCs signals according to this data

#include <Wire.h>
#include <Servo.h>

//Define I2C variables
#define SLAVE_ADDRESS 0x04

//LED pin for checking
#define LED_PIN 13

// Assign your channel out pins
#define FL_MOTOR_OUT_PIN 4
#define FR_MOTOR_OUT_PIN 5
#define BL_MOTOR_OUT_PIN 6
#define BR_MOTOR_OUT_PIN 7

//define Servo variables
Servo FL_MOTOR;
Servo FR_MOTOR;
Servo BL_MOTOR;
Servo BR_MOTOR;

void setup()
{
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);

  // attach servo objects, these will generate the correct 
  // pulses for driving Electronic speed controllers, servos or other devices
  // designed to interface directly with RC Receivers  
  FL_MOTOR.attach(FL_MOTOR_OUT_PIN);
  FR_MOTOR.attach(FR_MOTOR_OUT_PIN);
  BL_MOTOR.attach(BL_MOTOR_OUT_PIN);
  BR_MOTOR.attach(BR_MOTOR_OUT_PIN);

  // initialize i2c as slave
  // define call backs ofr I2C
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(SetServos);

}

void loop()
{
delay(10);	

}



void SetServos(int byteCount)
{

// the RPI shoud send 4 x 1 uint16_t (2 bytes) values
// one for each PID updated Motor speed

  //Expected bytes to be received
  uint16_t b[4];

  if (byteCount == 8)
    {
      while(Wire.available()) {
	
	for (int i=0;i<4;i++)
	  {
	    b[i]  = Wire.read() << 8; //lower bits
	    b[i] |= Wire.read();      //upper bits
	    
	    Serial.print("Received value ");
	    Serial.print(i);
	    Serial.print(" = ");
	    Serial.println(b[i]);
	  }
      }
	
    }
  else {
    //short read from master
    //Do nothing
    Serial.println("short read from master");
    Serial.print("Expected 8 bytes");
    Serial.print("Received");
    Serial.print(byteCount);
    Serial.print("bytes");
  }

  //Update servo values
  FL_MOTOR.writeMicroseconds(b[0]);
  FR_MOTOR.writeMicroseconds(b[1]);
  BL_MOTOR.writeMicroseconds(b[2]);
  BR_MOTOR.writeMicroseconds(b[3]);


}


