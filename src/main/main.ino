/*
----------------------------------------------------------------------------------------
                            Solar simulator controller main sketch
----------------------------------------------------------------------------------------

    Contains code relating to the operation of a Teensy 3.6 Board to control a solar 
    simulator operating at varying temperature levels. It does so by taking measurements
    of both current and voltage at varying cells load, as well as being able to illuminate
    the cell being tested only when it needs to be.

    This code expects Serial in specific formats. These are:
      * "test\n" - Runs a self test of the hardware and reports back via Serial, 
                   finalised by the use of the `SUCCESS;` macro.
      * "run\n"  - Runs the main operation of the hardware.
      * "stop\n" - Stops the main operation of the hardware.
      * "temp:5,10,15,20,25...\n" - Sets the temperature targets for the device.
      * "t_thresh:1.5\n" - Sets the temperature target threshold for taking measurements.
      * "sst36vuw\n" - An announce from the connected device to check the controller exists

    The hardware used includes:
      * MAX31855K - SPI-based K-type thermocouple amplifier
      * X113647 and 28BYJ-48 - Stepper driver and motor respectively
      * MP930 - 1 ohm thick film power resistor
      * Generic 5 ohm and 10 ohm rotary potentiometersa

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
#define ANNOUNCE_CMD String("sst36vuw")
#define TEMP_RNG_HEADER String("temp:")
#define TEMP_THR_HEADER String("t_thresh:")

// USEFUL MACROS
#define SUCCESS Serial.println(String("done"))
#define FAIL(err) Serial.println(String("fail"));Serial.println(String(err))
#define lengthOf( x )  ( sizeof( x ) / sizeof( *x ) )
#define isCmd(cmd) String(cmdCode.substring(0, cmd.length())).equalsIgnoreCase(cmd)

// CONFIG MACROS
#define TEMP_AVERAGE_COUNT 10

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
static const int STEPS_PER_MEASUREMENT = 5;

String cmdCode = "";
float temperatureBuffer[20] = {};
float temperatureThreshold = -1.0;

// Thermocouple instantiation. Usage: Thermocouple.readCelcius() : Double. Check output using isnan
Adafruit_MAX31855 Thermocouple(THERMOCOUPLE_CS); 

// Stepper Motor instantiation. Usage: StepperMotor.step(number_of_steps : Int)
X113647Stepper StepperMotor(STEPS_PER_REVOLUTION, SM_IN1, SM_IN2, SM_IN3, SM_IN4);

// Program state
ProgramState state = IDLE;

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
  Serial.println("Testing #1");
  delay(2000);
  Serial.println("Testing for AIDS");
  delay(1500);
  Serial.println("Testing testing 123");
  delay(450);
  Serial.println("Testerooni");
  delay(1500);
  Serial.println("Tests Complete");
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

void stepAndMeasure() 
{
  StepperMotor.step(STEPS_PER_MEASUREMENT);
  float temp = getCellTemperature();
  float loadV = getLoadVoltage();
  float loadI = getLoadCurrent();

  
}

boolean isTemperatureNearTarget()
{
  float temp = getCellTemperature();
  for (int i = 0; i < lengthOf(temperatureBuffer); i++)
  {
    if (abs(temp - temperatureBuffer[i]) < temperatureThreshold) {
      return true;
    }
  }

  return false;
}

void feedCommand()
{
  if (Serial.available()) 
  {
    cmdCode = Serial.readString();

    if (isCmd(TEMP_RNG_HEADER)) 
    {
      parseTemperatureRange();
    } else if (isCmd(TEMP_THR_HEADER)) 
    {      
      
    } 
    else       
    {
      runExternalCmd(); 
    }
    
    cmdCode = "";
  }
}

void parseTemperatureRange()
{
  String tempArr = cmdCode.substring(TEMP_RNG_HEADER.length());
  int commaIndex = 0;
  int count = 0;
  
  while (commaIndex != -1)
  {
    int nextComma = tempArr.indexOf(commaIndex, ',');
    // Length -1 is used on the following line as we want to neglect the '\n' character
    String temp = tempArr.substring(commaIndex, nextComma == -1 ? tempArr.length() - 1 : nextComma);

    if (count < lengthOf(temperatureBuffer)) 
    {
      temperatureBuffer[count] = temp.toFloat();
    }
    
    commaIndex = nextComma;
    count++;
  }
}

void parseTemperatureThreshold()
{
}

void runExternalCmd() {
  if (isCmd(TEST_CMD)) 
  {
    state = TEST;
  } 
  else if (isCmd(RUN_CMD))
  {
    float sumTempBuffer = 0.0;
    for (int i = 0; i < lengthOf(temperatureBuffer); i++) {
      sumTempBuffer += temperatureBuffer[i];
    }
    
    boolean setupOk = temperatureThreshold >= 0 && sumTempBuffer > 0.0;
    
    if (setupOk) 
    {
      state = RUN;
    } else 
    {
      FAIL("Temperature threshold or target values not set");
      state = IDLE;
    }
  }
  else if (isCmd(STOP_CMD))
  {
    state = IDLE;
  }
  else if (isCmd(ANNOUNCE_CMD))
  {
    Serial.println("active");
  }
}

float getCellTemperature()
{
  double temp = 0.0;
  for (int i = 0; i < TEMP_AVERAGE_COUNT; i++)
  {
    temp += Thermocouple.readCelsius();
  }

  return temp/10.0;
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

