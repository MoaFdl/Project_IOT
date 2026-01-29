# Source - https://stackoverflow.com/a
# Posted by Ali Hussein, modified by community. See post 'Timeline' for change history
# Retrieved 2026-01-06, License - CC BY-SA 4.0

import serial
import json
import datetime
import socket
import struct
import threading
import time

id_client= "1"
nome_client="cliente test"
stato_cli= "true"


def client_thread(json_files):
    try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as socket_connesion:
            # Connessione al server sulla stessa macchina, porta 8989
                socket_connesion.connect(('localhost', 8989))
                print("Connection established")
                print(json_files)
                numero=2
                data = struct.pack('i',numero)  # 'i' = 4-byte signed int
                socket_connesion.sendall(data)
                socket_connesion.sendall(bytes(json_files,encoding="utf-8"))

    except Exception as e:
        print(f"Error: {e}")


def stampaDati(Stringa):
    righe=Stringa.splitlines()
    
    nota_dato="xx"
    unita_misura=righe[0]
    tipo_sensore = righe[1] 
    descrizione_ard=righe[2]
    misurazione_ard=righe[3:-1]
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




StringaDati=""
Porta="COM3"
baudrate=115200
"""
try:
            
        # Crea il socket
            socket_connesion=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # Connessione al server sulla stessa macchina, porta 8989
            socket_connesion.connect(('localhost', 8989))
            print("Connection established")


except Exception as e:
        print(f"Error: {e}")
"""

serialPort = serial.Serial(Porta, baudrate ,bytesize=8 ,timeout=None ,stopbits=serial.STOPBITS_ONE )
while 1:
    # Read data out of the buffer until a carraige return / new line is found
    serialString = serialPort.readline()
    # Print the contents of the serial data
    try:
       
        StringaDati=  StringaDati +serialString.decode("Ascii")

        if serialString.decode("Ascii").strip() == "Ciclo terminato. Premi il bottone per riavviare.":
           json_file=stampaDati(StringaDati)
           thread = threading.Thread(target=client_thread,args=(json_file,))
           thread.start()
           thread.join()
           StringaDati=""
           
    except Exception as e:
        print(f"Errore durante l'elaborazione: {e}")

    except serial.SerialException:
     print(f"Errore: Impossibile aprire la porta {Porta}")
    except KeyboardInterrupt:
        print("\nProgramma interrotto dall'utente.")
