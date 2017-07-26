/*
----------------------------------------------------------------------------------------
                            Solar simulator controller main sketch
----------------------------------------------------------------------------------------

    Contains code relating to the operation of a Teensy 3.6 Board to control a solar 
    simulator operating at varying temperature levels. It does so by taking measurements
    of both current and voltage at varying cells load, as well as being able to illuminate
    the cell being tested only when it needs to be.

*/

// PIN DEFINITIONS
#define THERMOCOUPLE_PIN 1
#define PSU_OUTPUT_PIN 2
#define V_IN_PIN 3
#define I_SENSE_PIN 4

// OTHER DEFINITIONS
#define BAUD 115200

PROGMEM enum LightState 
{
  ON,
  OFF
};

void setup() 
{
  // Initialise pin modes
  pinMode(THERMOCOUPLE_PIN, INPUT);
  pinMode(V_IN_PIN, INPUT);
  pinMode(I_SENSE_PIN, INPUT);
  pinMode(PSU_OUTPUT_PIN, OUTPUT);

}

void loop() 
{
  
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

