#include <Arduino.h>
#include <Notecard.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArtronShop_SHT45.h>
#include <Wire.h>
#include <DS18B20.h>
#include "Adafruit_LTR390.h"

Notecard notecard;
#define productUID "com.blues.blustream"

ArtronShop_SHT45 sht45(&Wire, 0x44); // SHT45-AD1B => 0x44
#define usbSerial Serial
#define UVB_PIN PIN_A0

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

Adafruit_LTR390 ltr = Adafruit_LTR390();

#define ONE_WIRE_BUS D12
OneWire oneWire(ONE_WIRE_BUS);
DS18B20 TempSensor(&oneWire);


struct SensorData {
  double DStemp;
  double DStempF;
  double temp;
  double tempF;
  double humidity;
  double uvb;
  int bars;
};

struct NotecardDiganostics{

  char devID[20];
  char sn[20];
  double voltage;
  char rat[20];
  char mode[20];
  bool conectionStatus;
  int bars;

};

int freq = 60;
int TempH = 85;
int TempL = 60;

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

// 'Screenshot 2023-10-19 at 10', 128x64px
const unsigned char PETSMART_Logo [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x60, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x38, 0x70, 0xf9, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x07, 0x38, 0x71, 0x9b, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x37, 0xbf, 0x03, 0xb8, 0xd1, 0x98, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0x36, 0x3f, 0x33, 0xfc, 0xd9, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0xe6, 0x0c, 0x73, 0xfc, 0xf9, 0xf1, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x8c, 0xc3, 0xfc, 0xdd, 0xb1, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x06, 0x0e, 0x0c, 0x7b, 0x64, 0x80, 0x19, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0c, 0x19, 0x20, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x8c, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
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

void NoteAdd (double temperature, double temp2, double humidity, double UVB){

      J *req = NoteNewRequest("note.add");
      JAddStringToObject(req, "file", "Terrarium.qo");
      JAddBoolToObject(req, "sync", true);

      J *body = JCreateObject();
      JAddNumberToObject(body, "temp", temperature);
      JAddNumberToObject(body, "Humi", humidity);
      JAddNumberToObject(body, "temp2", humidity);
      JAddNumberToObject(body, "UVB", UVB);
      JAddItemToObject(req, "body", body);

      NoteRequest(req);

}



SensorData SensorRead() { 

  SensorData data;
  delay(2000);   // Delay between readings (adjust as needed)

  TempSensor.requestTemperatures();
  delay(1000);
  data.DStemp = TempSensor.getTempC();
  data.DStempF = (data.DStemp * 9 / 5) + 32;


  if (sht45.measure()) {
  // Read temperature and humidity from the sensor
  data.temp = sht45.temperature();
  data.tempF= ((data.temp* 9 / 5) + 32);
  data.humidity = sht45.humidity();

  } else {
    usbSerial.println("SHT45 read error");
    data.temp = -1;
    data.humidity = -1;
  }

  // Check if any reading failed
  if (isnan(data.temp) || isnan(data.humidity)) {
    usbSerial.println("Failed to read from DHT sensor!");
  }

   if (J *req = notecard.newRequest("card.wireless")) {
    J *rsp = notecard.requestAndResponse(req);
    notecard.logDebug(JConvertToJSONString(rsp));
    // storing the oject net before assing the value
    J *net = JGetObject(rsp, "net");
    data.bars  = JGetInt(net, "bars");
    notecard.deleteResponse(rsp);
  }


  data.uvb = ltr.readUVS();
  ///UVB read
  usbSerial.println("UVB= " + String(data.uvb));
  
  //if( data.tempF < TempL || data.tempF > TempH){
  //  Alarmqo(data.tempF,data.tdsValue,data.phValue);
  //}
  return data;
}

double GetEnvVar(){

J *req = NoteNewRequest("env.get");

J *names = JAddArrayToObject(req, "names");
JAddItemToArray(names, JCreateString("freq"));
JAddItemToArray(names, JCreateString("TempH"));
JAddItemToArray(names, JCreateString("TempL"));
JAddItemToArray(names, JCreateString("freqT"));


J *rsp = notecard.requestAndResponse(req);
if (rsp != NULL){
   // usbSerial.print(J *rsp);
    J *body = JGetObject(rsp, "body");
    freq = atof(JGetString(body, "freqT"));
    TempH = atof(JGetString(body, "TempH"));
    TempL = atof(JGetString(body, "TempL"));
    notecard.deleteResponse(rsp);
  }

usbSerial.println("New Frequency= " + String(freq) + " Seconds");
usbSerial.println("TempL= " + String(TempL) + " TempH= " + String(TempH));
return freq;
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


  //attnArm();
  freq = GetEnvVar(); 

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

  usbSerial.println("devID= " + String(data.devID));
  usbSerial.println("sn: " + String(data.sn));
  usbSerial.println("V: " + String(data.voltage) + "V");
  usbSerial.println("Rat: " + String(data.rat));
  usbSerial.println("dataConnection: " + String(data.conectionStatus) );
  usbSerial.println("bars: " + String(data.bars));
  usbSerial.println("mode= " + String(data.mode));

  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println("Notecard info:");
  display.println(String(data.devID));
  display.println("sn:" + String(data.sn));
  display.println("V:" + String(data.voltage) + "V");
  display.println("Rat:" + String(data.rat));
  if(data.conectionStatus == 1){
      display.println("Status:Connected");
  }
  else{
      display.println("Status:Disconnected");
  }
  display.println("bars: " + String(data.bars));
  display.println("mode:" + String(data.mode));

  display.display();

  return data;
}


void displayWrite(double temperature, double XYZ, double humidity, double UVB, int bars) {
  int i = 56;
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.drawBitmap(0, 0, PETSMART_Logo, 128, 64, WHITE);
  //display.println(F("*********************"));
  //display.println(F("BLUES Inc. "));
  //display.println(F("*********************"));
  display.setCursor(0,21);             // Start at top-left corner
  display.println("T1= " + String(temperature) + "F T2= " + String(XYZ) + "F");
  display.println("Humidity= " + String(humidity) + "%");
  display.println("UVB= " + String(UVB));
  display.println("Signal:");
  if (freq > 60){
    display.println("Cycle: " + String(int(freq/60))+ " hour");
  } 
  else{
    display.println("Cycle: " + String(int(freq))+ " min");
  }

  int barWidth = 6;
  int barHeight = 7;
  int startX = 43;
  int startY = 45;

  for (int i = 0; i < 5; i++) {
    if (i < bars) {
      display.fillRect(startX + i * (barWidth + 2), startY, barWidth, barHeight, SSD1306_WHITE);
    } else {
      display.drawRect(startX + i * (barWidth + 2), startY, barWidth, barHeight, SSD1306_WHITE);
    }
  }


  display.display();
  delay(2000);
}



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

void ltrInit(){

  if ( ! ltr.begin() ) {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  ltr.setMode(LTR390_MODE_UVS);
  if (ltr.getMode() == LTR390_MODE_ALS) {
    Serial.println("In ALS mode");
  } else {
    Serial.println("In UVS mode");
  }

  ltr.setGain(LTR390_GAIN_9);
  Serial.print("Gain : ");
  switch (ltr.getGain()) {
    case LTR390_GAIN_1: Serial.println(1); break;
    case LTR390_GAIN_3: Serial.println(3); break;
    case LTR390_GAIN_6: Serial.println(6); break;
    case LTR390_GAIN_9: Serial.println(9); break;
    case LTR390_GAIN_18: Serial.println(18); break;
  }

  ltr.setResolution(LTR390_RESOLUTION_16BIT);
  Serial.print("Resolution : ");
  switch (ltr.getResolution()) {
    case LTR390_RESOLUTION_13BIT: Serial.println(13); break;
    case LTR390_RESOLUTION_16BIT: Serial.println(16); break;
    case LTR390_RESOLUTION_17BIT: Serial.println(17); break;
    case LTR390_RESOLUTION_18BIT: Serial.println(18); break;
    case LTR390_RESOLUTION_19BIT: Serial.println(19); break;
    case LTR390_RESOLUTION_20BIT: Serial.println(20); break;
  }

  ltr.setThresholds(100, 1000);
  ltr.configInterrupt(true, LTR390_MODE_UVS);
}



void blink(int miliseconds, int times){
  
  SensorData data;
  int i = 0;
  usbSerial.println("Blinking for  " + String(times) + " Minutes");

  while (i < times*60/(2*miliseconds/1000))
  {
  Serial.println("");
  Serial.println("I= "  + String(i));
  Serial.println("Loop Total= "  + String(times*60/(2*miliseconds/1000)));
  Serial.println("");

  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(miliseconds);       
  data = SensorRead();
  GetEnvVar();
  displayWrite(data.tempF, data.DStempF, data.humidity, data.uvb, data.bars);              // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
  delay(miliseconds);    
  i++;

  }
                // wait for a second
}


void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  TempSensor.begin();
  analogReadResolution(10);  // Set the ADC resolution to 10 bits
  delay(2500);
  usbSerial.begin(115200);
  notecardInit();
  GetEnvVar();
  displayInit();
  ltrInit();
  while (!sht45.begin()) {
    Serial.println("SHT45 not found ! Freezing...");
  }



}


void loop() {
  
  SensorData data;
  Serial.println("Main LOOP");
  Serial.println("Main LOOP");
  Serial.println("Main LOOP");
  data = SensorRead();
  displayWrite(data.tempF, data.DStempF, data.humidity, data.uvb, data.bars);
  NoteAdd(data.tempF, data.DStempF,data.humidity,data.uvb);
  //delay(5000);
  blink(2000,freq);
  // put your main code here, to run repeatedly:
}
