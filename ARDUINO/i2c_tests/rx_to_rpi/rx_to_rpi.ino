// MultiChannels RC PWM Decoder
//
// rcarduino.blogspot.com
//
// A simple approach for reading three RC Channels using pin change interrupts
// rcarduino.blogspot.com
//

// include the pinchangeint library - see the links in the related topics section above for details
#include <PinChangeInt.h>
#include <Wire.h>
#include <Servo.h>

// Assign your channel in pins
#define THROTTLE_IN_PIN 8
#define YAW_IN_PIN 9
#define PITCH_IN_PIN 10
#define ROLL_IN_PIN 11

//Define I2C variables
#define SLAVE_ADDRESS 0x04

//LED pin for checking
#define LED_PIN 13

//For RC inputs Scaling
#define THR_MIN 890
#define THR_MAX 1895
#define RC_MIN 1000
#define RC_MAX 2000
#define K_YAW 30
#define K_PITCH 20
#define K_ROLL 20

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define THROTTLE_FLAG 1
#define YAW_FLAG 2
#define PITCH_FLAG 4
#define ROLL_FLAG 8

// holds the update flags defined above
volatile uint8_t bUpdateFlagsShared;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the 
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals
volatile uint16_t unThrottleInShared;
volatile uint16_t unYawInShared;
volatile uint16_t unPitchInShared;
volatile uint16_t unRollInShared;

// These are used to record the rising edge of a pulse in the calcInput functions
// They do not need to be volatile as they are only used in the ISR. If we wanted
// to refer to these in loop and the ISR then they would need to be declared volatile
uint32_t ulThrottleStart;
uint32_t ulYawStart;
uint32_t ulPitchStart;
uint32_t ulRollStart;

void setup()
{
  Serial.begin(9600);
  Serial.println("multiChannels");

  pinMode(LED_PIN, OUTPUT);

  // using the PinChangeInt library, attach the interrupts
  // used to read the channels
  PCintPort::attachInterrupt(THROTTLE_IN_PIN, calcThrottle,CHANGE); 
  PCintPort::attachInterrupt(YAW_IN_PIN, calcYaw,CHANGE); 
  PCintPort::attachInterrupt(PITCH_IN_PIN, calcPitch,CHANGE); 
  PCintPort::attachInterrupt(ROLL_IN_PIN, calcRoll,CHANGE); 

  // initialize i2c as slave
  // define call backs ofr I2C
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(SendRemote);

}

void loop()
{
  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained 
  // between calls to loop.
  static uint16_t unThrottleIn;
  static uint16_t unYawIn;
  static uint16_t unPitchIn;
  static uint16_t unRollIn;
  // local copy of update flags
  static uint8_t bUpdateFlags;

  // check shared update flags to see if any channels have a new signal
  if(bUpdateFlagsShared)
    {
      noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

      // take a local copy of which channels were updated in case we need to use this in the rest of loop
      bUpdateFlags = bUpdateFlagsShared;
    
      // in the current code, the shared values are always populated
      // so we could copy them without testing the flags
      // however in the future this could change, so lets
      // only copy when the flags tell us we can.
    
      if(bUpdateFlags & THROTTLE_FLAG)
	{
	  unThrottleIn = unThrottleInShared;
	}
    
      if(bUpdateFlags & YAW_FLAG)
	{
	  unYawIn = unYawInShared;
	}
    
      if(bUpdateFlags & PITCH_FLAG)
	{
	  unPitchIn = unPitchInShared;
	}

      if(bUpdateFlags & ROLL_FLAG)
	{
	  unRollIn = unRollInShared;
	}
     
      // clear shared copy of updated flags as we have already take
      // we still have a local copy if we need to use it in bUpdateFlags
      bUpdateFlagsShared = 0;
    
      interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on
      // as soon as interrupts are back on, we can no longer use the shared copies, the interrupt
      // service routines own these and could update them at any time. During the update, the 
      // shared copies may contain junk. Luckily we have our local copies to work with :-)
    }
  
  bUpdateFlags = 0;
}


// On request I2C data Sends shared values the Remote values
// for Throttle, Yaw, Pitch and Roll
// Note interrupts sotpped while sending values, should not 
// take much time
void SendRemote()
{

  union Sharedblock
  {
    byte b[4]; // utiliser char parts[4] pour port série
    float d;
  } RCsignal[4];

  RCsignal[0].d=
    (float) (unThrottleInShared-THR_MIN)/
    (THR_MAX-THR_MIN) * 100.0;
  RCsignal[1].d=
     ((float) unYawInShared-(RC_MAX+RC_MIN)/2)/
    (RC_MAX-RC_MIN) * K_YAW;
  RCsignal[2].d=
     ((float) unPitchInShared-(RC_MAX+RC_MIN)/2)/
    (RC_MAX-RC_MIN) * K_PITCH;
  RCsignal[3].d=
     ((float) unRollInShared-(RC_MAX+RC_MIN)/2)/
    (RC_MAX-RC_MIN) * K_ROLL;
  


  byte data[16];
  for (int i=0;i<4;i++)
    {
    for (int ii=0;ii<4;ii++)
      {
	data[ii+4*i] = RCsignal[i].b[ii];
      }
    }
 
  Wire.write(data,16);

}

// simple interrupt service routine
void calcThrottle()
{
  // if the pin is high, its a rising edge of the signal pulse, so lets record its value
  if(digitalRead(THROTTLE_IN_PIN) == HIGH)
    { 
      ulThrottleStart = micros();
    }
  else
    {
      // else it must be a falling edge, so lets get the time and subtract the time of the rising edge
      // this gives use the time between the rising and falling edges i.e. the pulse duration.
      unThrottleInShared = (uint16_t)(micros() - ulThrottleStart);
      // use set the throttle flag to indicate that a new throttle signal has been received
      bUpdateFlagsShared |= THROTTLE_FLAG;
    }
}

void calcYaw()
{
  if(digitalRead(YAW_IN_PIN) == HIGH)
    { 
      ulYawStart = micros();
    }
  else
    {
      unYawInShared = (uint16_t)(micros() - ulYawStart);
      bUpdateFlagsShared |= YAW_FLAG;
    }
}

void calcPitch()
{
  if(digitalRead(PITCH_IN_PIN) == HIGH)
    { 
      ulPitchStart = micros();
    }
  else
    {
      unPitchInShared = (uint16_t)(micros() - ulPitchStart);
      bUpdateFlagsShared |= PITCH_FLAG;
    }
}

void calcRoll()
{
  if(digitalRead(ROLL_IN_PIN) == HIGH)
    { 
      ulRollStart = micros();
    }
  else
    {
      unRollInShared = (uint16_t)(micros() - ulRollStart);
      bUpdateFlagsShared |= ROLL_FLAG;
    }
}
