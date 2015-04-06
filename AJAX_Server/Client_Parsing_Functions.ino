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
}


void burn_command(YunClient client)
{
  digitalWrite(sol2, HIGH);  //open Ox Flow
  delay(200);  //Delay .2 seconds before opening Ox flow
  digitalWrite(sol3, HIGH);  //open Ox Flow
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
    client.println("Data Recording Stopped");
  }
  else
  {
    record = true;
    calibrate_ADC();
    
    File output = FileSystem.open("/mnt/sd/arduino/www/output.csv", FILE_APPEND);
    if(output)
    {
      // Read initial values
      rTimeStamp = millis();
      rA0 = analogRead(A0);
      rA1 = analogRead(A1);
      rA2  = analogRead(A2);
      rA3 = analogRead(A3);
      rA4 = analogRead(A4);
      rA5 = analogRead(A5);
      rA11 = analogRead(A11);
      rLoad = read_ADC();
      
      // Put initial values in string
      String dataOutput = "New Record\n0,";           // Time starts at 0 for each new recording
      dataOutput += String(rA0) + ",";
      dataOutput += String(rA1) + ",";
      dataOutput += String(rA2) + ",";
      dataOutput += String(rA3) + ",";
      dataOutput += String(rA4) + ",";
      dataOutput += String(rA5) + ",";
      dataOutput += String(rA11) + ",";
      dataOutput += String(rLoad) + "\n";
      
      // Write to SD Card
      output.print(dataOutput);
      output.close();
    }
    
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
