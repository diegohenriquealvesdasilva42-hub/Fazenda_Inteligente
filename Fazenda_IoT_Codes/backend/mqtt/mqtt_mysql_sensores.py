import paho.mqtt.client as mqtt
import mysql.connector
from datetime import datetime
import time
import uuid

INTERVALO_SAVE =  1

con = mysql.connector.connect(
    host='localhost',             #Mudar para o seu HOST
    database='IoT',
    user='root',
    password='diego11092002' 
)
cursor = con.cursor()

temp_estufa = None
umid_estufa = None
temp_externa = None
umid_externa = None
solo_estufa = None
solo_externo = None
nivel_poco = None
ar_raw = None
ar_qualidade = None
ar_status = None

ultimo_save = 0 

def todos_recebidos():
    return None not in (
        temp_estufa, umid_estufa, temp_externa, umid_externa,
        solo_estufa, solo_externo, nivel_poco,
        ar_raw, ar_qualidade, ar_status
    )

def salvar():
    sql = """INSERT INTO sensores 
        (temp_estufa, umid_estufa, temp_externa, umid_externa,
         solo_estufa, solo_externo, nivel_poco,
         ar_raw, ar_qualidade, ar_status, data_hora)
        VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)
    """

    dados = (
        temp_estufa, umid_estufa, temp_externa, umid_externa,
        solo_estufa, solo_externo, nivel_poco,
        ar_raw, ar_qualidade, ar_status,
        datetime.now()
    )

    cursor.execute(sql, dados)
    con.commit()
    print("✔ Registro salvo no BD:", dados)

def on_message(client, userdata, msg):
    global temp_estufa, umid_estufa, temp_externa, umid_externa
    global solo_estufa, solo_externo, nivel_poco
    global ar_raw, ar_qualidade, ar_status
    global ultimo_save

    valor = msg.payload.decode().strip()
    topico = msg.topic

    print(f"[MQTT] {topico} -> {valor}")

    if valor == "ERRO":
        return

    try:
        if topico == "fazenda/estufa/sensor/temperatura":
            temp_estufa = float(valor)

        elif topico == "fazenda/estufa/sensor/umidade":
            umid_estufa = float(valor)

        elif topico == "fazenda/externo/sensor/temperatura":
            temp_externa = float(valor)

        elif topico == "fazenda/externo/sensor/umidade":
            umid_externa = float(valor)

        elif topico == "fazenda/estufa/sensor/solo":
            solo_estufa = int(valor)

        elif topico == "fazenda/externo/sensor/solo":
            solo_externo = int(valor)

        elif topico == "fazenda/poco/sensor/nivel":
            nivel_poco = int(valor)

        elif topico == "fazenda/estufa/sensor/ar/raw":
            ar_raw = int(valor)

        elif topico == "fazenda/estufa/sensor/ar/qualidade":
            ar_qualidade = int(valor)

        elif topico == "fazenda/estufa/sensor/ar/status":
            ar_status = valor

    except ValueError:
        print("⚠ Valor inválido recebido:", valor)
        return

    agora = time.time()

    if todos_recebidos() and (agora - ultimo_save >= INTERVALO_SAVE):
        salvar()
        ultimo_save = agora

def on_connect(client, userdata, flags, rc):
    print("🔌 Conectado ao broker MQTT | Código:", rc)

    client.subscribe("fazenda/estufa/sensor/temperatura")
    client.subscribe("fazenda/estufa/sensor/umidade")
    client.subscribe("fazenda/externo/sensor/temperatura")
    client.subscribe("fazenda/externo/sensor/umidade")
    client.subscribe("fazenda/estufa/sensor/solo")
    client.subscribe("fazenda/externo/sensor/solo")
    client.subscribe("fazenda/poco/sensor/nivel")
    client.subscribe("fazenda/estufa/sensor/ar/raw")
    client.subscribe("fazenda/estufa/sensor/ar/qualidade")
    client.subscribe("fazenda/estufa/sensor/ar/status")

    print(" Tópicos inscritos")

def main():
    client = mqtt.Client(client_id=f"PythonFazenda-{uuid.uuid4()}")
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("test.mosquitto.org", 1883, 60)

    print("🚀 MQTT rodando | Salvando no banco a cada 1 seg ")
    client.loop_forever()

if __name__ == "__main__":
    main()
