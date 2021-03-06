void process(YunClient client)
{
  String command = client.readStringUntil('/');  // Parse string for first section
  // remove newlines
  command.replace("\n", "");
  command.replace("\r", "");
  //Serial.println(command);
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
  event_log += String(millis() - record_start_time) + ",Stop\n";
}


void burn_command(YunClient client)
{
  digitalWrite(sol2, HIGH);  //open Ox Flow
  delay(200);  //Delay .2 seconds before opening Ox flow
  digitalWrite(sol3, HIGH);  //open Ox Flow
  client.println("Ok");
  event_log += String(millis() - record_start_time) + ",Burn\n";
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
  
  // Add to event log if pin turned on
  if(value == 1)
  {
    event_log += String(millis() - record_start_time) + ",Pin " + String(pin) + " on\n";
  }
  else
  {
    event_log += String(millis() - record_start_time) + ",Pin " + String(pin) + " off\n";
  }
}


// Changes record state and opens/closes file
void record_command(YunClient client)
{
  if(record)
  {
    record = false;
    
    File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_APPEND);
    if(output)
    {
      event_log = "\nTime [ms],Event\n" + event_log + "\n";
      output.print(event_log);  // Write to SD Card
      output.close();
    }
    event_log = "";   // Clear event log
    client.println("Data Recording Stopped");
  }
  else
  {
    record = true;
    calibrate_ADC();
    record_start_time = millis();
    client.println("Load cell has been tared.\nNow Recording Data");
  }
}


//Clear output file
void clear_command(YunClient client)
{
  FileSystem.remove("/mnt/sd/arduino/www/output.csv");
  
  File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_WRITE);
  
  if(output)
  {
    output.println("Time [ms],P1,P2,P3,P4,P5,P6,P7,Force [lbs]");
    output.close();
  }
  client.println("Output Cleared");
}
