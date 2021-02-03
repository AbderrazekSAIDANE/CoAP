/* 
 *  Abderrazek SAIDANE - EI2I-4
 *  Wassim ALLAL
 *  Date du 11/2020
 *  Application Uplink & Downlink "CoAP ESP32 DHT11 & Lam"
 */


 
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>

#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTTYPE    DHT11 
#define DHTPIN 5     // Digital pin connected to the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

const char* ssid     = /*NOM DE VOTRE RESEAU INTERNET*/;
const char* password = /*LE MOT DE PASSE DU RESEAU INTERNET*/;


// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port);
// CoAP server endpoint url callback
void callback_light_on(CoapPacket &packet, IPAddress ip, int port); //Callback pour allumer la LED
void callback_light_off(CoapPacket &packet, IPAddress ip, int port); //Callback pour éteindre la LED
void captureTemperature_dht11(); //Fonction appellant le DHT11 pour un retour d'information sur la témpérature
void captureHumidite_dht11(); //Fonction appellant le DHT11 pour un retour d'information sur l'humidité

// UDP and CoAP class
WiFiUDP udp; // Utilisation du protocole de télécommunication UDP
Coap coap(udp); // Utilisation du protocole de transfert web CoAP 

// LED STATE
bool LEDSTATE;

// CoAP server endpoint URL
void callback_light_on(CoapPacket &packet, IPAddress ip, int port) { 
  Serial.println("[Light] ON");
  digitalWrite(4, HIGH) ;
  }


// CoAP server endpoint URL
void callback_light_off(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Light] OFF");
  digitalWrite(4, LOW) ;
}

// CoAP server endpoint URL
// GET dht data
void captureTemperature_dht11(){
  static char char_t[6];
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t)) {
    Serial.println("Failed to read Temparature from DHT sensor!");
    return;
  }
  dtostrf(t, 2, 2, char_t);
  //dtostrf(h, 3, 2, char_h);
  Serial.println(char_t);
  Serial.println(t);
  //Serial.println(char_h);
  Serial.println("Received GET request for dht sensor.");
  coap.put(IPAddress(192,168,1,31), 5680   , "temp", char_t); //Cette adresse IP est à modifiée par l'adresse IP de l'ordinateur sur lequel Node red est installé
}

void captureHumidite_dht11(){
  static char char_h[6];
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read Humidity from DHT sensor!");
    return;
  }
  dtostrf(h, 2, 2, char_h);
  Serial.println(char_h);  
  Serial.println("Received GET request for dht sensor.");
  coap.put(IPAddress(192,168,1,31), 5683, "hum", char_h); //Cette adresse IP est à modifiée par l'adresse IP de l'ordinateur sur lequel Node red est installé   
}

// CoAP client response callback
/*void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Coap Response got]");
  
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  Serial.println(p);
}*/

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // LED State
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  LEDSTATE = true;
  
  //Callback_light
  Serial.println("Setup Callback Light ON");
  coap.server(callback_light_on, "light/ON");
  Serial.println("Setup Callback Light OFF");
  coap.server(callback_light_off, "light/OFF");
  
  //Callback_response
  //coap.response(callback_response);
  
  // start coap server/client
  coap.start();

}

void loop() {
  captureTemperature_dht11();
  //delay(5000);
  //captureHumidite_dht11();
  delay(5000);
  //delay(3000);
  coap.loop();
  
}
