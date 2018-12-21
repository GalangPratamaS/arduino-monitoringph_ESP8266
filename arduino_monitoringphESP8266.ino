#include <SoftwareSerial.h>
SoftwareSerial wifi (6,7); // Rx Tx
#include <OneWire.h>
#include <DallasTemperature.h>

#define SensorPin A2            //pH meter Analog output to Arduino Analog Input 0 A2
#define SensorSoilMeasure A0  //soil measure sensor A0
#define ONE_WIRE_BUS A1  // sensor suhu

#define nama_wifi "gratis"
#define pass_wifi "pirates123"
#define ip_host "192.168.43.207"  // alamat website

int sensorPin = A0;
int nilai_sensor;
boolean connected = false;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorSuhu(&oneWire);

float suhuSekarang;
float ph;
int sensorValue = 0;        //ADC value from sensor
float outputValue = 0.0;        //pH value after conversion 
int SoilMeasture;

void setup() {
  // put your setup code here, to run once:
wifi.begin(115200);
Serial.begin(9600);
wifi.setTimeout(5000);
Serial.println("ESP8266 cek cek");
delay (1000);
wifi.println("AT+RST");
delay(1000);
if(wifi.find("WIFI GOT IP"))
{
  Serial.println(" ESP8266 SIAP ");
}
else {
  Serial.println(" Tidak Ada Response dari ESP8266 ");
  while(1);
}
delay(1000);

for (int i=0; i<5; i++){
  connect_to_wifi();
  if (connected){
    break;
  }
}
  if (!connected){
    while(1);
  }
  delay(5000);
  wifi.println("AT+CIPMUX=0");
  delay(1000);

   // Serial.begin(9600);
  sensorSuhu.begin();
}

void loop() {

   suhuSekarang = ambilSuhu();
  SoilMeasture = ambilSoilMeasture();
  ph = hasil();
  String tempSuhu = String(suhuSekarang);
  String tempSoil = String(SoilMeasture);
  String tempPh = String(ph);
  
  // put your main code here, to run repeatedly:
String cmd = "AT+CIPSTART=\"TCP\",\"";
cmd+= ip_host;
cmd+="\",80";
wifi.println(cmd);
Serial.println(cmd);
if (wifi.find("Error")){
  Serial.println("Koneksi eror");
  return;
}
nilai_sensor = analogRead(sensorPin);
cmd = "GET /monitoring/insert_data.php?suhu=";
cmd+=tempSuhu;
cmd+="&humidity=";
cmd+=tempSoil;
cmd+="&ph_tanah=";
cmd+=tempPh;
cmd+="\r\n";
cmd+="HTTP/1.0/1/\r\n";
cmd+="\r\n";

wifi.print("AT+CIPSEND=");
wifi.println(cmd.length());
if (wifi.find(">")){
  Serial.print(">");
} else {
  wifi.println("AT+CIPCLOSE");
  Serial.println("Koneksi Timeout");
  delay(1000);
  return;
}
wifi.print(cmd);
delay(2000);

while(wifi.available())
{
  char c =wifi.read();
  Serial.write(c);
  if (c=='\r') Serial.print('\n');
}
Serial.println("-----end");
delay(10000);
}

void connect_to_wifi()
{
  wifi.println("AT+CWMODE=1");
  String cmd = "AT+CWJAP=\"";
  cmd+=nama_wifi;
  cmd+="\",\"";
  cmd+=pass_wifi;
  cmd+="\"";
  wifi.println(cmd);
  Serial.println(cmd);
  if (wifi.find("OK")){
    Serial.println("Berhasil Terkoneksi ke internet");
  connected=true;
  } else {
    Serial.println("Gagal Terkoneksi");
  connected=false;
  }

}

float ambilSuhu(){
   sensorSuhu.requestTemperatures();
   float suhu = sensorSuhu.getTempCByIndex(0);
   return suhu;  
   delay(100); 
}

int ambilSoilMeasture(){
   int measture = analogRead(A0);
   return measture;   
   delay(100);
}

float hasil(){
  sensorValue = analogRead(SensorPin);

  //Mathematical conversion from ADC to pH
  //rumus didapat be  rdasarkan datasheet 
  outputValue = (-0.0693*sensorValue)+7.3855;

  return outputValue;
  delay(100);
}

