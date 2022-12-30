#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#define P1 D7
#define P2 D6
#define P3 D5

int flagP1 = 1, flagP2 = 1, flagP3 = 1;

// Network ID
#define FIREBASE_HOST "parking-69dc3-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "W17MJuTNDcW0JJZG7v4M8LSjfz7coeovoCW9k0c5"
const char* ssid     = "tetringan";
const char* password = "password";

const long utcOffsetInSeconds = 3600*7;
//area saya ada di WIB Indonesia,
//dan itu harus +7jam dari GMT
//sehingga program diatas harus dikalikan dengan 7 saja

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

String dayStamp, timeStamp;

void setup() {
  // Initialize the time client
  timeClient.begin();
  
  // NodeMCU Utility
  Serial.begin(9600);

  // Networking
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //deklarasi pin
  pinMode(P1, INPUT);
  pinMode(P2, INPUT);
  pinMode(P3, INPUT);

  //start firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
}

void loop(){
  WiFiClient client;


  // Ketersediaan Area
  count();
   
  // park 1
  int park1 = digitalRead(P1);
  
  if(park1 == LOW){
    // send status to firebase
    Firebase.setString("/area/park1" , "Terisi");

    // send logs to firebase
    if(flagP1 != park1) logs("park1", "masuk");

    flagP1 = park1;
  }
  else{
    // send status to firebase
    Firebase.setString("/area/park1" , "Kosong");

    // send logs to firebase
    if(flagP1 != park1) logs("park1", "keluar");
    
    flagP1 = park1;
  }

  // park 2
  int park2 = digitalRead(P2);
  
  if(park2 == LOW){
    // send status to firebase
    Firebase.setString("/area/park2" , "Terisi");

    // send logs to firebase
    if(flagP2 != park2) logs("park2", "masuk");

    flagP2 = park2;
  }
  else{
    // send kosong to firebase
    Firebase.setString("/area/park2" , "Kosong");

    // send logs to firebase
    if(flagP2 != park2) logs("park2", "keluar");
    
    flagP2 = park2;
  }


  // park 3
  int park3 = digitalRead(P3);
  
  if(park3 == LOW){
    // send status to firebase
    Firebase.setString("/area/park3" , "Terisi");

    // send logs to firebase
    if(flagP3 != park3) logs("park3", "masuk");

    flagP3 = park3;
  }
  else{
    // send status to firebase
    Firebase.setString("/area/park3" , "Kosong");

    // send logs to firebase
    if(flagP3 != park3) logs("park3", "keluar");
    
    flagP3 = park3;
  }
  
  if (Firebase.failed()) {
      Serial.print("setting /area/park failed:");
      Serial.println(Firebase.error());  
      return;
  }
  
} 


// Function that gets current epoch time
void getTime() {
  timeClient.update();

   //Get a time structure
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  // Extract date
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
  
  dayStamp = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  Serial.println(dayStamp);
  
  // Extract time
  timeStamp = timeClient.getFormattedTime();
  Serial.println(timeStamp);
}

void logs(String area, String logs){
  getTime();
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& object = jsonBuffer.createObject(); 
  object["area"] = area;
  object["status"] = logs;
  object["date"] = dayStamp;
  object["time"] = timeStamp;

  // append a new value to /logs
  String name = Firebase.push("logs/", object);
  // handle error
  if (Firebase.failed()) {
      Serial.print("pushing /logs failed:");
      Serial.println(Firebase.error());  
      return;
  }
  
  Serial.print("pushed: /logs/" + dayStamp + "/" + area);
  Serial.println(name);  
}

void count(){
  
  // append a new value to /logs
  int count = flagP1 + flagP2 + flagP3;
  Firebase.setInt("/area/count", count);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /area/count failed:");
      Serial.println(Firebase.error());  
      return;
  }
  
} 
