#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <PubSubClient.h> 
#include <WiFi.h>

// Configurações Wi-Fi e MQTT
const char* default_SSID = "Wokwi-GUEST"; 
const char* default_PASSWORD = ""; 
const char* default_BROKER_MQTT = "20.206.249.58"; 
const int default_BROKER_PORT = 1883; 
const char* TOPICO_VELOCIDADE_CARRO1 = "/TEF/fiware_carros_monitor/attrs/speed1"; 
const char* TOPICO_VELOCIDADE_CARRO2 = "/TEF/fiware_carros_monitor/attrs/speed2";
const char* TOPICO_ULTRAPASSAGEM = "/TEF/fiware_carros_monitor/attrs/ultrapassagem1";
const char* TOPICO_ULTRAPASSAGEM2 = "/TEF/fiware_carros_monitor/attrs/ultrapassagem2";
const char* ID_MQTT = "fiware_carros_monitor"; 

const int vel1 = 32; 
const int vel2 = 33;
int speed1 = 0;
int speed2 = 0;
const int echoPin = 2; // Pino (D2) Recebe a informação de volta
const int trigPin = 4; // Pino (D4) Envia o sinal

// Contadores de ultrapassagens
int ultrapassagem1 = 0;
int ultrapassagem2 = 0;

// Inicialização dos LCDs
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

// Número máximo de leituras a armazenar
const int maxLeituras = 10;

// Índice controlador dos arrays
int indice = 0;

// Arrays para armazenar leituras de velocidade e distância
int leituras_vel1[maxLeituras];
int leituras_vel2[maxLeituras];
long distancias[maxLeituras];

// Arrays para armazenar as médias móveis
int mediaMovelVelCarro1[10]; 
int mediaMovelVelCarro2[10]; 
int contagemLeituras = 0; // CONTADOR DE LEITURAS

// Variáveis de tempo
unsigned long previousMillis = 0; // Tempo anterior para intervalos
const long interval = 2000; // Intervalo em milissegundos (2 segundos)

WiFiClient espClient;
PubSubClient MQTT(espClient);

void setup() {
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(26, INPUT);
    pinMode(25, INPUT);

    lcd.init(); 
    lcd.backlight();

    initWiFi();
    initMQTT();
}

void loop() {
    VerificaConexoesEMQTT();
    unsigned long currentMillis = millis(); // Pega o tempo atual
    long duration;
    long distance;

    speed1 = analogRead(vel1) / 10;
    speed2 = analogRead(vel2) / 10;

    // Medição da distância
    digitalWrite(trigPin, LOW);
    delayMicroseconds(4);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration / 58.31;

    // Armazenamento das velocidades e distância nos arrays
    if (indice < maxLeituras) {
        leituras_vel1[indice] = speed1;
        leituras_vel2[indice] = speed2;
        distancias[indice] = distance;
        indice++;
    } else {
        indice = 0; // Reset do índice quando chega ao máximo
    }

    // Adiciona as leituras de velocidade à média móvel
    for (int i = 9; i > 0; i--) {
        mediaMovelVelCarro1[i] = mediaMovelVelCarro1[i - 1];
        mediaMovelVelCarro2[i] = mediaMovelVelCarro2[i - 1];
    }

    mediaMovelVelCarro1[0] = speed1;
    mediaMovelVelCarro2[0] = speed2;
    contagemLeituras++;

    // Verifica se foram feitas 10 leituras
    if (contagemLeituras >= 10) {
        int somaVelCarro1 = 0;
        int somaVelCarro2 = 0;
        for (int i = 0; i < 10; i++) {
            somaVelCarro1 += mediaMovelVelCarro1[i];
            somaVelCarro2 += mediaMovelVelCarro2[i];
        }
        speed1 = somaVelCarro1 / 10;
        speed2 = somaVelCarro2 / 10;
    }

    int chanceultrapassagem = 0;

    // Verificação de ultrapassagem com base na média móvel
    if ((speed1 >= 1.3 * speed2 || speed2 >= 1.3 * speed1) && distance < 400) {
        chanceultrapassagem = 1;
    }

    // Confirmação da chance de ultrapassagem (ocorre somente se passaram-se 1s desde a última)
    if (currentMillis - previousMillis >= interval && chanceultrapassagem == 1) {
        previousMillis = currentMillis; // Salva o tempo atual na variável previousMillis
        if (distance == 2 && speed1 >= 1.3 * speed2) {
            ultrapassagem1++;
            Serial.println("O corredor (1) fez uma ultrapassagem!");
            Serial.print("Ultrapassagens do carro (1): ");
            Serial.println(ultrapassagem1);
        }
        if (distance == 2 && speed2 >= 1.3 * speed1) {
            ultrapassagem2++;
            Serial.println("O corredor (2) fez uma ultrapassagem!");
            Serial.print("Ultrapassagens do carro (2): ");
            Serial.println(ultrapassagem2);
        }
    }

    // Atualiza o LCD a cada iteração
    atualizarLCD();

    // Imprime os dados no Serial Monitor a cada 10 leituras
    if (indice == 0) {
        Serial.print("v1 = ");
        Serial.println(speed1);
        Serial.print("v2 = ");
        Serial.println(speed2);
        Serial.print("dist = ");
        Serial.println(distance);
        Serial.print("Ultrapassagens do carro 1: ");
        Serial.println(ultrapassagem1);
        Serial.print("Ultrapassagens do carro 2: ");
        Serial.println(ultrapassagem2);
    }

    publicarDadosMQTT();
    MQTT.loop();
}

void atualizarLCD() {
    lcd.setCursor(0, 0);
    lcd.print("velCarro1: ");
    lcd.print(speed1);  // Printa speed1
    lcd.setCursor(0, 1);
    lcd.print("velCarro2: ");
    lcd.print(speed2); // Printa speed2
}

void initWiFi() {
    delay(10);
    Serial.println("Conectando ao WiFi...");
    WiFi.begin(default_SSID, default_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi conectado.");
}

void initMQTT() {
    MQTT.setServer(default_BROKER_MQTT, default_BROKER_PORT);
    while (!MQTT.connected()) {
        Serial.print("Conectando ao broker MQTT...");
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado!");
        } else {
            Serial.print("Falha na conexão. Estado: ");
            Serial.print(MQTT.state());
            delay(2000);
        }
    }
}

void publicarDadosMQTT() {
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "{\"velocidade\": %d}", speed1);
    MQTT.publish(TOPICO_VELOCIDADE_CARRO1, buffer);
    
    snprintf(buffer, sizeof(buffer), "{\"velocidade\": %d}", speed2);
    MQTT.publish(TOPICO_VELOCIDADE_CARRO2, buffer);
    
    snprintf(buffer, sizeof(buffer), "{\"ultrapassagem_carro1\": %d}", ultrapassagem1);
    MQTT.publish(TOPICO_ULTRAPASSAGEM, buffer);

    snprintf(buffer, sizeof(buffer), "{\"ultrapassagem_carro2\": %d}", ultrapassagem2);
    MQTT.publish(TOPICO_ULTRAPASSAGEM2, buffer);
}

void VerificaConexoesEMQTT() {
    if (!MQTT.connected())
        reconnectMQTT();
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(default_BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
        } else {
            Serial.println("Falha ao reconectar no broker.");
        }
    }
}

