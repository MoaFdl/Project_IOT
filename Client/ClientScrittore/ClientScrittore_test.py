import socket
import struct
import threading
import time
import json
import datetime

id_client= "7"
nome_client="cliente test5"
stato_cli= "true"


def client_thread(choice):
    try:
        # Crea il socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connessione al server sulla stessa macchina, porta 8989
            s.connect(('localhost', 8989))
            print("Connection established")

            # Impacchetta l'intero come 4 byte (formato nativo, come in C)
            data = struct.pack('i', choice)  # 'i' = 4-byte signed int
            s.sendall(data)
            json_files=stampaDati()
            s.sendall(bytes(json_files,encoding="utf-8"))

    except Exception as e:
        print(f"Error: {e}")

def stampaDati():
    nota_dato="0"
    unita_misura="1"
    tipo_sensore = "0" 
    descrizione_ard="0"
    misurazione_ard=["1","2"]
    timestamp_dato= datetime.datetime.now()

    StringaDict = {"id": id_client,
                    "nome":nome_client,
                    "stato":stato_cli,
                    "misura":unita_misura,
                    "tipo":tipo_sensore,
                    "descrizione":descrizione_ard,
                    "dato":misurazione_ard,
                    "nota": nota_dato,
                    "timestamp": timestamp_dato.isoformat()}
    StringaDati_json= json.dumps(StringaDict)
    return StringaDati_json


print("1. Read")
print("2. Write")

try:
    choice = int(input())
except ValueError:
    print("Invalid Input")
    

if choice not in (1, 2):
    print("Invalid Input")
    

# Crea e avvia il thread (come nel C)
thread = threading.Thread(target=client_thread, args=(choice,))
thread.start()

# Simula il sleep(20) del C (non necessario, ma per fedeltà)
time.sleep(20)

# Attende la fine del thread
thread.join()

