import serial
import time
import os 


SERIAL_PORT = '/dev/ttyACM0'  
                             
BAUD_RATE = 115200
FILE_DIRECTORY = 'p3_log'        
FILE_NAME = 'datos_sensor.txt'
FULL_FILE_PATH = os.path.join(FILE_DIRECTORY, FILE_NAME)


try:
    os.makedirs(FILE_DIRECTORY, exist_ok=True)

except OSError as e:
    exit() 


try:
    with open(FULL_FILE_PATH, 'w') as f:
        f.write("Tipo;X;Y;Z\n") 
except IOError as e:
    exit() 


ser = None  
try:
    print(f"[INFO] Intentando conectar a {SERIAL_PORT} a {BAUD_RATE} baudios...")
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"[INFO] Conectado exitosamente a {SERIAL_PORT}.")

    print("[INFO] Esperando 2 segundos para inicialización del Arduino...")
    time.sleep(2)
    ser.flushInput() 
    print("[INFO] Comenzando lectura de datos...")

    while True:
        if ser.in_waiting > 0: 
            try:
                linea_bytes = ser.readline()
                linea_str = linea_bytes.decode('utf-8', errors='ignore').strip()
            except serial.SerialException as e:
                print(f"[ERROR] Error leyendo del puerto serie: {e}")
                break # 

            if linea_str: 
                print(f"[RAW] {linea_str}") 
                
                partes = linea_str.split(',') #


                # "A,0.01,0.98,-0.21"
                if len(partes) == 4 and partes[0] in ['A', 'G', 'M']:
                    tipo_sensor = partes[0]
                    try:

                        x = float(partes[1])
                        y = float(partes[2])
                        z = float(partes[3])
                        
                        linea_guardar = f"{tipo_sensor};{x:.4f};{y:.4f};{z:.4f}\n"
                        
                        with open(FULL_FILE_PATH, 'a') as f:
                            f.write(linea_guardar)
                        print(f"[SAVE] {linea_guardar.strip()}")

                    except Exception as e:
                        print(f"Error '{linea_str}'")

                else:
                   
                    print(f"formato no reconocido")


except KeyboardInterrupt:
    print("\n[INFO] Programa interrumpido")
except Exception as e:
    print(f"[ERROR] {e}")
finally:
    if ser and ser.is_open:
        ser.close()
        print("[INFO] Puerto serie cerrado.")
    print("[INFO] Programa finalizado.")