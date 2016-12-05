//Echo Dot - Espectro

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

//Pilih PIN RGB
#define PIN 3

//Jumlah neopixel led
#define NUMPIXELS 1

//Wifi name & pass
const char *ssid = "wifi berbayar";
const char *password = "123qweasd";
//mqtt setting
const char *topik = "mifmasterz/assistant/control";
const char* mqtt_server = "cloud.makestro.com";
const char *uname = "mifmasterz";
const char *pass = "123qweasd";
//library mqtt
WiFiClient espClient;
PubSubClient client(espClient);

//neopixel lib
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
    //konek ke wifi
    Serial.begin(9600);
    WiFi.begin(ssid, password);
    Serial.println("");
    // tunggu mpe konek
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    //konek ke mqtt
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    /*
    setting led
    putih : setColor(255,255,255);
    merah : setColor(150,0,0);
    ijo : setColor(0,150,0);
    biru : setColor(0,0,150);
    mati : setColor(0,0,0);
    */
    Serial.println("cloud makestro connected");
    pixels.begin(); // init NeoPixel library.
    setColor(150,0,0);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char msg[255];
  DynamicJsonBuffer jsonBuffer;
  //convert byte to array char
  for(int i =0; i<length; i++){
     msg[i] = (char)payload[i];
  }
  //parse json
  JsonObject& root = jsonBuffer.parseObject(msg);

  String request1 = root["request"];
  String  reference1 = root["reference"];
  String  value1 = root["value"];

  Serial.println();
  Serial.println("nilai:" + value1);
  //cek nilainya
  if(value1=="1"){
    //blue color
    setColor(0,0,150);
  }else{
    //turn off
    setColor(0,0,0);
  }
  // buang json ke serial untuk debug
  String output;
  root.printTo(output);
  Serial.println(output);

}

void reconnect() {
  // loop sampe bisa konek lagi
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // coba konek
    if (client.connect("espectro_client", uname, pass)) {
      Serial.println("connected");
      //subscribe ke topik
      client.subscribe(topik);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // delay 5 detik
      delay(5000);
    }
  }
}

void setColor(int R, int G, int B)
{
    //cetak ke serial jika ada pergantian warna dari web
    Serial.println("Set Color R:"+String(R)+" G:"+String(G)+" B:"+String(B));
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    //set warna led ke 0
    pixels.setPixelColor(0, pixels.Color(R, G, B));

    pixels.show();
}


void loop()
{
  //kalau belom konek ke mqtt konek dulu lah...
  if (!client.connected()) {
      reconnect();
    }
    client.loop();
}
