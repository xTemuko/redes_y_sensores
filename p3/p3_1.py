import serial
import re
import time

PUERTO = 'COM6'           
BAUDIOS = 115200
ARCHIVO = 'p3\\datos_sensor.txt'


with open(ARCHIVO, 'w') as f:
    f.write("Tipo;X;Y;Z\n")  
    f.write("A;0.0;0.0;1.0\n")  
    f.write("G;0.1;0.1;0.1\n") 
    f.write("M;30.0;30.0;-10.0\n")  

print(f"[INFO] Archivo '{ARCHIVO}' creado con cabecera y datos de prueba.")


try:
    ser = serial.Serial(PUERTO, BAUDIOS, timeout=1)
    print(f"[INFO] Conectado a {PUERTO}. Esperando datos...")

    while True:
        linea = ser.readline().decode('utf-8', errors='ignore').strip()
        if linea.startswith("Enviado:"):
            print(f"[DEBUG] {linea}")
            match = re.match(r"Enviado: ([AGM])(-?\d+\.\d+),(-?\d+\.\d+),(-?\d+\.\d+)", linea)
            if match:
                tipo = match.group(1)
                x, y, z = match.group(2), match.group(3), match.group(4)
                fila = f"{tipo};{x};{y};{z}\n"
                with open(ARCHIVO, 'a') as f:
                    f.write(fila)
                print(f"[INFO] Guardado: {fila.strip()}")
            else:
                print("[WARN] Línea ignorada (no coincide con formato esperado)")

except Exception as e:
    print(f"[ERROR] No se pudo conectar al puerto serie: {e}")
