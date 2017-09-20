/*
----------------------------------------------------------------------------------------
                            Solar simulator Thermocouple Test sketch
----------------------------------------------------------------------------------------

    Contains code relating to the operation of a Teensy 3.6 Board to control a solar 
    simulator operating at varying temperature levels. This sketch tests the thermocouple output.
    
    The hardware used includes:
      * MAX31855K - SPI-based K-type thermocouple amplifier

*/

#include <SparkFunMAX31855k.h> // Using the max31855k driver

#define THERMOCOUPLE_CS 10

SparkFunMAX31855k probe(THERMOCOUPLE_CS);

enum ProgramState
{
  IDLE,
  GET_DATA
};

// Program state
ProgramState state = IDLE;
String cmdCode;

// Command definitions
#define ANNOUNCE_CMD String("sst36vuw")
#define RUN_CMD String("run_therm")
#define STOP_CMD String("stop")

// Useful macros
#define isCmd(cmd) String(cmdCode.substring(0, cmd.length())).equalsIgnoreCase(cmd)

void setup() 
{
  Serial.begin(115200);  
}
//void loop() {
// float temperature;
//  
//  // Read methods return NAN if they don't have a valid value to return.
//  // The following conditionals only print the value if it's not NAN.
//  temperature = probe.readCJT();
//  if (!isnan(temperature)) {
//    Serial.print("Cold Junction Temperature is (˚C): ");
//    Serial.println(temperature);
//  }
//  
//  // Read the temperature in Celsius
//  temperature = probe.readTempC();
//  if (!isnan(temperature)) {
//    Serial.print("Temp[C]=");
//    Serial.println(temperature);
//  }
//
//  // Read the temperature in Fahrenheit
//  temperature = probe.readTempF();
//  if (!isnan(temperature)) {
//    Serial.print("Temp[F]=");
//    Serial.println(temperature);
//  }
//
//  // Read the temperature in Kelvin
//  temperature = probe.readTempK();
//  if (!isnan(temperature)) {
//    Serial.print("Temp[K]=");
//    Serial.println(temperature);
//  }
//
//  // Read the temperature in Rankine
//  temperature = probe.readTempR();
//  if (!isnan(temperature)) {
//    Serial.print("Temp[R]=");
//    Serial.println(temperature);
//  }
//
//  delay(750);
//}
void loop() 
{
  feedCommand();

  switch (state) {
    case IDLE:
      // No-op
      break;
    case GET_DATA:
      runDataIt();
      break;   
  }
}

// Do a single data-gathering itteration
void runDataIt()
{
  float CJT;
  float TH;
  CJT = probe.readCJT();
  TH  = probe.readTempK();

  if (!isnan(CJT) && !isnan(TH))
  {
    sendTemperature(CJT, TH);
    delay(1000); // This is too fast by default so let's slow it down a bit
  }

}

// Feeds serial commands
void feedCommand()
{
  if (Serial.available()) 
  {
    cmdCode = Serial.readString();

    if (isCmd(RUN_CMD)) 
    {
      state = GET_DATA;
    } 
    else if (isCmd(STOP_CMD)) 
    {      
      Serial.println("disconnected");
      state = IDLE;
    } 
    else if (isCmd(ANNOUNCE_CMD))
    {
      Serial.println("active");
    }
  }
}

// Sends CSV temperature data back to connected computer
void sendTemperature(float coldJunctionTemp, float thermTempK)
{
  Serial.println("CJT," + String(coldJunctionTemp) + ",TH," + String(thermTempK));
}


