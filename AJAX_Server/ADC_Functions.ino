const byte RESET = 0xFF;
const byte ON = 0x80;
const byte READ_ONLY = 0x00;
const byte WAIT = 75;
const byte GAIN = 128;

const byte WRITE_CONFIG = 0b00010000;
//Bipolar Configurations
const byte CONFIG[3] = {0b10000000, 0b00000001, 0b00010111};  // Test Configuration (Gain = 128)
//const byte CONFIG[3] = {0b10000000, 0b00000001, 0b00010000};  // Modified Configuration: Gain = 1
//const byte CONFIG[3] = {0b10000000, 0b00000001, 0b00010001};  // Modified Configuration: Gain = 2

//Unipolar Configurations
//const byte CONFIG[3] = {0b10010000, 0b00000001, 0b00010000};  // Modified Configuration: Gain = 1, Reference V monopolar
//const byte CONFIG[3] = {0b10010000, 0b00000001, 0b00010001};  // Modified Configuration: Gain = 2, Reference V monopolar
//const byte CONFIG[3] = {0b10010000, 0b00000001, 0b00010111};  // Modified Configuration: Gain = 128, Reference V monopolar


const byte WRITE_MODE  = 0b00001000;
//const byte MODE[3] = {0b00101000, 0b00000100, 0b01010000};    // Test Mode

const byte MODE[3] = {0b00001000, 0b00000100, 0b01010000};    // Modified Mode: Continuous Conversion Mode
//const byte MODE[3] = {0b00001000, 0b00000111, 0xFF};    // Modified Mode: Continuous Conversion Mode and Filter Word of 1023


const byte CALIBRATE_MODE[3] = {0b11001000, 0b00000111, 0xFF};    // Modified Mode: Continuous Conversion Mode and Filter Word of 1023

const byte EX_MODE[3] = {0x28, 0x00, 0x60};

const byte READ_MODE   = 0b01001000;
const byte READ_CONFIG = 0b01010000;
const byte READ_STATUS = 0b01000000;

//Used to hold bytes of input
byte in[3];
long output;
float voltage, weight;

float read_ADC()
{
  boolean data_rdy = ADC_ready();
  // Busy wait for ADC
  while(!data_rdy)
  {
    data_rdy = ADC_ready();
    delay(1);
  }
  
  // Read Data
  SPI.transfer(0x58);
  output = 0;
  for(int i = 0; i < 3; i++)
  {
    output = (output << 8) + SPI.transfer(READ_ONLY);  //Read in 3 bytes
  }
  
  voltage = (output * (3.75 / 8388607.0 ) - 3.75) * (1000 / GAIN);
  weight = voltage * 151.5;
  
  //Debug serial output
  /*
  Serial.println(output, DEC);
  Serial.println( String(voltage) + " mV" );
  Serial.println( String(voltage * 1000) + " microV" );
  */
  
  //Serial.println( String(weight) + " lbs");
  return weight;
}


boolean ADC_ready()
{
   SPI.transfer(READ_STATUS);
   byte sr = SPI.transfer(READ_ONLY);
   //Serial.println("  Status Register: " + String(sr, BIN));
   if(sr & 0x80)
   {
     //Serial.println("    ADC Not ready");
     return false;  // Not RDY bit set
   }
   return true;
}



void ADC_Setup()
{
  for(int i = 0; i < 5; i++)
  {
    SPI.transfer(RESET);  //total of 40 one's to reset ADC mode
  }
  delay(5);
  
  // Write to Configuration Register
  SPI.transfer(WRITE_CONFIG);
  for(int i = 0; i < 3; i++)
  {
    SPI.transfer(CONFIG[i]);
  }
  
  calibrate_ADC();
  
  //SPI.transfer(0b01011100);  // Continuous Conversion Mode
}


void calibrate_ADC()
{
  // Zero-Scale Calibration
  SPI.transfer(WRITE_MODE);
  for(int i = 0; i < 3; i++)
  {
    SPI.transfer(CALIBRATE_MODE[i]);
  }
  
  // Wait for calibration to complete
  boolean data_rdy = ADC_ready();
  while(!data_rdy)
  {
    data_rdy = ADC_ready();
    delay(1);
  }
  
  // Write to Mode Register
  SPI.transfer(WRITE_MODE);
  for(int i = 0; i < 3; i++)
  {
    SPI.transfer(MODE[i]);
  }
  
  // Settling time
  delay(200);
}

/*
void check_settings()
{
  Serial.println("Config:");
  // Read configuration register over SPI
  SPI.transfer(READ_CONFIG);
  for(int i = 0; i < 3; i++) in[i] = SPI.transfer(READ_ONLY);
  
  // Check read configuration register
  for(int i = 0; i < 3; i++)
  {
    if(in[i] == CONFIG[i]) Serial.println("  Match");
    else Serial.println("  Different");
  }
  
  
  Serial.println("Mode:");
  SPI.transfer(READ_MODE);
  for(int i = 0; i < 3; i++) in[i] = SPI.transfer(READ_ONLY);
  
  // Check read configuration register
  for(int i = 0; i < 3; i++)
  {
    if(in[i] == MODE[i]) Serial.println("  Match");
    else
    {
      Serial.print("  Should be: ");
      Serial.println(MODE[i], BIN);
      Serial.print("        Got: ");
      Serial.println(in[i], BIN);
    }
  }
}
*/
