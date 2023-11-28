

#include <Arduino.h>
#include <ModbusMaster.h>
#include <Notecard.h>



#define MAX485_DE      D13
#define MAX485_RE_NEG  D11
#define usbSerial Serial



#define Battery_Voltage_Address 0x3046
#define Battery_Current_Address 0X3047
#define Load_Voltage_Address 0x304A
#define Load_Current_Address 0x304B
#define Solar_Voltage_Address 0x304E
#define Solar_Current_Address 0x304F
#define Temperature_Address 0x30A2
#define Daily_Charging_Capacity_Address 0x3052 
#define Total_Charging_Capacity 0x3053
#define Controller_fun_stat3 0x902B



struct ModbusData {
  float batteryVoltage;
  float batteryCurrent;
  float loadVoltage;
  float loadCurrent;
  float solarVoltage;
  float solarCurrent;
  float temperature;
  float dailyChargingCapacity;
  float totalChargingCapacity;
};

// instantiate ModbusMaster object
ModbusMaster node;

Notecard notecard;
#define productUID "com.blues.voltaic_batterycont"
double freq = 5;
double Load = 0;
#define interruptAttnPin D9


void NoteAdd (ModbusData data){

      J *req = NoteNewRequest("note.add");
      JAddStringToObject(req, "file", "batteryCont.qo");
      JAddBoolToObject(req, "sync", true);

      J *body = JCreateObject();
      JAddNumberToObject(body, "BatV", data.batteryVoltage/100 );
      JAddNumberToObject(body, "BatA", data.batteryCurrent/100 );
      JAddNumberToObject(body, "LoadV", data.loadVoltage/100 );
      JAddNumberToObject(body, "LoadA", data.loadCurrent/100 );
      JAddNumberToObject(body, "SolarV", data.solarVoltage/100 );
      JAddNumberToObject(body, "SolarA", data.solarCurrent/100 );
      JAddNumberToObject(body, "Temp", data.temperature/100 );
      JAddNumberToObject(body, "DCC", data.dailyChargingCapacity/100);
      JAddNumberToObject(body, "TCC", data.totalChargingCapacity/100 );
      JAddItemToObject(req, "body", body);


      NoteRequest(req);

}

