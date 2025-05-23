#!/usr/bin/env python3
import socket
import re
import time
import os


HOST = '0.0.0.0'    
PORT = 6000         #
ARCHIVO = 'p5_6/datos_sensor.txt'

TIPO_SENSOR = 'A'

RE_FLOAT3 = re.compile(r'\s*(-?\d+\.\d+)\s*,\s*(-?\d+\.\d+)\s*,\s*(-?\d+\.\d+)\s*')

os.makedirs(os.path.dirname(ARCHIVO), exist_ok=True)


with open(ARCHIVO, 'w') as f:
    f.write("Tipo;X;Y;Z\n")
print(f"[INFO] Archivo '{ARCHIVO}' inicializado.")


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen(1)
    print(f"[INFO] Servidor TCP escuchando en {HOST}:{PORT} ...")

    while True:
        conn, addr = s.accept()
        print(f"[INFO] Conexión entrante desde {addr[0]}:{addr[1]}")
        with conn:

            fileobj = conn.makefile('r')
            while True:
                try:
                    linea = fileobj.readline()
                    if not linea:  # conexión cerrada
                        print("[INFO] Cliente desconectado.")
                        break
                    linea = linea.strip()
                    print(f"[DEBUG] Recibido: '{linea}'")
                    m = RE_FLOAT3.match(linea)
                    if m:
                        x, y, z = m.group(1), m.group(2), m.group(3)
                        fila = f"{TIPO_SENSOR};{x};{y};{z}\n"
                        with open(ARCHIVO, 'a') as f:
                            f.write(fila)
                        print(f"[INFO] Guardado: {fila.strip()}")
                    else:
                        print("[WARN] Línea ignorada (no coincide con formato float,float,float)")
                except Exception as e:
                    print(f"[ERROR] Lectura de socket falló: {e}")
                    break
        # Volver a esperar otra conexión
        print("[INFO] Esperando nueva conexión...")
