#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ESP32Servo.h>

/* ====== WIFI ====== */
const char* ssid = "/* SEU ID */";
const char* password = "/* SUA SENHE */";

/* ====== MQTT ====== */
const char* mqtt_server = "test.mosquitto.org";
WiFiClient espClient;
PubSubClient client(espClient);

#define DHTTYPE DHT11
#define DHTPIN_ESTUFA   4
#define DHTPIN_EXTERNO  27

DHT dhtEstufa(DHTPIN_ESTUFA, DHTTYPE);
DHT dhtExterno(DHTPIN_EXTERNO, DHTTYPE);

#define SOIL_ESTUFA   32
#define SOIL_EXTERNO  33
#define MQ135         35
#define CHUVA         34
#define NIVEL_POCO    36

#define LDR_DIGITAL   21   // 0 = claro | 1 = escuro


#define BOMBA_AGUA   23   
#define RELE_VENT    22
#define SERVO_PIN    15
#define LED_ESTUFA   19
#define POSTE_LUZ    26

Servo servoVent;

/* CONTROLE */
String modoSistema = "Auto";

/* ESTADOS */
bool bombaLigada = false;
bool ventoinhaLigada = false;
bool luzEstufaLigada = false;
bool posteLigado = false;

/* LIMITES */
#define TEMP_MAX_ESTUFA   26
#define SOLO_SECO_LIMITE  40
#define MQ_RUIM_LIMITE    2200

/* FUNÇÕES */
int lerMQ135() {
  long soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += analogRead(MQ135);
    delay(10);
  }
  return soma / 10;
}

int soloPercent(int raw) {
  return map(raw, 4095, 0, 0, 100);
}


void aplicarAtuadores() {
  digitalWrite(BOMBA_AGUA, bombaLigada ? LOW : HIGH);
  digitalWrite(LED_ESTUFA, luzEstufaLigada ? HIGH : LOW);
  digitalWrite(POSTE_LUZ, posteLigado ? LOW : HIGH);

  if (ventoinhaLigada) {
    digitalWrite(RELE_VENT, LOW);
    servoVent.write(0);
  } else {
    digitalWrite(RELE_VENT, HIGH);
    servoVent.write(120);
  }
}


void publicarEstados() {
  client.publish("fazenda/geral/bomba/estado", bombaLigada ? "Liga" : "Desliga");
  client.publish("fazenda/estufa/ventoinha/estado", ventoinhaLigada ? "Liga" : "Desliga");
  client.publish("fazenda/estufa/luz/estado", luzEstufaLigada ? "Liga" : "Desliga");
  client.publish("fazenda/geral/poste/estado", posteLigado ? "Liga" : "Desliga");
  client.publish("fazenda/sistema/modo/estado", modoSistema.c_str());
}


void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) msg += (char)payload[i];

  Serial.print("MQTT -> ");
  Serial.print(topic);
  Serial.print(" | ");
  Serial.println(msg);

  // MODO
  if (String(topic) == "fazenda/sistema/modo") {
    modoSistema = msg; 
    client.publish("fazenda/sistema/modo/estado", modoSistema.c_str());
    return;
  }

  // MANUAL
  if (modoSistema == "Manual") {

    if (String(topic) == "fazenda/geral/bomba") {
      bombaLigada = (msg == "Liga");
    }

    if (String(topic) == "fazenda/estufa/ventoinha") {
      ventoinhaLigada = (msg == "Liga");
    }

    if (String(topic) == "fazenda/estufa/luz") {
      luzEstufaLigada = (msg == "Liga");
    }

    if (String(topic) == "fazenda/geral/poste") {
      posteLigado = (msg == "Liga");
    }

    aplicarAtuadores();
    publicarEstados();
  }
}



void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  pinMode(LDR_DIGITAL, INPUT);

  pinMode(BOMBA_AGUA, OUTPUT);
  pinMode(RELE_VENT, OUTPUT);
  pinMode(LED_ESTUFA, OUTPUT);
  pinMode(POSTE_LUZ, OUTPUT);

  digitalWrite(RELE_VENT, HIGH);
  servoVent.attach(SERVO_PIN);
  servoVent.write(120);

  dhtEstufa.begin();
  dhtExterno.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Fazenda")) {

      client.subscribe("fazenda/sistema/modo");
      client.subscribe("fazenda/geral/bomba");
      client.subscribe("fazenda/estufa/ventoinha");
      client.subscribe("fazenda/estufa/luz");
      client.subscribe("fazenda/geral/poste");

      publicarEstados();

    } else {
      delay(2000);
    }
  }
}


void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  // LEITURA DOS SENSORES
  float tempEstufa = dhtEstufa.readTemperature();
  float umidEstufa = dhtEstufa.readHumidity();
  float tempExterna = dhtExterno.readTemperature();
  float umidExterna = dhtExterno.readHumidity();

  int soloEstufa = soloPercent(analogRead(SOIL_ESTUFA));
  int soloExterno = soloPercent(analogRead(SOIL_EXTERNO));

  int chuva = map(analogRead(CHUVA), 4095, 0, 0, 100);
  String statusChuva = (chuva > 30) ? "CHOVENDO" : "SECO";
  client.publish("fazenda/externo/sensor/chuva/status", statusChuva.c_str());

  int nivel = map(analogRead(NIVEL_POCO), 0, 4095, 0, 100);

  // LDR CORRIGIDO
  bool poucaLuz = digitalRead(LDR_DIGITAL) == LOW;

  int mqRaw = lerMQ135();
  String statusAr = (mqRaw < 1500) ? "BOM" : (mqRaw < 2200) ? "MEDIO" : "RUIM";
  int qualidadeAr = constrain(map(mqRaw, 1000, 3000, 100, 0), 0, 100);

  // PUBLICAÇÃO MQTT
  client.publish("fazenda/estufa/sensor/temperatura", String(tempEstufa).c_str());
  client.publish("fazenda/estufa/sensor/umidade", String(umidEstufa).c_str());
  client.publish("fazenda/externo/sensor/temperatura", String(tempExterna).c_str());
  client.publish("fazenda/externo/sensor/umidade", String(umidExterna).c_str());
  client.publish("fazenda/estufa/sensor/solo", String(soloEstufa).c_str());
  client.publish("fazenda/externo/sensor/solo", String(soloExterno).c_str());
  client.publish("fazenda/poco/sensor/nivel", String(nivel).c_str());
  client.publish("fazenda/estufa/sensor/luminosidade", poucaLuz ? "Escuro" : "Claro");
  client.publish("fazenda/estufa/sensor/ar/raw", String(mqRaw).c_str());
  client.publish("fazenda/estufa/sensor/ar/qualidade", String(qualidadeAr).c_str());
  client.publish("fazenda/estufa/sensor/ar/status", statusAr.c_str());

  // CONTROLE AUTOMÁTICO
  if (modoSistema == "Auto") {
    bombaLigada = (soloEstufa < SOLO_SECO_LIMITE || soloExterno < SOLO_SECO_LIMITE);
    ventoinhaLigada = (tempEstufa > TEMP_MAX_ESTUFA || statusAr == "RUIM");
    luzEstufaLigada = poucaLuz;

    aplicarAtuadores();
    publicarEstados();
  }

  delay(3000);
}
