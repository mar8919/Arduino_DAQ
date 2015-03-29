#include <FileIO.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

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

YunServer server;

void setup() {
  // initialize communication:
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
  
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
  /*
  pinMode(A0, INPUT);  // P1
  pinMode(A1, INPUT);  // P2
  pinMode(A2, INPUT);  // P3
  pinMode(A3, INPUT);  // P4
  pinMode(A4, INPUT);  // P5
  pinMode(A5, INPUT);  // P6
  pinMode(A11, INPUT); // P7
  pinMode(A9, INPUT);  // NTC1
  pinMode(A10, INPUT); // NTC2
  */
  
  
  // initialize file
  FileSystem.begin();
}



void loop()
{
  // TEST PYRO STATUS, REMOVE AFTER TESTING
  /*
  byte my_pyro_stat = digitalRead(pyro_status);
  if(my_pyro_stat)
  {
    digitalWrite(sol1, HIGH);
    digitalWrite(sol2, HIGH);
    digitalWrite(sol3, HIGH);
    digitalWrite(sol4, HIGH);
  }
  else
  {
    digitalWrite(sol1, LOW);
    digitalWrite(sol2, LOW);
    digitalWrite(sol3, LOW);
    digitalWrite(sol4, LOW);
  }
  */
  
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
             
        dataOutput += String(current_time) + ",";
        dataOutput += String(analogRead(A0)) + ",";
        dataOutput += String(analogRead(A1)) + ",";
        dataOutput += String(analogRead(A2)) + ",";
        dataOutput += String(analogRead(A3)) + ",";
        dataOutput += String(analogRead(A4)) + ",";
        dataOutput += String(analogRead(A5)) + ",";
        dataOutput += String(analogRead(A11)) + "\n";
      }
      
      output.print(dataOutput);  // Write to SD Card
      output.close();
            
    }
  }
}
    


void process(YunClient client)
{
  String command = client.readStringUntil('/');  // Parse string for first section
  // remove newlines
  command.replace("\n", "");
  command.replace("\r", "");
  
  if(command == "stop")
  {
    stop_command(client);
    return;
  }
  if(command == "burn")
  {
    burn_command(client);
    return;
  }
  if(command == "digital")
  {
    digital_command(client);
    return;
  }
  if(command == "record")
  {
    record_command(client);
    return;
  }
  if(command == "clear")
  {
    clear_command(client);
    return; 
  }
}


void stop_command(YunClient client)
{
  digitalWrite(sol1, LOW);
  digitalWrite(sol2, LOW);
  digitalWrite(sol3, LOW);
  digitalWrite(sol4, LOW);
  digitalWrite(pyro_fire, LOW);
  digitalWrite(pyro_arm, LOW);
  client.println("Ok");
}

void burn_command(YunClient client)
{
  digitalWrite(sol1, HIGH);  //open Ox Flow
  delay(500);  //Delay .5 seconds before opening Ox flow
  digitalWrite(sol1, HIGH);  //open Ox Flow
  client.println("Ok");
}
  

void digital_command(YunClient client)
{
  int pin = client.parseInt();
  int value;
  if (client.read() == '/')
  {
    value = client.parseInt();
    digitalWrite(pin, value);
  }
  else
  {
    value = digitalRead(pin);
  }
  //server response
  String response = String(pin) + "," + String(value);
  client.println(response);
}


// Changes record state and opens/closes file
void record_command(YunClient client)
{
  if(record)
  {
    record = false;
    client.println("Stopped Recording");
  }
  else
  {
    record = true;
    client.println("Recording");
  }
}

//Clear output file
void clear_command(YunClient client)
{
  FileSystem.remove("/mnt/sd/arduino/www/output.csv");
  
  File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_WRITE);
  
  if(output)
  {
    output.println("Time [ms],P1,P2,P3,P4,P5,P6,P7");
    output.close();
  }
  client.println("Output Cleared");
}
