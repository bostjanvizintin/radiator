#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <RTClib.h>


#define RELAY 13 //Relay pin
#define ONE_WIRE_BUS 2 //Dallas sensor pin

#define CHECK_EVERY 4 //The program will check for temperature and time changes every x seconds
#define SET_TIME FALSE //If true the program will update the time on RTC

int endTemp[] = {25,25,21}; //The temperature at whick relay stops
String work[][2] = {{"21:00", "21:05"},{"21:10", "21:15"}, {"21:17", "21:18"}}; //The time at which relay can start (!!!must be XX:XX and not X:XX!!!)


RTC_DS1307 rtc;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

long lastTimeChecked = 0;
long workTime[sizeof(work)/sizeof(work[0])][2];
boolean relay = false;

void setup() {
  while (!Serial) {}
  Serial.begin(9600);

  if(sizeof(endTemp)/sizeof(int) != sizeof(work)/sizeof(work[0])) {
    while(true) {
      Serial.println("endTemp and work arrays have to be the same length"); 
      }
    }

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  sensors.begin();
  Wire.begin();
  rtc.begin();

  if (SET_TIME) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  convertTimes();
}

void loop() {
  int endTemp = getEndTemp();

  sensors.requestTemperatures();
  double temp = sensors.getTempCByIndex(0);

  DateTime now = rtc.now();
  long timeStampNow = (long)now.hour() * 60 * 60 + (long) now.minute() * 60 + (long) now.second();

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(", ");
  Serial.println(endTemp);

  if (lastTimeChecked <= timeStampNow) { //checks if it passed enough time from last check
    lastTimeChecked = timeStampNow + CHECK_EVERY;//updates time of last check
    if (temp < endTemp) { //check if the temperature is right to turn on
      relay = true;
      digitalWrite(RELAY, HIGH);
    } else {
      relay = false;
      digitalWrite(RELAY, LOW);
      }
  }
  delay(1000);
}

void convertTimes() {
   for(int i = 0; i < sizeof(work)/sizeof(work[0]); i++) {
      workTime[i][0] = work[i][0].substring(0, 3).toInt() * 60 * 60 + work[i][0].substring(3).toInt() * 60;
      workTime[i][1] = work[i][1].substring(0, 3).toInt() * 60 * 60 + work[i][1].substring(3).toInt() * 60;
    }
  }

int getEndTemp() {
  int temp = -127;
  
  DateTime now = rtc.now();
  long timeStampNow = (long)now.hour() * 60 * 60 + (long) now.minute() * 60 + (long) now.second();

  for(int i = 0; i < sizeof(work)/sizeof(work[0]); i++) {
    if(timeStampNow > workTime[i][0] && timeStampNow < workTime[i][1]) {
      return endTemp[i];
      }
    }
    return temp;  
  }


