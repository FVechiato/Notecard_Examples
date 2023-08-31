#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Notecard.h>
#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_SCD4x_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_SCD4x
#include <ArtronShop_SHT45.h>


#define productUID "com.blues.ffreitas:terpene"
#define usbSerial Serial
#define gasSensorSerial Serial1

ArtronShop_SHT45 sht45(&Wire, 0x44); // SHT45-AD1B => 0x44
SCD4x CO2Sensor;


#define D5 5
#define D10 10
#define DHTPIN 6        // Replace '2' with the actual pin number you are using to connect the DHT22 sensor
#define DHTTYPE DHT22   // DHT22 (AM2302) sensor type

DHT dht(DHTPIN, DHTTYPE);
Notecard notecard;
double freq = 0;

void NoteAdd (double temperature, double humidity, double ppm, double co2){

      J *req = NoteNewRequest("note.add");
      JAddStringToObject(req, "file", "terpene.qo");
      JAddBoolToObject(req, "sync", true);

      J *body = JCreateObject();
      JAddNumberToObject(body, "temp", temperature);
      JAddNumberToObject(body, "humi", humidity);
      JAddNumberToObject(body, "Ethylene", ppm);
      JAddNumberToObject(body, "CO2", co2);
      JAddItemToObject(req, "body", body);

      NoteRequest(req);

}


void TempHumi(float* temperature, float* humidity) {

  delay(2000);   // Delay between readings (adjust as needed)

  if (sht45.measure()) {
  // Read temperature and humidity from the sensor
  *temperature = sht45.temperature();
  *humidity = sht45.humidity();

  } else {
    Serial.println("SHT45 read error");
    *temperature = -1;
    *humidity = -1;
  }

  // Check if any reading failed
  if (isnan(*temperature) || isnan(*humidity)) {
    usbSerial.println("Failed to read from DHT sensor!");
  }
}


double GetFreq(){

J *req = NoteNewRequest("env.get");
JAddStringToObject(req, "name", "Freq");

double freq = 0.0;
J *rsp = notecard.requestAndResponse(req);
if (rsp != NULL){
   // usbSerial.print(J *rsp);
    freq = atof(JGetString(rsp, "text"));
    notecard.deleteResponse(rsp);
  }
return freq;
}

uint8_t FucCheckSum(const uint8_t* data, size_t size) {
    uint8_t tempq = 0xFF;

    for (size_t i = 0; i < size - 1; i++) {
        tempq += data[i];
    }

    tempq = (~tempq) & 0xFF;

    return tempq;
}

void EthyleneRead(int* ppm) {

    int maxtry = 8;
    *ppm= -1;
    while (maxtry > 0) {
        byte incomingByte = gasSensorSerial.read();
        maxtry--;
        if (incomingByte == 0xFF) {
            break;
        }
    }
    if (maxtry != 0){
          uint8_t data[8];
          usbSerial.print("Ethylene read:");
          for (size_t i = 0; i < 8; i++) {
              data[i] = gasSensorSerial.read();
              usbSerial.print("| 0x");
              usbSerial.print(data[i], HEX);
              usbSerial.print(" |");
              

          }
          usbSerial.println("");
          uint8_t checksum = FucCheckSum(data, 8);
          usbSerial.print("Checksum: 0x");
          usbSerial.println(checksum, HEX);

          String sensorRead = "No Valid data";
        /// Check if gas read is Ethylene (first byte = 0x08)
        if(data[0]== 0x08){
        /// Confirm checksum  
          if (checksum == data[7]) {
              sensorRead = "";
              *ppm = (data[3] * 256 + data[4]);

              if (data[2] == 0x01) {
                  *ppm = *ppm / 10;
              }

          }
        }
    }
}
void attnArm(){

J *req = NoteNewRequest("card.attn");
JAddStringToObject(req, "mode", "arm,env");

NoteRequest(req);

}

void attnDisarm(){

J *req = NoteNewRequest("card.attn");
JAddStringToObject(req, "mode", "disarm,-all");

NoteRequest(req);


}

void blink(int miliseconds, int times){
  int i = 0;
  usbSerial.println("Blinking for  " + String(2*times*miliseconds/1000) + " seconds");
  while (i < times)
  {
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(miliseconds);                     // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(miliseconds);    
  i++;

  }
                // wait for a second
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  delay(2500);
  usbSerial.begin(115200);
  gasSensorSerial.begin(9600);
  dht.begin();
  notecard.begin();
  notecard.setDebugOutputStream(usbSerial);
  
  while (!sht45.begin()) {
    Serial.println("SHT45 not found ! Freezing...");
    blink(100,3);
    delay(1000);
  }
  while (CO2Sensor.begin() == false)
  {
    Serial.println(F("Sensor not detected. Please check wiring. Freezing..."));
    blink(100,3);
    delay(1000);
  }

  J *req = notecard.newRequest("hub.set");
  JAddStringToObject(req, "product", productUID);
  JAddStringToObject(req, "mode", "continuous");
  JAddBoolToObject(req,"sync",true);
  notecard.sendRequestWithRetry(req, 5);

  pinMode(D5, INPUT);
  pinMode(D10, OUTPUT);
  attnArm();

  freq = GetFreq();   
}

void loop() {

  float temperature; 
  float humidity; 
  int ppm;

  TempHumi(&temperature, &humidity);

  int maxtry = 5;
  ppm = -1;
  while(ppm == -1){

    EthyleneRead(&ppm);
    maxtry--;
    if(maxtry == 0){
      break;
    }

  }



  double temp2 = (temperature*9/5)+32;
  double hum = humidity;
  double ppm2 = ppm;
  double co2;

if (digitalRead(D5) == HIGH) {
   
    attnDisarm();
    freq = GetFreq();
    attnArm();  
    usbSerial.println("///////ATTENTION INTERRUPT//////////");  
  } 
 

  if (CO2Sensor.readMeasurement()) // readMeasurement will return true when fresh data is available
  {
     co2=CO2Sensor.getCO2();
  }

  NoteAdd(temp2,hum,ppm2,co2);
  usbSerial.println("Temperature: " + String(temperature) + " °F Humidity: " + String(humidity) + " %, CO2: " + String(co2) + "ppm and Ethylene Concentration: " + String(ppm) + "ppm");

  usbSerial.println("Waiting " + String(freq) + " Seconds...");

  blink(1000,freq/2);


}