double GetEnvVar(){

J *req = NoteNewRequest("env.get");

J *names = JAddArrayToObject(req, "names");
JAddItemToArray(names, JCreateString("freq"));
JAddItemToArray(names, JCreateString("Load"));

J *rsp = notecard.requestAndResponse(req);
if (rsp != NULL){
   // usbSerial.print(J *rsp);
    J *body = JGetObject(rsp, "body");
    freq = atof(JGetString(body, "freq"));
    Load = atof(JGetString(body, "Load"));
    notecard.deleteResponse(rsp);
  }

  usbSerial.println("New Frequency= " + String(freq) + " Seconds");
  usbSerial.println("Load = " + String(Load));

  return freq;
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

void notecardInit(){

  notecard.begin();
  notecard.setDebugOutputStream(usbSerial);
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


  attnArm();
  freq = GetEnvVar(); 

}

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setLoad(){

    if (Load == 1){
    Serial.println("Lumiax Output ON");
    // Write the value to the coil
    node.writeSingleCoil(0x0000, 1);
    delay(1000);  // Delay for stability, adjust as needed
  }
  else{
    Serial.println("Lumiax Output OFF");
    // Write the value to the coil
    node.writeSingleCoil(0x0000, 0);
    //delay(1000);  // Delay for stability, adjust as needed
  }
}

int readModbusRegister(uint16_t regAddress)
{
  uint8_t result;
  uint16_t data;
  int value = -1;
  int numRead = 1;

  Serial.print("Reading Modbus Register at address: 0x");
  Serial.print(regAddress, HEX);

  result = node.readInputRegisters(regAddress, numRead);

  if (result == node.ku8MBSuccess) {
    Serial.print("   Response Bytes: ");
    for (uint8_t i = 0; i < numRead; i++) {
      Serial.print(node.getResponseBuffer(i), HEX);
      Serial.print(" ");
    }
    Serial.println();

    //Serial.print("Response Dec: ");
    for (uint8_t i = 0; i < numRead; i++) {
      //Serial.print(node.getResponseBuffer(i), DEC);
      //Serial.print(" ");
    }
    //Serial.println();

    data = node.getResponseBuffer(0);

    //Serial.print("Input Register Value: ");
    value = data;
    //Serial.println(data);
  } else {
    Serial.print("Error reading input register " + String(regAddress) +  ". Error code: ");
    Serial.println(result, HEX);
  }

  delay(1000);

  return value;
}


ModbusData  GetData(){

  ModbusData data;

  data.batteryVoltage = readModbusRegister(Battery_Voltage_Address);
  data.batteryCurrent = 0xFFFF - readModbusRegister(Battery_Current_Address);
  data.loadVoltage = readModbusRegister(Load_Voltage_Address);
  data.loadCurrent = readModbusRegister(Load_Current_Address);
  data.solarVoltage = readModbusRegister(Solar_Voltage_Address);
  data.solarCurrent = readModbusRegister(Solar_Current_Address);
  data.temperature = readModbusRegister(Temperature_Address);
  data.dailyChargingCapacity = readModbusRegister(Daily_Charging_Capacity_Address);
  data.totalChargingCapacity = readModbusRegister(Total_Charging_Capacity);

  setLoad();

  Serial.println("-----------------------------------------------------");
  Serial.println("Battery Voltage: " + String(data.batteryVoltage/100) + "V" );
  Serial.println("Battery Current: " + String(data.batteryCurrent/100) + "A" );
  Serial.println("Load Voltage: " + String(data.loadVoltage/100) + "V" );
  Serial.println("Load Current: " + String(data.loadCurrent/100) + "A" );
  Serial.println("Solar Voltage: " + String(data.solarVoltage/100) + "V" );
  Serial.println("Solar Current: " + String(data.solarCurrent/100) + "A" );
  Serial.println("Temperature: " + String(data.temperature/100) + "C" );
  Serial.println("Daily Charging Capacity: " + String(data.dailyChargingCapacity/100) + "kWh" );
  Serial.println("Total Charging Capacity: " + String(data.totalChargingCapacity/100) + "kWh" );
  Serial.println("-----------------------------------------------------");

  return data;
}


void AttnPinInterrupt(){
  detachInterrupt(digitalPinToInterrupt(interruptAttnPin));
  usbSerial.println("///////ATTENTION PIN INTERRUPT//////////"); 
  attnDisarm();
  freq = GetEnvVar();
  attnArm(); 
  setLoad(); 
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(interruptAttnPin), AttnPinInterrupt, RISING);

}

void Lumiax_init(){

       Serial.println("Setting Lumiax to Manual Mode");
       uint8_t result = node.writeSingleRegister(Controller_fun_stat3, 0x0A);
       if (result == node.ku8MBSuccess) {
        Serial.println("Write success!");
        } else {
        Serial.print("Write failed! Error: ");
        Serial.println(result, HEX);
        }
        setLoad();


}
void setup()
{
  
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  // Modbus communication runs at 115200 baud
  Serial.begin(9600);
  Serial1.begin(9600);
  
  notecardInit();
  // Modbus slave ID 1
  node.begin(1, Serial1);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  pinMode(interruptAttnPin, INPUT_PULLDOWN); // Set D1 as an input with a pull-up resistor
  attachInterrupt(digitalPinToInterrupt(interruptAttnPin), AttnPinInterrupt, RISING);
  Lumiax_init();
  GetEnvVar();

}



void loop()
{
     ModbusData data;
     uint16_t CFS3 = node.readInputRegisters(Controller_fun_stat3 , 1);
     Serial.println("CFS3 read");
     Serial.println(CFS3, BIN);
     data = GetData();
     //NoteAdd(data);
     Serial.println("Waiting " + String(freq) + " seconds");
     delay(freq*1000);


}
