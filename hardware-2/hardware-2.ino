#include <DHT.h>
#include <SFE_BMP180.h>
#include <Wire.h>
//#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <TinyGPS.h>
#include <math.h>
#include "RF24.h"
#include <SPI.h>

// nRF24
const uint64_t pipe = 0xE8E8F0F0E1LL; // адрес канала передачи
RF24 radio(49,53);

// nRF24 data structure
typedef struct {
  time_t timestamp;
  float lat;
  float lon;
  float pressure;
  int co;
  int temp;
  int hud;
  int pm1;
  int pm25;
  int pm10;
  int al;
  int dem;
} 
Data;

Data data;

//
SFE_BMP180 pressure;

//GPS
#define SerialGPS Serial3
TinyGPS gps;

// DHT-22
#define DHTPIN 2 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

//PMS5003
#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
unsigned char buf[LENG];
int PM01Value = 0;          
int PM25Value = 0;
int PM10Value = 0;
SoftwareSerial PMS5003_Serial(11, 10); // RX, TX


// CO MQ7
//#define buzzer 10
#define sensor A0
int coValue;

// ban dau tinh do cao
double t0 = 0;
double p0;
//code test
 String json_string;
 
void setup() {
  // put your setup code here, to run once:
  //// CODE TEST
  data.dem = 0;
  Serial.begin(9600);
  PMS5003_Serial.begin(9600);
  SerialGPS.begin(9600);
  
//  Serial.println("DHTxx test!");
  initPressure();
  dht.begin();
//  t0 = getTemperaturePressure();
 // p0 = getPressure(t0);
    t0 = 20.4;
  p0 = getPressure(t0);

  // nRF24 
  initRF24();
}

void loop() {
  // put your main code here, to run repeatedly:
//  if(senddata){
    sendData();
//  }
  delay(50);
}

void initRF24(){
  radio.begin();                      // Включение модуля
  radio.setAutoAck(1);                // Установка режима подтверждения приема;
  radio.setRetries(1,1);              // Установка интервала и количества попыток
  radio.setDataRate(RF24_250KBPS);    // Устанавливаем скорость
  radio.setPALevel(RF24_PA_MAX);      // Recommended LOW PA Level
  radio.setChannel(10);               // Устанавливаем канал
//  radio.setPayloadSize(16);         // Mac dinh 32 byte
  radio.openWritingPipe(pipe);
}

void sendData(){
  getLatLng(&data.lat, &data.lon, &data.al);
  data.timestamp = now();
  
//  data.lat = 0;
//  data.lon = 0;
  data.temp = int(getTemperature()*10);
  data.hud = int(getHumidity()*10);
  data.pressure = getPressure(getTemperaturePressure());
//  String pressure_DHT22 = String(getPressure(getTemperature()));
  
  int* pm = readPM();
  data.pm1 = pm[0];
  data.pm25 = pm[1];
  data.pm10 = pm[2];
//  coValue = (analogRead(sensor)/1024)*5*200;
  data.co = analogRead(sensor);
  data.al = int(getAltitudePressure(getTemperaturePressure(), t0)*100);

//  code test
  json_string = String("{\"time\":\"" + String(data.timestamp) + "\",") 
                + String("\"values\":{")
                + String("\"LAT\":\"" + String(data.lat,10) + "\",")
                + String("\"LON\":\"" + String(data.lon,10) + "\",")
                + String("\"temp\":\"" + String(data.temp) + "\",")
                + String("\"hud\":\"" + String(data.hud) + "\",")
                + String("\"pressure\":\"" + String(data.pressure) + "\",")
                + String("\"pm1\":\"" + String(data.pm1) + "\",")
                + String("\"pm25\":\"" + String(data.pm25) + "\",")
                + String("\"pm10\":\"" + String(data.pm10) + "\",")
                + String("\"CO\":\"" + String(data.co) + "\",")
                + String("\"al\":\"" + String(data.al) + "\",")
                + String("\"dem\":\"" + String(data.dem) + "\"")
                + String("}}");
       Serial.println(json_string);
  radio.write(&data, sizeof(data));
  //  CODE TEST
  data.dem+=1;
//  if(data.dem == 10) {
//    data.dem = 0;
//    }
}

