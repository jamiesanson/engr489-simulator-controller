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

int testmsgcnt = 0;
String cmdCode = "";
ProgramState state = IDLE;

void setup() 
{
  // Initialise pin modes
  pinMode(THERMOCOUPLE_PIN, INPUT);
  pinMode(V_IN_PIN, INPUT);
  pinMode(I_SENSE_PIN, INPUT);
  pinMode(PSU_OUTPUT_PIN, OUTPUT);
  Serial.begin(BAUD);
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
  delay(2000);
  if (testmsgcnt <= 6)
  {
    Serial.println("Example main");
  } else {
    SUCCESS;
  }

  testmsgcnt++;
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

void testDevice()
{
  
}

void runMain()
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

