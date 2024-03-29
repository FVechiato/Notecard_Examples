#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Notecard.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned char LogoBlues [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x07, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x1F, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x7F, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xFF, 0xFF, 0xF0, 0x00, 0x7C, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0xFF, 0x0F, 0xF8, 0x00, 0x7C, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0xF8, 0x01, 0xFC, 0x00, 0x7C, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x07, 0xE0, 0x00, 0x7E, 0x00, 0x7C, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0F, 0xC0, 0x00, 0x3F, 0x00, 0x7C, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0F, 0x80, 0x00, 0x1F, 0x00, 0x7C, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1F, 0x0F, 0xFF, 0x0F, 0x80, 0x7C, 0xFE, 0x01, 0xF1, 0xF8, 0x3F, 0x00, 0xFE, 0x00, 0x3F, 0x80,
0x1F, 0x0F, 0xFF, 0x0F, 0x80, 0x7D, 0xFF, 0x81, 0xF1, 0xF8, 0x3F, 0x03, 0xFF, 0x80, 0xFF, 0xC0,
0x1F, 0x0F, 0xFF, 0x87, 0x80, 0x7F, 0xFF, 0xC1, 0xF1, 0xF8, 0x3F, 0x07, 0xFF, 0xC1, 0xFF, 0xE0,
0x3E, 0x0F, 0xFF, 0x87, 0xC0, 0x7F, 0xFF, 0xE1, 0xF1, 0xF8, 0x3F, 0x0F, 0xFF, 0xE1, 0xFF, 0xF0,
0x3E, 0x0F, 0xFF, 0x87, 0xC0, 0x7F, 0xFF, 0xE1, 0xF1, 0xF8, 0x3F, 0x1F, 0xC7, 0xF3, 0xF1, 0xF0,
0x3E, 0x0F, 0x0F, 0x87, 0xC0, 0x7F, 0x07, 0xF1, 0xF1, 0xF8, 0x3F, 0x1F, 0x01, 0xF3, 0xF0, 0x00,
0x3E, 0x0F, 0x0F, 0x87, 0xC0, 0x7E, 0x03, 0xF1, 0xF1, 0xF8, 0x3F, 0x3F, 0x01, 0xFB, 0xF8, 0x00,
0x3E, 0x0F, 0x0F, 0x87, 0xC0, 0x7C, 0x01, 0xF1, 0xF1, 0xF8, 0x3F, 0x3F, 0xFF, 0xF9, 0xFE, 0x00,
0x3E, 0x0F, 0xFF, 0x87, 0xC0, 0x7C, 0x01, 0xF1, 0xF1, 0xF8, 0x3F, 0x3F, 0xFF, 0xF9, 0xFF, 0xC0,
0x3E, 0x0F, 0xFF, 0x87, 0xC0, 0x7C, 0x01, 0xF1, 0xF1, 0xF8, 0x3F, 0x3F, 0xFF, 0xF8, 0x7F, 0xF0,
0x1F, 0x0F, 0xFF, 0x87, 0x80, 0x7C, 0x01, 0xF1, 0xF1, 0xF8, 0x3F, 0x3F, 0xFF, 0xF8, 0x1F, 0xF0,
0x1F, 0x0F, 0xFF, 0x0F, 0x80, 0x7E, 0x03, 0xF1, 0xF1, 0xF8, 0x3F, 0x3F, 0x00, 0x00, 0x03, 0xF8,
0x1F, 0x0F, 0xFF, 0x0F, 0x80, 0x7E, 0x03, 0xF1, 0xF1, 0xF8, 0x3F, 0x1F, 0x00, 0x00, 0x01, 0xF8,
0x0F, 0x80, 0x00, 0x1F, 0x00, 0x7F, 0x8F, 0xF1, 0xF1, 0xFC, 0x7F, 0x1F, 0x83, 0xF3, 0xF1, 0xF8,
0x0F, 0xC0, 0x00, 0x3F, 0x00, 0x7F, 0xFF, 0xE1, 0xF0, 0xFF, 0xFF, 0x0F, 0xFF, 0xF3, 0xFF, 0xF0,
0x07, 0xE0, 0x00, 0x7E, 0x00, 0x7F, 0xFF, 0xC1, 0xF0, 0xFF, 0xFF, 0x0F, 0xFF, 0xE1, 0xFF, 0xF0,
0x03, 0xF8, 0x01, 0xFC, 0x00, 0x7F, 0xFF, 0xC1, 0xF0, 0x7F, 0xFF, 0x07, 0xFF, 0xC0, 0xFF, 0xE0,
0x01, 0xFF, 0x0F, 0xF8, 0x00, 0x7D, 0xFF, 0x01, 0xF0, 0x3F, 0xDF, 0x01, 0xFF, 0x00, 0x7F, 0xC0,
0x00, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x7C, 0x00, 0x1F, 0x00,
0x00, 0x7F, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x1F, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define RX_PIN 36  // Replace with the actual RX pin number
#define TX_PIN 37  // Replace with the actual TX pin number
#define productUID "com.blues.ffreitas:test123"

Notecard notecard;

//Neopixel
#define PIN 48
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


//Neopixel



// Task handles
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;
TaskHandle_t task3Handle = NULL;
const TickType_t watchdogTimeout = 10000 / portTICK_PERIOD_MS;

struct NotecardDiganostics{

