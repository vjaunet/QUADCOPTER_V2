// Rpi send data to Arduino over I2C
// Arduino sends ESCs signals according to this data

#include <Wire.h>
#include <Servo.h>

//Define I2C variables
#define SLAVE_ADDRESS 0x04

//LED pin for checking
#define LED_PIN 13

//Number of servos
#define SERVO_NUM 4

// Assign your channel out pins
#define FL_MOTOR_OUT_PIN 4
#define FR_MOTOR_OUT_PIN 5
#define BL_MOTOR_OUT_PIN 6
#define BR_MOTOR_OUT_PIN 7

//define Servo variables
Servo MOTOR[SERVO_NUM];

void setup()
{
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);

  // attach servo objects, these will generate the correct 
  // pulses for driving Electronic speed controllers, servos or other devices
  // designed to interface directly with RC Receivers  
  MOTOR[0].attach(FL_MOTOR_OUT_PIN);
  MOTOR[1].attach(FR_MOTOR_OUT_PIN);
  MOTOR[2].attach(BL_MOTOR_OUT_PIN);
  MOTOR[3].attach(BR_MOTOR_OUT_PIN);

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

  union int_byt{
    uint8_t b[2]; 
    uint16_t i;
  };    
  
  uint8_t trash;

  //Expected bytes to be received
  union int_byt *rcv_data = new int_byt[SERVO_NUM];

  if (byteCount == 8)
    {
      while(Wire.available()) {
	
	for (int i=0;i<SERVO_NUM;i++)
	  {
	    rcv_data[i].b[0] = Wire.read(); //upper bits?
	    rcv_data[i].b[1] = Wire.read(); //upper bits?
	  }
      }
	
    }
  else {
    //short read from master
    //Do nothing
    // Serial.println("Short read from master :");
    // Serial.print("Expected 8 bytes,");
    // Serial.print("Received ");
    // Serial.print(byteCount);
    // Serial.println(" bytes");

    return;
  }

  //Update servo values
  for (int i=0;i<SERVO_NUM;i++)
    {
      	    // Serial.print("Received value ");
	    // Serial.print(i);
	    // Serial.print(" = ");
	    // Serial.println(rcv_data[i].i);

      MOTOR[i].writeMicroseconds(rcv_data[i].i);
    }

  return;

}


