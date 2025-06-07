#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

// --- CONFIGURAÇÃO DOS SENSORES ---
#define DHTPIN 21
#define DHTTYPE DHT22
const int trigPin = 23;
const int echoPin = 22;

// --- CONFIGURAÇÃO DO ATUADOR (NOVO) ---
#define SIRENE_PIN 13 // Pino onde o LED/Sirene está conectado

DHT dht(DHTPIN, DHTTYPE);

// --- INFORMAÇÕES DA REDE WI-FI ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// --- CONFIGURAÇÃO DO THINGSPEAK ---
unsigned long channelID = 2983238;          // ID do ThingSpeak
const char* writeAPIKey = "NTRRVFUZOZLZ18PB"; // Write API Key
const char* readAPIKey = "N108DGNCAMCEWLDK"; // Read API Key

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Inicializa o sensor DHT22
  dht.begin();

  // Define os pinos para o sensor ultrassônico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Define o pino para a sirene (NOVO)
  pinMode(SIRENE_PIN, OUTPUT);
  digitalWrite(SIRENE_PIN, LOW); // Garante que a sirene comece desligada
  Serial.println("Pino da sirene configurado.");

  // Conecta ao Wi-Fi
  Serial.println("Conectando-se ao Wi-Fi...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConectado ao Wi-Fi!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar ao Wi-Fi!");
    return;
  }

  // Conecta ao ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // --- LEITURA DOS SENSORES ---
  // Medir a distância com o HC-SR04 (nível da água)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = (duration / 2) * 0.0344;

  // Medir a umidade e temperatura do DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha na leitura do DHT22!");
  } else {
    // Exibir os dados no monitor serial
    Serial.println("\n--- DADOS DOS SENSORES ---");
    Serial.print("Distância (nível da água): ");
    Serial.print(distance);
    Serial.println(" cm");

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.print("°C | Umidade: ");
    Serial.print(humidity);
    Serial.println("%");

    // --- ENVIO DOS DADOS PARA O THINGSPEAK ---
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, distance);

    int writeStatusCode = ThingSpeak.writeFields(channelID, writeAPIKey);
    if (writeStatusCode == 200) {
      Serial.println("Dados enviados com sucesso para o ThingSpeak!");
    } else {
      Serial.println("Erro ao enviar dados. HTTP status: " + String(writeStatusCode));
    }
  }

  // --- CONTROLE DO ATUADOR (SIRENE) - (NOVO) ---
  Serial.println("\n--- VERIFICANDO COMANDO DA SIRENE ---");
  // Lê o comando do Field 4 do ThingSpeak
  int comandoSirene = ThingSpeak.readIntField(channelID, 4, readAPIKey);
  int readStatusCode = ThingSpeak.getLastReadStatus();

  if (readStatusCode == 200) {
    Serial.print("Comando da sirene recebido do Field 4: ");
    Serial.println(comandoSirene);
    if (comandoSirene == 1) {
      digitalWrite(SIRENE_PIN, HIGH); // Liga o LED
      Serial.println(">>> ALERTA! SIRENE LIGADA! <<<");
    } else {
      digitalWrite(SIRENE_PIN, LOW);  // Desliga o LED
      Serial.println("Sirene desligada.");
    }
  } else {
    Serial.println("Erro ao ler comando da sirene. HTTP status: " + String(readStatusCode));
  }

  Serial.println("\nAguardando 20 segundos para o próximo ciclo...");
  delay(20000);
}