void setTimefromGPS(){

  // thieu vong for o ngoai nen ko chay vao while duoc.  
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (SerialGPS.available()) {
    if (gps.encode(SerialGPS.read())) { // process gps messages
      // when TinyGPS reports new data...
      unsigned long age;
      int Year;
      byte Month, Day, Hour, Minute, Second;
      
      // Truong ko them dong nay thi sao cac bien Month, day, hour... o tren thay doi duoc???
      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
      
      if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
//        adjustTime(offset * SECS_PER_HOUR);
      }
      
    }
  }
  }
}
void getLatLng(float *lat,float *lon ,int *al){
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  float flat = 0, flon = 0;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (SerialGPS.available())
    {
      char c = SerialGPS.read();
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    *lat = flat;
    *lon = flon;
//    *al = int(gps.f_altitude()*100);
    
    int Year;
    byte Month, Day, Hour, Minute, Second;
    gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
    
    if (age < 500) {
      // set the Time to the latest GPS reading
      setTime(Hour, Minute, Second, Day, Month, Year);
    }
  
  }
  
  gps.stats(&chars, &sentences, &failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
}
// return 1: success
// return 0: fail
int initPressure(){
//  String result = "";
  
  if (pressure.begin()){
//    Serial.println("BMP180 init success");
    return 1;
  }
  else
  {
//    Serial.println("BMP180 init fail (disconnected?)");
    return 0;
  } 
//  return result;
}

// return 0 fail
double getTemperature(){
  double t = dht.readTemperature();
  String result = "";
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return 0;
  }
  return t;
}

// return 0 fail
double getHumidity(){
  
  double h = dht.readHumidity();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return 0;
  }
  return h;
}

// return 0 fail
double getPressure(double T){
  char status;
  double P;
  status = pressure.startPressure(3);
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    // Retrieve the completed pressure measurement:
    // Note that the measurement is stored in the variable P.
    // Use '&P' to provide the address of P.
    // Note also that the function requires the previous temperature measurement (T).
    // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getPressure(P,T);
    if (status != 0)
    {
      return P;
    }
    else return 0;
  }
  else return 0;
}

// return 0 fail
double getTemperaturePressure()
{
  char status;
  double T; //,P,p0,a;


  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
//    Serial.print("Temp of pressure: ");
//    Serial.println(T);
    if (status != 0)
    {
      return T;
    }
    else{
       Serial.println("error retrieving temperature measurement\n");
       return 0;
    }
  }
  else {
    Serial.println("error starting temperature measurement\n");
    return 0;
  }
}

/**
* t,p la nhiet do, ap suat o tren cao
* t0, p0 la nhiet do, ap suat ban dau duoi mat dat
*/
double getAltitudePressure(double t, double t0){
  const double R = 287.05; // J/Kg°K
  const double g = 9.80665; //m/s 2.
  
  if(t == 0 || t0 == 0){
    return 0;
  }
  else {
    double k = 273.15;
    double T = t + k;
    double T0 = t0 + k;
    double p = getPressure(t);
    if(p == 0) return 0;
    else {
      return ((R/g)*((T+ T0)/2)*log10 (p0/p));
    }
  }
}

int* readPM(){
   if(PMS5003_Serial.find(0x42)){    //start to read when detect 0x42
    PMS5003_Serial.readBytes(buf,LENG);
 
    if(buf[0] == 0x4d){
      if(checkValue(buf,LENG)){
        PM01Value = transmitPM01(buf); //count PM1.0 value of the air detector module
        PM25Value = transmitPM25(buf);//count PM2.5 value of the air detector module
        PM10Value = transmitPM10(buf); //count PM10 value of the air detector module 
      }           
    }
  }
  static int result[3];
  result[0] = PM01Value;
  result[1] = PM25Value;
  result[2] = PM10Value;
  return result;
}

//Check pms5003 module
char checkValue(unsigned char *thebuf, char leng)
{  
  char receiveflag=0;
  int receiveSum=0;
 
  for(int i=0; i<(leng-2); i++){
  receiveSum=receiveSum+thebuf[i];
  }
  receiveSum=receiveSum + 0x42;
  
  if(receiveSum == ((thebuf[leng-2]<<8)+thebuf[leng-1]))  //check the serial data 
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

//transmit PM 1.0
int transmitPM01(unsigned char *thebuf)
{
  int PM01Val;
  PM01Val = ((thebuf[3]<<8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}
 
//transmit PM 2.5
int transmitPM25(unsigned char *thebuf)
{
  int PM25Val;
  PM25Val = ((thebuf[5]<<8) + thebuf[6]); //count PM2.5 value of the air detector module
  return PM25Val;
}
 
//transmit PM 10
int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val = ((thebuf[7]<<8) + thebuf[8]); //count PM10 value of the air detector module  
  return PM10Val;
}
