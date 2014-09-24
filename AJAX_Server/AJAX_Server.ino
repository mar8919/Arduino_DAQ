#include <FileIO.h>
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

const int ledPin = 13; // the pin that the LED is attached to
int incomingByte;      // a variable to read incoming serial data into
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
  
  // initialize file
  FileSystem.begin();
  File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_APPEND); 
  //TODO: Make append for read system, Add delete button?

  // test write to file
  if(output)
  {
    output.println(String(millis()) + ",Sketch Starting");
    // close test file
    output.close();
  }
  else
  {
    digitalWrite(ledPin, HIGH);  //LED 13 turns on to indicate file error
  }
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
  
  //delay(10);
}



void process(YunClient client)
{
  String command = client.readStringUntil('/');  // Parse string for first section
  if(command == "stop")
  {
    //TODO:
    //preset to turn off all solonoids (except release?)
  }
  if(command == "digital")
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
      
      
      // record pin request
      File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_APPEND);
      if(output)
      {
        //output.println( String(millis()) + ",Pin " + String(pin) + " set to " + String(value) );
        output.println( String(millis()) + ",Pin " + String(pin) + " is currently " + String(value) );
        output.close();
      }
      else
      {
        digitalWrite(ledPin, HIGH);  //LED 13 turns on to indicate file error
      }
      
      
    }
    //server response
    client.print(pin);
    client.print(",");
    client.println(value);
  }
}
