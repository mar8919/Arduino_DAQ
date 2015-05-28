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
unsigned long record_start_time;
boolean record = false;
boolean file_closed = true;
String event_log = "";

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
      for(int i = 0; i < 5; i++)
      {
        current_time = millis();
             
        dataOutput += String(current_time - record_start_time) + ",";
        //
        dataOutput += String(analogRead(A0)) + ",";
        dataOutput += String(analogRead(A1)) + ",";
        dataOutput += String(analogRead(A2)) + ",";
        dataOutput += String(analogRead(A3)) + ",";
        dataOutput += String(analogRead(A4)) + ",";
        dataOutput += String(analogRead(A5)) + ",";
        dataOutput += String(analogRead(A11)) + ",";
        
        /*
        dataOutput += String(analogRead(A0) * 1.1358 - 132.66) + ",";
        dataOutput += String(analogRead(A1) * 1.1358 - 132.66) + ",";
        dataOutput += String(analogRead(A2) * 1.1358 - 132.66) + ",";
        dataOutput += String(analogRead(A3) * 1.1358 - 132.66) + ",";
        dataOutput += String(analogRead(A4) * 1.1358 - 132.66) + ",";
        dataOutput += String(analogRead(A5) * 1.1358 - 132.66) + ",";
        dataOutput += String(analogRead(A11) * 1.1358 - 132.66) + ",";
        */
        
        dataOutput += String(read_ADC()) + "\n";
        
      }
      
      output.print(dataOutput);  // Write to SD Card
      output.close();
            
    }
  }
}
    






