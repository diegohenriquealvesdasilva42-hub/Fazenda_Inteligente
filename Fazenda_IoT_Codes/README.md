# Fazenda IoT

Projeto de monitoramento e automação agrícola utilizando **ESP32**, **MQTT**, **MySQL** , **Flask** e **MQTT Panel**.


O sistema coleta dados de sensores da estufa e do ambiente externo, armazena em banco de dados e exibe as informações em um dashboard web em tempo real e no Mqtt Panel.


## Tecnologias Utilizadas
- ESP32 (Arduino)
- MQTT (Broker Mosquitto)
- Python 3
- Flask
- MySQL
- HTML + CSS


## Estrutura do Projeto

Fazenda-IoT/
├── backend/
│   ├── mqtt/        # Recebe dados MQTT e salva no banco
│   └── web/         # Dashboard Flask
├── database/        # Script do banco de dados
├── firmware/        # Código do ESP32


## MQTT Panel (Monitoramento e Controle)

Além do dashboard web, o projeto também utiliza o aplicativo **MQTT Panel** para:

- Visualizar dados dos sensores em tempo real
- Controlar atuadores da fazenda
- Alternar entre modo Automático e Manual


## Broker MQTT
- Broker: `test.mosquitto.org`
- Porta: `1883`


## Tópicos de Controle (MQTT Panel → ESP32)

| Função | Tópico | Payload |
|------|------|--------|
| Modo do sistema | `fazenda/sistema/modo` | `Auto` / `Manual` |
| Bomba de água | `fazenda/geral/bomba` | `Liga` / `Desliga` |
| Ventoinha estufa | `fazenda/estufa/ventoinha` | `Liga` / `Desliga` |
| Luz da estufa | `fazenda/estufa/luz` | `Liga` / `Desliga` |
| Poste externo | `fazenda/geral/poste` | `Liga` / `Desliga` |


## Tópicos de Monitoramento (ESP32 → MQTT Panel)

| Sensor | Tópico |
|------|------|
| Temperatura Estufa | `fazenda/estufa/sensor/temperatura` |
| Umidade Estufa | `fazenda/estufa/sensor/umidade` |
| Temperatura Externa | `fazenda/externo/sensor/temperatura` |
| Umidade Externa | `fazenda/externo/sensor/umidade` |
| Umidade Solo Estufa | `fazenda/estufa/sensor/solo` |
| Umidade Solo Externo | `fazenda/externo/sensor/solo` |
| Nível do Poço | `fazenda/poco/sensor/nivel` |
| Qualidade do Ar | `fazenda/estufa/sensor/ar/status` |


## Estados Publicados pelo Sistema
- `fazenda/sistema/modo/estado`
- `fazenda/geral/bomba/estado`
- `fazenda/estufa/ventoinha/estado`
- `fazenda/estufa/luz/estado`
- `fazenda/geral/poste/estado`

📌 O MQTT Panel permite o controle manual completo do sistema sem necessidade de acessar o servidor web.

## Execução 
1️⃣ Banco de Dados
sql: source database/fazenda_bd.sql;
2️⃣ Backend MQTT
prompt:cd backend/mqtt
       python mqtt_mysql_sensores.py
3️⃣ Dashboard Web: cd backend/web
                   python dashboard.py
4️⃣ Abrir o Dashboard: http://localhost:5000

           
                   

