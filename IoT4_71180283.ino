#include <WiFi.h>
#include <FirebaseESP32.h>
#include <HCSR04.h>

#define FIREBASE_HOST "https://sensorlampu-a94ba-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "VkDKXQE62nvMS2waolDSc6Zp1ybp0XLZqmj8WD19"
#define WIFI_SSID "iyan"
#define WIFI_PASSWORD "07042000"

int pinRelay = 27;
int pos = 0;
UltraSonicDistanceSensor sensorJarak(26,25);

FirebaseData firebaseData;

String path = "/node1";
int jarakLama;
int jarakBaru;

int on_off = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWifi();
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, HIGH);   // matiin relay (low level trigger)
  jarakLama = sensorJarak.measureDistanceCm();
}

void loop() {
  //cek status sistem pada firebase, jika 1 maka hidup dan jika 0 maka mati
  Firebase.getInt(firebaseData, path + "/statusSistem");
  on_off = firebaseData.intData();
  Serial.print("kondisi sistem ");
  Serial.println(on_off);
  if(on_off == 1){
    //ambil nilai dari sensor ultrasonic dan kirim ke firebase
    jarakBaru = sensorJarak.measureDistanceCm();
    Serial.println(jarakBaru);

    if(jarakBaru != jarakLama){
      Firebase.setInt(firebaseData, path + "/jarak", jarakBaru);
      jarakLama = jarakBaru;
      
      //lakukan pengecekan jika jarak kurang dari 10cm maka akan menyalakan relay
      if(jarakLama < 10 && jarakLama != -1){
        if(pos == 0) {
          digitalWrite(pinRelay, LOW);    //menyalakan relay
          pos = 1;
        }
        Firebase.setInt(firebaseData, path + "/kondisiRelay", 1);
        Serial.println("Relay nyala");
        delay(5000);          // nyala selama 5 detik
      }
      else {
        if(pos == 1) {
          digitalWrite(pinRelay, HIGH);    //mematikan relay
          pos = 0;
        }
        Firebase.setInt(firebaseData, path + "/kondisiRelay", 0);
        Serial.println("Relay mati");
      }
    }
    else {
      Firebase.setInt(firebaseData, path + "/kondisiRelay", 0);
      Serial.println("Relay mati");
    }
  }
  else{
    
  }
}

void initWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(". \n");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  while (Firebase.ready() != true)
  {
    Serial.print("Firebase belum terhubung");
 
  }
  Firebase.ready();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}
