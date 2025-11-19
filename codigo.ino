/**
 * Autores:
 * Miguel Amaro Dourado de Sousa
 * Jéssica Vitória Tavares
 * Luara Freire Soares
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>

// ============================================================
//  CONFIGURAÇÕES DE REDE E BROKER
// ============================================================

// Credenciais Wi-Fi
const char* SSID = "NOME_DA_SUA_REDE_WIFI";         // Ex: "MinhaCasa" ou "Wokwi-GUEST" no simulador
const char* PASSWORD = "SENHA_DA_SUA_REDE_WIFI";     // Deixe vazio "" se for rede aberta

// Endereço do Broker MQTT (FIWARE)
const char* BROKER_MQTT = "IP_PUBLICO_DA_SUA_EC2";   // IP Público da Instância AWS ou Localhost
const int BROKER_PORT = 1883;                        // Porta padrão MQTT

// ============================================================
//  CONFIGURAÇÕES DO DISPOSITIVO (Fiware IoT Agent)
// ============================================================

// ID único deste ESP32 na rede MQTT (não pode haver dois iguais conectados)
const char* ID_MQTT = "esp32_setor_exemplo"; 

// Tópico onde o ESP32 vai PUBLICAR os dados dos sensores
const char* TOPICO_PUBLISH = "/TEF/sensor_setor_001/attrs"; 

// Tópico onde o ESP32 vai ESCUTAR comandos
const char* TOPICO_SUBSCRIBE = "/TEF/sensor_setor_001/cmd"; 

// Prefixo usado para validar comandos recebidos (deve ser igual ao Device ID)
const char* topicPrefix = "sensor_setor_001"; 

// ============================================================
//  PINAGEM
// ============================================================
const int PINO_LED = 2;       // LED onboard (D4 ou D2 dependendo da placa)
const int PINO_LDR = 34;      // Sensor de Luz (LDR)
const int PINO_POT_CO2 = 35;  // Potenciômetro (Simula sensor de gás/CO2)
const int PINO_DHT = 15;      // Sensor de Temperatura/Umidade (DHT22)

// ============================================================
//  OBJETOS E VARIÁVEIS GLOBAIS
// ============================================================
DHTesp dht;
WiFiClient espClient;
PubSubClient MQTT(espClient);

// Controle de estado do atuador
char EstadoSaida = '0';

// ============================================================
//  FUNÇÕES AUXILIARES
// ============================================================

void initSerial() {
    Serial.begin(115200);
}

void initWiFi() {
    delay(10);
    Serial.println("------ Conexao WI-FI ------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    WiFi.begin(SSID, PASSWORD);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Conectado! IP obtido: ");
    Serial.println(WiFi.localIP());
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    Serial.print("- Mensagem recebida (Comando): ");
    Serial.println(msg);

    // Padrão de comando UltraLight 2.0: device_id@comando|valor
    String onTopic = String(topicPrefix) + "@on|";
    String offTopic = String(topicPrefix) + "@off|";

    if (msg.equals(onTopic)) {
        digitalWrite(PINO_LED, HIGH);
        EstadoSaida = '1';
        Serial.println("Comando ON recebido -> LED LIGADO");
    } else if (msg.equals(offTopic)) {
        digitalWrite(PINO_LED, LOW);
        EstadoSaida = '0';
        Serial.println("Comando OFF recebido -> LED DESLIGADO");
    }
}

void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

void reconnectMQTT() {
    while (!MQTT.connected()) {
        Serial.print("* Tentando conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE);
        } else {
            Serial.print("Falha. Estado: ");
            Serial.print(MQTT.state());
            Serial.println(" Tentando novamente em 2s");
            delay(2000);
        }
    }
}

void publishSensorData() {
    // 1. Ler Luminosidade (LDR)
    int ldrRaw = analogRead(PINO_LDR);
    int luminosidade = map(ldrRaw, 0, 4095, 0, 100);

    // 2. Ler CO2 (Simulado pelo Potenciômetro)
    int potRaw = analogRead(PINO_POT_CO2);
    // Mapeia 0-4095 (analógico) para 400-2000 ppm (faixa comum de CO2)
    int co2 = map(potRaw, 0, 4095, 400, 2000);

    // 3. Ler Temperatura (DHT22)
    TempAndHumidity data = dht.getTempAndHumidity();
    float temperatura = data.temperature;

    // Validação de leitura do DHT
    if (isnan(temperatura)) {
        temperatura = 0.0; 
    }

    // 4. Montar Payload UltraLight 2.0
    String payload = "";
    payload += "l|" + String(luminosidade);
    payload += "|t|" + String(temperatura);
    payload += "|c|" + String(co2);

    Serial.print("Enviando dados: ");
    Serial.println(payload);

    MQTT.publish(TOPICO_PUBLISH, payload.c_str()); 
}

// ============================================================
//  SETUP E LOOP
// ============================================================

void setup() {
    pinMode(PINO_LED, OUTPUT);
    digitalWrite(PINO_LED, LOW); // Inicia desligado

    initSerial();
    initWiFi();
    
    // Inicializa DHT no pino definido
    dht.setup(PINO_DHT, DHTesp::DHT22);
    
    initMQTT();
}

void loop() {
    if (!MQTT.connected()) {
        reconnectMQTT();
    }
    
    MQTT.loop(); // Processa mensagens recebidas
    
    publishSensorData(); // Lê e envia dados
    
    delay(2000); // Intervalo de envio (2 segundos)
}
