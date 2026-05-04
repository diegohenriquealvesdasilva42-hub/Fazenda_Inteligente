# 🌱 Fazenda Inteligente - Automação IoT Agrícola

Bem-vindo(a) ao repositório vitrine do **Projeto Fazenda IoT**. 
*Nota: O código-fonte deste projeto é mantido em um repositório privado. Este espaço destina-se a apresentar a arquitetura, as funcionalidades e os resultados alcançados com o desenvolvimento.*

## 📌 Sobre o Projeto
A agricultura moderna exige soluções tecnológicas para otimizar recursos e aumentar a produtividade. Este projeto consiste em um **sistema integrado de monitoramento e automação agrícola** capaz de gerenciar estufas e áreas externas.

A solução garante a eficiência no uso de recursos como água e energia elétrica, além de promover um ambiente ideal para o cultivo, reduzindo desperdícios através de intervenções autônomas baseadas em dados em tempo real.

## 🎯 Principais Funcionalidades
- **Monitoramento em Tempo Real:** Leitura constante de temperatura, umidade (ar e solo), qualidade do ar (MQ135), índice de chuvas e luminosidade.
- **Controle Duplo (Automático e Manual):** O sistema pode atuar sozinho baseado em limites pré-definidos (ex: ligar bomba d'água se o solo estiver seco) ou ser comandado manualmente pelo usuário.
- **Dashboard Web de Gestão:** Painel analítico construído com Flask para visualização de dados históricos e em tempo real, com lógica de cores para alertas críticos.
- **Aplicativo Mobile:** Integração com o aplicativo MQTT Panel para controle de atuadores e monitoramento direto do smartphone, de qualquer lugar.

## ⚙️ Arquitetura e Tecnologias
O projeto foi estruturado para ser robusto e expansível:

1. **Hardware (Edge):** Um microcontrolador **ESP32** (programado em C++) atua como cérebro local, lendo 7 tipos de sensores diferentes e controlando atuadores via relés e servo motores.
2. **Mensageria (Middleware):** A comunicação é feita via **Protocolo MQTT** utilizando o broker público Eclipse Mosquitto, garantindo baixa latência.
3. **Backend & Persistência:** Um script em **Python** assina os tópicos MQTT, processa os dados e realiza a inserção em um banco de dados relacional **MySQL**.
4. **Frontend:** Uma aplicação web servida por **Flask** (Python) + HTML/CSS apresenta o Dashboard para o usuário final.


## 👤 Autor

**Diego Henrique A. da Silva**  
Graduando em Engenharia de Computação e entusiasta de Internet das Coisas (IoT) e Arquitetura de Sistemas.

[![LinkedIn](www.linkedin.com/in/diegohenrique-dtech)
