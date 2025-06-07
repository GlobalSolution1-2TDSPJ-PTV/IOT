w#include <WiFi.h> 
#include <ThingSpeak.h>
#include <DHT.h>

#define DHTPIN 21
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const int trigPin = 23;
const int echoPin = 22;

// Informações da rede Wi-Fi
const char* ssid = "Wokwi-GUEST";           // Substitua com o nome da sua rede Wi-Fi
const char* password = "";      // Substitua com a senha da sua rede Wi-Fi

// Configuração do ThingSpeak
unsigned long channelID = 2979598;        // Substitua com o ID do seu canal no ThingSpeak
const char* writeAPIKey = "CCDYC36P8A6NT8CH"; // Substitua com sua Write API Key

WiFiClient client;  // Criação do objeto cliente Wi-Fi

void setup() {
  Serial.begin(115200);

  // Inicializa o DHT22
  dht.begin();

  // Conecta ao Wi-Fi
  Serial.println("Conectando-se ao Wi-Fi...");
  WiFi.begin(ssid, password);

  // Espera pela conexão Wi-Fi
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);  // Atraso de 1 segundo entre as tentativas
    Serial.print(".");
    attempts++;
  }

  // Verifica o status da conexão
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
  
  // Definir pinos para o sensor ultrassônico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Medir a distância com o HC-SR04 (nível da água)
  long duration, distance;
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344; // Convertendo para cm

  // Medir a umidade e temperatura do DHT22
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Verificar se a leitura do DHT falhou
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha na leitura do DHT22!");
    return;
  }

  // Exibir os dados no monitor serial
  Serial.print("Distância (nível da água): ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print("C  Umidade: ");
  Serial.print(humidity);
  Serial.println("%");

  // Enviar os dados para o ThingSpeak
  ThingSpeak.setField(1, temperature);  // Envia a temperatura para o campo 1
  ThingSpeak.setField(2, humidity);     // Envia a umidade para o campo 2
  ThingSpeak.setField(3, distance);     // Envia a distância (nível da água) para o campo 3

  // Atualiza o canal do ThingSpeak
  int x = ThingSpeak.writeFields(channelID, writeAPIKey);
  if(x == 200) {
    Serial.println("Dados enviados com sucesso para o ThingSpeak!");
  } else {
    Serial.println("Erro ao enviar os dados para o ThingSpeak.");
  }

  delay(20000); // Aguarda 20 segundos antes de enviar novamente (limite de atualização do ThingSpeak)
}
