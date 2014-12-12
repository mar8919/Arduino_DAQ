#include <FileIO.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

const int ledPin = 13; // the pin that the LED is attached to
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
  
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  
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
  
  
  /*
  if (record)
  {
    //Only open file first for first record
    if(file_closed)
    {
      File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_APPEND);
    }
    //read inputs and record data to file
    read_inputs(output);
  }
  //close file if not recording and open
  else if(file_closed == false)
  {
    output.close();
  }
  */
  
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
        dataOutput += String(analogRead(A6)) + "\n";
      }
      //dataOutput += "END\n";
      
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
    //TODO:
    //preset to turn off all solonoids (except release?)
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
  client.print(response);
}


// Changes record state and opens/closes file
void record_command(YunClient client)
{
  if(record)
  {
    record = false;
    //output.close();
    client.println("Done Recording");
  }
  else
  {
    record = true;
    //File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_APPEND);
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
    output.println("Time [ms],A0,A1,A2,A3,A4,A5,A6");
    // close test file
    output.close();
  }
  client.println("Output Cleared");
}
