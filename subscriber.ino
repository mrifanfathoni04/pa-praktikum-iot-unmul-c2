#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

// Update these with values suitable for your network.
const char* ssid = "Tong";
const char* password = "tonggnot"; 
const char* mqtt_server = "broker.hivemq.com"; // broker gratisan

// Buzzer & LED pin
#define BUZZER_PIN D7

int sudut = 0;

Servo myservo;

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

bool cek_int(String str)
{
    for (int x = 0; x < str.length(); x++)
    {
        if (isdigit(str[x]) == false)
        {
            return false;
        }
    }
    return true;
};

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Fungsi untuk menerima data
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("] ");
  String data = ""; // variabel untuk menyimpan data yang berbentuk array char
  for (int i = 0; i < length; i++) {
    // Serial.print((char)payload[i]);
    data += (char)payload[i]; // menyimpan kumpulan char kedalam string
  }
  int jarak;
  if(cek_int(data)){
  Serial.println(data  + " cm");
   int jarak = data.toInt();
  }else if(data=="off"){
    Serial.println("Keran dimatikan dari MQTT");
  }else if(data=="on"){
    Serial.println("Keran dinyalakan dari MQTT");
  }
  // Serial.println(" cm");
  if(!cek_int(data)){

  }
  else if (jarak > 100) { // pengkondisian
    sudut = 90;
  
    digitalWrite(BUZZER_PIN,LOW);
    myservo.write(sudut);
  } 

  else if (jarak > 10 && jarak < 20 && sudut == 90){
    digitalWrite(BUZZER_PIN,HIGH);
  } 
  
  else if (jarak <= 10) {
    sudut = 0;
    digitalWrite(BUZZER_PIN,LOW);
    myservo.write(0);
  }

  else {
    digitalWrite(BUZZER_PIN,LOW);
  }

  if (data == "off") {
    sudut = 0;
    myservo.write(sudut);
    Serial.println("keran mati");
  } else if(data == "on") {
    sudut = 90;
    myservo.write(sudut);
    Serial.println("keran nyala");
  }
  if(sudut==90){
  Serial.println("posisi keran terbuka(nyala)");}
  else{Serial.println("posisi keran tertutup(mati)");}
}

// fungsi untuk mengubungkan ke broker
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("papraktikumc2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);  // Inisialisasi pin buzzer
  myservo.attach(D4);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883); // setup awal ke server mqtt
  client.setCallback(callback); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}