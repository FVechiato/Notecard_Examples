#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Notecard.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h> 

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

struct NotecardDiganostics{

  char devID[20];
  char sn[20];
  double voltage;
  char rat[20];
  char mode[20];
  bool conectionStatus;
  int bars;

};

void notecardInit(){
//Standard Pins: SDA_PIN = 8 SDL_PIN = 9 
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

  if(data.conectionStatus == 1){
      Serial.println("Status:Connected");
      NeopixelColor("GREEN");
  }
  else{
      Serial.println("Status:Disconnected");
      NeopixelColor("RED");
  }

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

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  notecardInit();

  // Create tasks
  xTaskCreatePinnedToCore(task1Function, "Task1", 4096, NULL, 1, &task1Handle, 0);
  xTaskCreatePinnedToCore(task2Function, "Task2", 4096, NULL, 1, &task2Handle, 1);
  xTaskCreatePinnedToCore(NotecardMonitor, "Task3", 4096, NULL, 1, &task3Handle, 1);
}

void loop() {
  // Empty, as tasks are doing the work
}
