#include <Wire.h>
#include <SD.h>
#include <rgb_lcd.h>
#include <TH02_dev.h>
#include <RTClib.h>

/*

Read Radon values off SafetySiren Radon Gas Detector model HS80002

*/
RTC_Millis RTC;
File myFile;

int segments[] = {
0,0,0,0,0,0,0};

int leddigits[] = {
1,1,1,1,1,1,1, // space
1,1,1,0,0,0,1, // L
0,0,0,0,0,0,1, // 0
1,0,0,1,1,1,1, // 1
0,0,1,0,0,1,0, // 2
0,0,0,0,1,1,0, // 3
1,0,0,1,1,0,0, // 4
0,1,0,0,1,0,0, // 5 or S
0,1,0,0,0,0,0, // 6
0,0,0,1,1,1,1, // 7
0,0,0,0,0,0,0, // 8
0,0,0,1,1,0,0 // 9
};

char ledvalues[] = " L0123456789";
char leddisp[] = "  ";

const int pinButton = 0;
int i = 0;
int j = 0;
int found = 0;
int ledvaluecnt = 0;

unsigned long duration;

void setup() {
Serial.begin(9600);

delay(150);
TH02.begin();

RTC.begin(DateTime(__DATE__, __TIME__));

// Initialize SD card
if(!SD.begin()){
      Serial.println("SD Initialization Failed.");
      return;
    }
    Serial.println("SD Initialization Complete.");
    
    
pinMode(pinButton, INPUT);
pinMode(2, INPUT); // Digit 1 strobe
pinMode(3, INPUT); // Digit 2 strobe
pinMode(4, INPUT); // Segment A
pinMode(5, INPUT); // Segment B
pinMode(6, INPUT); // Segment C
pinMode(7, INPUT); // Segment D
pinMode(8, INPUT); // Segment E
pinMode(9, INPUT); // Segment F
pinMode(13, INPUT); // Segment G
}

void loop() {
  // get 'now'
  DateTime now = RTC.now();
  
  /*
     Digit 1
   */
  duration = pulseIn(2, LOW);
  segments[0] = digitalRead(4);
  segments[1] = digitalRead(5);
  segments[2] = digitalRead(6);
  segments[3] = digitalRead(7);
  segments[4] = digitalRead(8);
  segments[5] = digitalRead(9);
  segments[6] = digitalRead(13);

  ledvaluecnt = 0;
  for (i=0;i<sizeof(leddigits)/sizeof(int);i=i+7)
  {
    found = 1;
    for (j=0;j<7;j++)
    {
      if (segments[j] != leddigits[i+j]){
        found = 0;
        break;
      }
    }
    if (found == 1)
    {
      leddisp[0] = ledvalues[ledvaluecnt];
      break;
    }

    ledvaluecnt++;
  } 

  /*
   Digit 2
   */
  duration = pulseIn(3, LOW);
  segments[0] = digitalRead(4);
  segments[1] = digitalRead(5);
  segments[2] = digitalRead(6);
  segments[3] = digitalRead(7);
  segments[4] = digitalRead(8);
  segments[5] = digitalRead(9);
  segments[6] = digitalRead(13);

  ledvaluecnt = 0;
  for (i=0;i<sizeof(leddigits)/sizeof(int);i=i+7)
  {
    found = 1;
    for (j=0;j<7;j++)
    {
      if (segments[j] != leddigits[i+j]){
        found = 0;
        break;
      }
    }
    if (found == 1)
    {
      leddisp[1] = ledvalues[ledvaluecnt];
      if (leddisp[1] != ' ')
      {
        Serial.println(leddisp);
        break;
      }
    }
    ledvaluecnt++;
  }
    
  Serial.println(String((int) TH02.ReadTemperature()));
  Serial.println(String((int) TH02.ReadHumidity()));
   
  delay(1000);
  String year = String(now.year());
  String month = String(now.month());
  String day = String(now.day());
  String path = year + month + day + ".txt";
  char buf[16];
  path.toCharArray(buf, 16);
  myFile = SD.open(buf, FILE_WRITE);
  if(myFile){
    Serial.println("Writing to file.");
    String hour = String(now.hour());
    String min = String(now.minute());
    String sec = String(now.second());
    myFile.println("Timestamp: " + hour + min + sec);
    myFile.println("Temperature: " + String((int) TH02.ReadTemperature()));
    myFile.println("Humidity: " + String((int) TH02.ReadHumidity()));
    myFile.println("Light: " + String(analogRead(A2)));
    myFile.println("UV: " + String(analogRead(A3)));
    myFile.println("Radon: " + String(leddisp[0]) + String(leddisp[1]));    
    myFile.close();
  } else{
    Serial.println("Error opening file.");
  }
}