  char devID[20];
  char sn[20];
  double voltage;
  char rat[20];
  char mode[20];
  bool conectionStatus;
  int bars;

};
void displayInit(){

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.clearDisplay();
  display.drawBitmap(0, 15, LogoBlues, 128, 64, WHITE);
  display.display();
  //delay(5000); // Pause for 2 seconds

  // Clear the buffer
  //display.clearDisplay();
  
}

void notecardInit(){
  //Changing I2C pins -- Standard Pins: SDA_PIN = 8 SDL_PIN = 9 
  Wire.setPins(35,36);
  notecard.begin();
  notecard.setDebugOutputStream(Serial);
// read the hub.set and only set it if necessary 
// add a second prodctUID var

  J *req = notecard.newRequest("hub.set");
  JAddStringToObject(req, "product", productUID);
  JAddStringToObject(req, "mode", "continuous");
  JAddBoolToObject(req,"sync",true);
  notecard.sendRequestWithRetry(req, 5);

  req = notecard.newRequest("card.triangulate");
  JAddStringToObject(req, "mode", "cell");
  JAddBoolToObject(req, "on", true);
  JAddBoolToObject(req, "set", true);
  notecard.sendRequestWithRetry(req, 5);


  //attnArm();
  //freq = GetEnvVar(); 

}

void NeopixelColor(String color) {
  Serial.println("Neopixel set to " + color);
  if (color == "RED") {
    pixels.setPixelColor(0, pixels.Color(10, 0, 0));
  } else if (color == "GREEN") {
    pixels.setPixelColor(0, pixels.Color(0, 10, 0));
  } else if (color == "BLUE") {
    pixels.setPixelColor(0, pixels.Color(0, 0, 10));
  }

  pixels.show();

}

NotecardDiganostics Notecarddiag(){

  NotecardDiganostics data;

  if (J *req = notecard.newRequest("card.status")) {
    J *rsp = notecard.requestAndResponse(req);
    notecard.logDebug(JConvertToJSONString(rsp));

    data.conectionStatus = JGetBool(rsp, "connected");

    notecard.deleteResponse(rsp);
  }
  if (J *req = notecard.newRequest("card.voltage")) {
      J *rsp = notecard.requestAndResponse(req);
      notecard.logDebug(JConvertToJSONString(rsp));

      data.voltage = JGetNumber(rsp, "value");

      notecard.deleteResponse(rsp);
  }
 if (J *req = notecard.newRequest("hub.get")) {
    J *rsp = notecard.requestAndResponse(req);
    notecard.logDebug(JConvertToJSONString(rsp));
    char* tempVar  = JGetString(rsp, "device");
    strlcpy(data.devID, tempVar, sizeof(data.devID));
    tempVar  = JGetString(rsp, "sn");
    strlcpy(data.sn, tempVar, sizeof(data.sn));
    tempVar = JGetString(rsp, "mode");
    strlcpy(data.mode, tempVar, sizeof(data.mode));

    notecard.deleteResponse(rsp);
 }
 if (J *req = notecard.newRequest("card.wireless")) {
    J *rsp = notecard.requestAndResponse(req);
    notecard.logDebug(JConvertToJSONString(rsp));
    // storing the oject net before assing the value
    J *net = JGetObject(rsp, "net");
    data.bars  = JGetInt(net, "bars");
    char *tempVar = JGetString(net, "rat");
    strlcpy(data.rat, tempVar, sizeof(data.rat));  
    notecard.deleteResponse(rsp);
 }

  Serial.println("devID= " + String(data.devID));
  Serial.println("sn: " + String(data.sn));
  Serial.println("V: " + String(data.voltage) + "V");
  Serial.println("Rat: " + String(data.rat));
  Serial.println("dataConnection: " + String(data.conectionStatus) );
  Serial.println("bars: " + String(data.bars));
  Serial.println("mode= " + String(data.mode));


  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println("Notecard info:");
  display.println(String(data.devID));
  display.println("sn:" + String(data.sn));
  display.println("V:" + String(data.voltage) + "V Rat:" + String(data.rat));

  if(data.conectionStatus == 1){
      Serial.println("Status:Connected");
      NeopixelColor("GREEN");
      display.println("Status:Connected");
  }
  else{
      Serial.println("Status:Disconnected");
      NeopixelColor("RED");
      display.println("Status:Disconnected");
  }

  display.println("bars: " + String(data.bars));
  display.println("mode:" + String(data.mode));

  display.display();



  return data;
}

// Task functions
void task1Function(void *parameter) {
  (void)parameter; // Unused parameter

  while (1) {
    Serial.println("Task 1 is running...");

  
    vTaskDelay(1000 / portTICK_PERIOD_MS);

  }
}

void task2Function(void *parameter) {
  (void)parameter; // Unused parameter

  while (1) {
    Serial.println("Task 2 is running...");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

void NotecardMonitor (void *parameter) {
  (void)parameter; // Unused parameter

  while (1) {
    Serial.println("Task 3 is running...");
    Notecarddiag();
    
    if (xTaskCheckForTimeOut(&task3Handle, watchdogTimeout)) {
    Serial.println("Task watchdog timeout - resetting system");
    // Reset the system
    ESP.restart();
}
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  notecardInit();
  displayInit();

  // Create tasks
  xTaskCreatePinnedToCore(task1Function, "Task1", 4096, NULL, 1, &task1Handle, 0);
  xTaskCreatePinnedToCore(task2Function, "Task2", 4096, NULL, 1, &task2Handle, 1);
  xTaskCreatePinnedToCore(NotecardMonitor, "Task3", 4096, NULL, 1, &task3Handle, 1);
}

void loop() {
  // Empty, as tasks are doing the work
}
