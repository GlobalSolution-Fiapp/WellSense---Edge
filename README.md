# 📡 WellSense - Monitoramento Ambiental (ESP32)

> Firmware para dispositivo IoT baseado em ESP32 para monitorização de ambientes corporativos (Temperatura, Luminosidade e CO2) integrado na plataforma FIWARE via MQTT.

![ESP32](https://img.shields.io/badge/Hardware-ESP32-red)
![C++](https://img.shields.io/badge/Language-C%2B%2B-blue)
![MQTT](https://img.shields.io/badge/Protocol-MQTT%20%7C%20UltraLight%202.0-green)

## 📋 Visão Geral

Este projeto consiste no firmware para um nó de sensor que recolhe dados ambientais e os envia para um **IoT Agent (FIWARE)**. O dispositivo também é capaz de receber comandos remotos para atuação (ex: ligar/desligar um LED de estado).

### Funcionalidades
* **Leitura de Temperatura:** Utiliza sensor DHT22.
* **Leitura de Luminosidade:** Utiliza um LDR (Fotorresistência).
* **Simulação de CO2:** Utiliza um potenciómetro para simular níveis de qualidade do ar (400-2000 ppm).
* **Comunicação:** Envio de dados via Wi-Fi usando protocolo MQTT (Padrão UltraLight 2.0).
* **Atuação:** Controlo remoto de LED onboard via comandos MQTT.

---

## 🔌 Hardware Necessário

* 1x Placa de Desenvolvimento **ESP32**
* 1x Sensor de Temperatura e Humidade **DHT22**
* 1x Sensor de Luz **LDR** (Fotorresistência)
* 1x **Potenciómetro** (10kΩ)
* 1x Resistor de 10kΩ (Pull-down para o LDR)
* Jumpers e Protoboard

### Pinagem (Pinout)

| Componente | Pino do Componente | Pino do ESP32 |
| :--- | :--- | :--- |
| **DHT22** | Dados (SDA) | `GPIO 15` |
| **LDR** | Saída Analógica (AO) | `GPIO 34` |
| **Potenciómetro** | Sinal (SIG) | `GPIO 35` |
| **LED Onboard** | Anodo (+) | `GPIO 2` |

---

## 🛠️ Instalação e Configuração

### 1. Pré-requisitos (IDE)

Podes usar a **Arduino IDE** ou o **PlatformIO** (VS Code).
Certifica-te de instalar as seguintes bibliotecas:

1.  **PubSubClient** (por Nick O'Leary) - Para comunicação MQTT.
2.  **DHT sensor library for ESPx** (por beegee_tokyo) - Para leitura do DHT22.

### 2. Configuração do Código (`sketch.ino`)

Abre o ficheiro `sketch.ino` e edita as seguintes constantes no início do ficheiro para corresponder ao teu ambiente:

```cpp
// --- Wi-Fi e Broker ---
const char* SSID = "O_TEU_WIFI_AQUI";
const char* PASSWORD = "A_TUA_PASSWORD_AQUI";
const char* BROKER_MQTT = "IP_PUBLICO_DA_AWS"; // Ex: 3.15.20.123

// --- Identificação do Dispositivo ---
// Estes valores devem bater com o aprovisionamento no FIWARE
const char* ID_MQTT = "esp32_setor_nome";       // Ex: esp32_infra_gs2
const char* topicPrefix = "sensor_setor_001";   // Ex: sensor_infra_001

// --- Tópicos MQTT ---
const char* TOPICO_PUBLISH = "/TEF/sensor_setor_001/attrs";
const char* TOPICO_SUBSCRIBE = "/TEF/sensor_setor_001/cmd";
```
## 🚀 Como Executar (Simulação Wokwi)

Este projeto foi desenhado para ser compatível com o simulador online [Wokwi](https://wokwi.com/).

1.  Acede ao Wokwi e cria um novo projeto ESP32.
2.  Copia o conteúdo de `diagram.json` deste repositório para a aba `diagram.json` do Wokwi.
3.  Copia o conteúdo de `sketch.ino` para a aba de código.
4.  Adiciona as bibliotecas no `Library Manager` (aba lateral):
    * `PubSubClient`
    * `DHT sensor library for ESPx`
5.  Clica em **Start Simulation**.

---

## 📡 Protocolo de Comunicação (UltraLight 2.0)

O dispositivo envia os dados num formato de texto leve separado por pipes (`|`):

**Formato de Envio:**
```text
l|<valor_luz>|t|<valor_temp>|c|<valor_co2>
```
*Exemplo: l|85|t|24.50|c|500*

## Comandos Recebidos: O dispositivo escuta no tópico de comandos por mensagens no formato:

    sensor_id@on| -> Liga o LED.

    sensor_id@off| -> Desliga o LED.


### Integrantes:
Jessica Vitória Tavares – RM566220

Luara Freire Soares – RM561266

Miguel Amaro Dourado de Sousa – RM566200

### 📄 Licença

Desenvolvido para o projeto WorkWell - Global Solution.
