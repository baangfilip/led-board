#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI
#include <SoftwareSerial.h>

#define IS31FL3731_ADDR   116              // 7 bit I2C address 


int counterX = 0;
int counterY = 0;
int counterEights = 1;
int counter = 8;
int pageNine = 11;

SoftwareSerial mySerial(3,4);  //rx, tx

void setup(){
  mySerial.begin(9600);
  mySerial.println("Setup");
  TinyWireM.begin();                    // initialize I2C lib
  SetupIS31FL3731();
  mySerial.println("Setup complete");
}


void loop(){
  if(counter > 144){
    counterEights = 1;
    counter = 8;
    delay(1000);  
    setPWRToEachLED(0);
  }
  setPowerOnLed(counter);
  counterEights++;
  counter++;
  if(counterEights > 8){
    counterEights = 1;
    counter += 8;
  }
  delay(100);                         // wait a half sec before next light
}


int setFunctionRegisterSetting(int adress, int value){

	GoToPage(pageNine);
  TinyWireM.beginTransmission(IS31FL3731_ADDR);
  uint8_t data[2];
  data[0] = adress;
  data[1] = value;
  TinyWireM.write(data, 2);                 // Access Command Register    
  int res = TinyWireM.endTransmission();          
  mySerial.print("result: ");      
  mySerial.println(res);
  return res;
}
int setLedONForEachFrame(){
  mySerial.println("set all led to ON");
  int res;
  for (int h = 0; h < 8; h++) {
    // each frame
    for (int k = 0; k <= 17; k++) {
      //each 8 bit value
	    GoToPage(h);
      TinyWireM.beginTransmission(IS31FL3731_ADDR);
      uint8_t data[2];
      data[0] = k;
      data[1] = 0xFF;
      TinyWireM.write(data, 2);                 
      res = TinyWireM.endTransmission();   
      if(res != 0){
        return res;
      }            
    }
  }
  return res;
}

int setPWRToEachLED(int power){
  mySerial.println("set power to each led power: ");
  mySerial.println(power);
  int res;
  for (int h = 0; h < 8; h++) {
    // each frame
    for (int k = 36; k <= 179; k++) {
      //each 8 bit value
	    GoToPage(h);
      TinyWireM.beginTransmission(IS31FL3731_ADDR);
      uint8_t data[2];
      data[0] = k;
      data[1] = power;
      TinyWireM.write(data, 2);                 
      res = TinyWireM.endTransmission();   
      if(res != 0){
        return res;
      }      
    }
  }
  return res;
}

int shutdownAddress = 0x0A;
int shutdownModeValue = 0x00;
int normalOperationModeValue = 0x01;
int modeAddress = 0x00;
int pictureMode = 0x00;
void SetupIS31FL3731(){// Setup the IS31FL3731 
  mySerial.println("S set shutdown mode ");
	int res = setFunctionRegisterSetting(shutdownAddress, shutdownModeValue); // set shutdown mode
  // mySerial.print("D set shutdown mode ");
  mySerial.println(res);
  
  mySerial.println("delay 1000");
	delay(1000);

  mySerial.println("S set normal operation mode ");
	res = setFunctionRegisterSetting(shutdownAddress, normalOperationModeValue);
  // mySerial.print("D set normal operation mode ");
  mySerial.println(res);

  // mySerial.print("set picture mode ");
	res = setFunctionRegisterSetting(modeAddress, pictureMode);
  mySerial.println(res);

  // mySerial.print("turn all leds on ");
	res = setLedONForEachFrame();
  mySerial.println(res);

  // mySerial.print("set power 0x0F to all leds ");
	res = setPWRToEachLED(0x0F);
  mySerial.println(res);

  // mySerial.print("set audio sync off ");
	res = setFunctionRegisterSetting(0x06, 0x00);
  mySerial.println(res);

  mySerial.println("delay 1000");
  delay(1000);
  // mySerial.print("set power to 0 for all leds");
	res = setPWRToEachLED(0);
  mySerial.println(res);
}

void GoToPage(int page){ // Go to page IS31FL3731 
  // mySerial.println("go to page: ");
  // mySerial.println(page);
  TinyWireM.beginTransmission(IS31FL3731_ADDR);
  uint8_t data[2];
  data[0] = 253;
  data[1] = page;
  TinyWireM.write(data, 2);          
  int res = TinyWireM.endTransmission();   
  // mySerial.print("go to page res: ");
  // mySerial.println(res);  
}

void setPowerOnLed(int ledNumber) {
  ledNumber = ledNumber + 36;
  for (int h = 0; h < 8; h++) {
    GoToPage(h);
    mySerial.println(ledNumber);
    TinyWireM.beginTransmission(IS31FL3731_ADDR);
    uint8_t data[2];
    data[0] = ledNumber; 
    data[1] = 15;
    TinyWireM.write(data, 2);  
    int res = TinyWireM.endTransmission();          // Send 1 byte to the slave
    mySerial.println(res);
  }
}
void drawPixel(int x, int y){  
  if (x > 7) {
    x = 15 - x;
    y += 8;
  } else {
    y = 7 - y;
  }
  int t = x;
	x = y;
	y = t;
  int ledNumber = x + y * 16;
  ledNumber = ledNumber + 36;
  
  for (int h = 0; h < 8; h++) {
    GoToPage(h);
    mySerial.println(ledNumber);
    TinyWireM.beginTransmission(IS31FL3731_ADDR);
    uint8_t data[2];
    data[0] = ledNumber; 
    data[1] = 15;
    TinyWireM.write(data, 2);  
    int res = TinyWireM.endTransmission();          // Send 1 byte to the slave
    mySerial.println(res);
  }
}
