#include <FileIO.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <SPI.h>

// Output Pin Definithions
const int ledPin = 13; // the pin that the LED is attached to
const int sol1 = 2;
const int sol2 = 3;
const int sol3 = 4;
const int sol4 = 5;
const int pyro_arm = 6;
const int pyro_fire = 7;
const int pyro_status = 8;
const int pyro_test = 9;

/*
// Output Pind Definitions
const int A9 = 9;
const int A10 = 10;
const int A11 = 12;
*/

// Parameters
const int record_interval = 10;


int incomingByte;      // a variable to read incoming serial data into
int last_record = 0;
unsigned long current_time;
unsigned long write_time;
boolean record = false;
boolean file_closed = true;


//Recording Variables
unsigned long rTimeStamp, nTimeStamp;
int rA0, rA1, rA2, rA3, rA4, rA5, rA11;
int nA0, nA1, nA2, nA3, nA4, nA5, nA11;
float rLoad, nLoad;


YunServer server;

void setup() {
  // initialize communication:
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
  
  // start the SPI library:
  delay(1);
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  
  // Start serial for debug
  //Serial.begin(115200);
  
  // Run ADC initialization code
  ADC_Setup();
  
  // Output Initializations
  pinMode(sol1, OUTPUT);  // Pin 0 used by bridge, using pin 8
  pinMode(sol2, OUTPUT);  // Pin 1 used by bridge, using pin 11
  pinMode(sol3, OUTPUT);
  pinMode(sol4, OUTPUT);
  pinMode(pyro_arm, OUTPUT);
  pinMode(pyro_fire, OUTPUT);
  pinMode(pyro_status, INPUT);
  pinMode(pyro_test, OUTPUT);
  
  
  // Input Initializations
  pinMode(A0, INPUT);  // P1
  pinMode(A1, INPUT);  // P2
  pinMode(A2, INPUT);  // P3
  pinMode(A3, INPUT);  // P4
  pinMode(A4, INPUT);  // P5
  pinMode(A5, INPUT);  // P6
  pinMode(A11, INPUT); // P7
  pinMode(A10, INPUT); // NTC1
  
  
  // initialize file
  FileSystem.begin();
}


void loop()
{
  // Get clients coming from server
  YunClient client = server.accept();
  
  // Check for new client
  if (client)
  {
    process(client);
    
    //end client connection
    client.stop();
  }
  if (record)
  {
    read_inputs();
  }
}


/*
 * Used to record selected digital inputs to SD Card
 */
void read_inputs()
{
  //Serial.println("  Reading inputs");
  current_time = millis();
  if( (current_time - last_record) > record_interval)
  {
    File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_APPEND);
    if(output)
    {
      String dataOutput = "";
      for(int i = 0; i < 10; i++)
      {
        // Get New Readings
        nTimeStamp = millis();
        nA0 = analogRead(A0);
        nA1 = analogRead(A1);
        nA2  = analogRead(A2);
        nA3 = analogRead(A3);
        nA4 = analogRead(A4);
        nA5 = analogRead(A5);
        nA11 = analogRead(A11);
        nLoad = read_ADC();
        
        
        // Put differences into strings
        dataOutput += String(nTimeStamp - rTimeStamp) + ",";
        dataOutput += String(nA0 - rA0) + ",";
        dataOutput += String(nA1 - rA1) + ",";
        dataOutput += String(nA2 - rA2) + ",";
        dataOutput += String(nA3 - rA3) + ",";
        dataOutput += String(nA4 - rA4) + ",";
        dataOutput += String(nA5 - rA5) + ",";
        dataOutput += String(nA11 - rA11) + ",";
        dataOutput += String(nLoad - rLoad) + "\n";
        
        
        // Update Previous Values
        rTimeStamp = nTimeStamp;
        rA0 = nA0;
        rA1 = nA1;
        rA2 = nA2;
        rA3 = nA3;
        rA4 = nA4;
        rA5 = nA5;
        rA11 = nA11;
        rLoad = nLoad;
        
      }
      
      current_time = millis();
      output.print(dataOutput);  // Write to SD Card
      output.close();
            
    }
  }
}
    






