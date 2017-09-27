/*
----------------------------------------------------------------------------------------
                            Solar simulator controller main sketch
----------------------------------------------------------------------------------------

    Contains code relating to the operation of a Teensy 3.6 Board to control a solar 
    simulator operating at varying temperature levels. It does so by taking measurements
    of both current and voltage at varying cells load, as well as being able to illuminate
    the cell being tested only when it needs to be.

    This code expects Serial in specific formats. These are:
      * "test\n"      - Runs a self test of the hardware and reports back via Serial, 
                        finalised by the use of the `SUCCESS;` macro.
      * "runsweep\n" - Runs the main operation of the hardware.
      * "reset\n"      - Stops the main operation of the hardware.
      * "sst36vuw\n"  - An announce from the connected device to check the controller exists

    The hardware used includes:
      * MAX31855K - SPI-based K-type thermocouple amplifier
      * X113647 and 28BYJ-48 - Stepper driver and motor respectively
      * MP930 - 1 ohm thick film power resistor
      * Generic 5 ohm and 10 ohm rotary potentiometers

*/

// INCLUDES
#include <Adafruit_MAX31855.h>
#include <X113647Stepper.h>
#include <ArduinoJson.h>

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
#define RUN_SWEEP_CMD String("runsweep")

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
  RUN_SWEEP
};

// Steps per revolution for stepper motor
static const int STEPS_PER_REVOLUTION = 64 * 32; 
static const int STEPS_PER_MEASUREMENT = 5;
// Range of degrees potentiometer can turn through
static const int POTENTIOMETER_RANGE = 270;
static const int STEPS_PER_SWEEP = POTENTIOMETER_RANGE*(STEPS_PER_REVOLUTION/360);

String cmdCode = "";
float temperatureBuffer[20] = {};
float temperatureThreshold = -1.0;
int stepCount = 0;

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
    case RUN_SWEEP:
      runSweepIt();
      break;      
  }

  emitState();
  delay(200);
}

void runTestIt()
{
  DynamicJsonBuffer jsonBuffer(200);  
  JsonObject& root = jsonBuffer.createObject();
  root["thermTemp"] = getThermocoupleTemp();
  root["rtd1Temp"] = getRTD1Temp();
  root["rtd2Temp"] = getRTD1Temp();
  root["volt"] = getLoadVoltage();
  root["curr"] = getLoadCurrent();
  root.printTo(Serial);
  Serial.println("Testing #1");
  delay(2000);
  Serial.println("This is another test response. TODO");
  delay(500);
  SUCCESS;
  state = IDLE;
}

// Single iteration of sweep command. Currently just calls stepAndMeasure until
// a full sweep of measurements is gathered
void runSweepIt()
{
  if (stepCount < STEPS_PER_SWEEP)
  {
//    stepAndMeasure();
    stepCount += STEPS_PER_MEASUREMENT;
  } 
  else 
  {
    resetPot();
    stepCount = 0;
    SUCCESS;
    state = IDLE;
  }
}

// To be called *after* a full sweep
void resetPot()
{
  StepperMotor.step(-STEPS_PER_SWEEP);
}

void emitState() 
{
  DynamicJsonBuffer jsonBuffer(200);  
  JsonObject& root = jsonBuffer.createObject();
  root["thermTemp"] = getThermocoupleTemp();
  root["rtd1Temp"] = getRTD1Temp();
  root["rtd2Temp"] = getRTD1Temp();
  root["volt"] = getLoadVoltage();
  root["curr"] = getLoadCurrent();
  root.printTo(Serial);
  Serial.print('\n');
}

void feedCommand()
{
  if (Serial.available()) 
  {
    cmdCode = Serial.readString();

    if (isCmd(TEST_CMD)) 
    {
      state = TEST;
    } 
    else if (isCmd(ANNOUNCE_CMD))
    {
      Serial.println("active");
    }
    else if (isCmd(RUN_SWEEP_CMD))
    {
      state = RUN_SWEEP;
    }
    
    cmdCode = "";
  }
}

float getThermocoupleTemp()
{
  double temp = 0.0;
  for (int i = 0; i < TEMP_AVERAGE_COUNT; i++)
  {
    temp += Thermocouple.readCelsius();
  }

  if (isnan(temp)) {
    return -200.0;
  } else {
    return temp/((float) TEMP_AVERAGE_COUNT);
  }
}

float getRTD1Temp()
{
  return 0.0;
}

float getRTD2Temp()
{
  return 0.0;
}

float getLoadVoltage()
{
  return 0.56;
}

float getLoadCurrent()
{
  return 1.0;
}
