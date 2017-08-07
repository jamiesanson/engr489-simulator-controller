/*
----------------------------------------------------------------------------------------
                            Solar simulator controller main sketch
----------------------------------------------------------------------------------------

    Contains code relating to the operation of a Teensy 3.6 Board to control a solar 
    simulator operating at varying temperature levels. It does so by taking measurements
    of both current and voltage at varying cells load, as well as being able to illuminate
    the cell being tested only when it needs to be.

*/

// INCLUDES
#include "Adafruit_MAX31855.h"
#include <X113647Stepper.h>

// PIN DEFINITIONS
#define PSU_OUTPUT_PIN 2
#define V_IN_PIN 3
#define I_SENSE_PIN 4
#define THERMOCOUPLE_CS 10
#define SM_IN1 16
#define SM_IN2 17
#define SM_IN3 18
#define SM_IN4 19

// SERIAL DEFINITIONS
#define BAUD 115200
#define TEST_CMD String("test")
#define RUN_CMD String("run")
#define STOP_CMD String("stop")

#define SUCCESS Serial.println(String("done"))
#define FAIL(err) Serial.println(String("fail"));Serial.println(String(err))

PROGMEM enum LightState 
{
  ON,
  OFF
};

PROGMEM enum ProgramState
{
  IDLE,
  TEST,
  RUN
};

// Steps per revolution for stepper motor
static const int STEPS_PER_REVOLUTION = 64 * 32; 

String cmdCode = "";
ProgramState state = IDLE;

// Thermocouple instantiation. Usage: Thermocouple.readCelcius() : Double. Check output using isnan
Adafruit_MAX31855 Thermocouple(THERMOCOUPLE_CS); 

// Stepper Motor instantiation. Usage: StepperMotor.step(number_of_steps : Int)
X113647Stepper StepperMotor(STEPS_PER_REVOLUTION, SM_IN1, SM_IN2, SM_IN3, SM_IN4);

void setup() 
{
  // Initialise pin modes
  pinMode(V_IN_PIN, INPUT);
  pinMode(I_SENSE_PIN, INPUT);
  pinMode(PSU_OUTPUT_PIN, OUTPUT);
  Serial.begin(BAUD);
  StepperMotor.setSpeed(5);
}

void loop() 
{
  feedCommand();

  switch (state)
  {
    case IDLE:
      // NO-OP
      break;
    case TEST:
      runTestIt();
      break;
    case RUN:
      runMainIt();
      break;
      
  }
}

void runTestIt()
{
  delay(2000);
  SUCCESS;
  state = IDLE;
}

void runMainIt()
{
  // TODO: - Wait for temperature to reach set values
  //       - Run pot sweep measuring voltage and current
  //       - Log data to SD card and repeat
  //       - Wait for next temperature value
  SUCCESS;
  state = IDLE;
}

void feedCommand()
{
  if (Serial.available()) 
  {
    cmdCode = Serial.readString();
    runExternalCmd();
    cmdCode = "";
  }
}

void runExternalCmd() {
  if (cmdCode == (TEST_CMD + String('\n'))) 
  {
    state = TEST;
  } 
  else if (cmdCode == (RUN_CMD + String('\n')))
  {
    state = RUN;
  }
  else if (cmdCode == (STOP_CMD + String('\n')))
  {
    state = IDLE;
  }
}

float getCellTemperature()
{
  
}

float getLoadVoltage()
{
  
}

float getLoadCurrent()
{
  
}

void setLightState(LightState state)
{
  switch(state) 
  {
    case ON:
      break;
    case OFF:
      break;
  }
}

