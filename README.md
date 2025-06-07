# 🌊 Projeto de Monitoramento e Alerta de Enchentes com ESP32 e ThingSpeak

Este projeto tem como objetivo **monitorar condições ambientais** (nível da água, umidade e temperatura) utilizando um **ESP32 com sensores**, e **emitir alertas automáticos via sirene (LED)** sempre que o sistema detectar risco de enchente. Os dados são enviados para o serviço **ThingSpeak**, e os comandos podem ser controlados remotamente pela nuvem.

---

## 📸 Visão Geral do Circuito (Wokwi)

![image](https://github.com/user-attachments/assets/42708bc4-a246-4860-ba17-d83c15e812c2)

### Led desligado sem risco
![image](https://github.com/user-attachments/assets/96cc9ce9-027e-4f20-a788-2c5141e75867)

### Led ligado com risco
![image](https://github.com/user-attachments/assets/8562aee2-f55e-4cb9-baec-fcf9c3c2cc95)




---

## 🧰 Componentes Utilizados

| Componente         | Função                                       |
|--------------------|----------------------------------------------|
| ESP32              | Microcontrolador com Wi-Fi integrado         |
| Sensor DHT22       | Mede temperatura e umidade                   |
| Sensor HC-SR04     | Mede distância (nível da água)               |
| LED vermelho       | Representa a sirene (atuador de alerta)      |
| Resistor 220Ω      | Protege o LED contra corrente excessiva      |
| ThingSpeak         | Plataforma IoT para visualização e controle  |

---

## 🔁 Fluxo do Sistema


- A[Leitura do DHT22 e HC-SR04] --> B[Verifica condições críticas: \n distância < 10cm e umidade > 80%]
- B -->|Sim| C[Ativa alerta (LED/Sirene)]
- C --> D[Envia dados para o ThingSpeak: Fields 1 a 4]
- B -->|Não| E[Desativa alerta]
- E --> D
- D --> F[Lê remotamente Field 4 para manter controle sincronizado]
- F --> G[Atualiza estado do LED localmente]

## 🔧 Configuração no ThingSpeak

Crie um novo Canal Público com os seguintes campos:

- Field 1: Temperatura

- Field 2: Umidade

- Field 3: Nível da Água

- Field 4: Comando da Sirene

Obtenha o:
- channelID
- Write API Key
- Read API Key

Atualize os valores no código:

```bash
unsigned long channelID = 2979598;
const char* writeAPIKey = "CCDYC36P8A6NT8CH";
const char* readAPIKey = "6C87V78PUWOZN1PF";
```
---

## ⚙️ Como Executar o Projeto

### ✅ Simulação no Wokwi
- Acesse https://wokwi.com
- Importe o projeto com o código e circuito acima
- Copie o Diagram.json e coloque no Diagram.json do wokwi
- Copie o sketch.ino e coloque no sketch.ino do wokwi
- Clique em "Start Simulation"
- Acompanhe o monitor serial para ver os dados sendo enviados e o alerta acionado

---

### 🔬 Testes e Validação
✔️ Teste 1 — Ambiente Normal
Distância > 20 cm
Umidade < 60%

✅ LED permanece desligado

✔️ Teste 2 — Situação de Alerta
Distância < 10 cm

Umidade > 80%

✅ LED acende (sirene ligada)

✅ Field 4 = 1 enviado ao ThingSpeak

---

## 🧠 Explicação Técnica
Sensor HC-SR04 mede a distância até a água. Abaixo de 10 cm é considerado risco de alagamento.

Sensor DHT22 mede umidade e temperatura. Alta umidade + proximidade da água = alerta.

ThingSpeak armazena os dados para visualização em gráficos e permite controle remoto da sirene.

A sirene é um LED vermelho, mas pode ser substituída por um buzzer físico.

---

## 💡 Possíveis Melhorias Futuras
Substituir o LED por um buzzer real ou alarme sonoro

Enviar notificações SMS ou email via IFTTT

Adicionar Node-RED para automação mais visual

Integrar com sensores de chuva ou fluxo
