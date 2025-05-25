import serial
import time
import os # Para manejo de rutas y directorios

# --- Configuración ---
SERIAL_PORT = '/dev/ttyACM0'  # Para Linux. Podría ser /dev/ttyACM1, /dev/ttyUSB0, etc.
                              # Ejecuta `dmesg | grep tty` después de conectar el Arduino para verificar.
BAUD_RATE = 115200
FILE_DIRECTORY = 'p3'        # Directorio donde se guardará el archivo
FILE_NAME = 'datos_sensor.txt'
FULL_FILE_PATH = os.path.join(FILE_DIRECTORY, FILE_NAME)

# --- Asegurarse de que el directorio de datos exista ---
try:
    os.makedirs(FILE_DIRECTORY, exist_ok=True)
    print(f"[INFO] Directorio '{FILE_DIRECTORY}' listo.")
except OSError as e:
    print(f"[ERROR] No se pudo crear el directorio '{FILE_DIRECTORY}': {e}")
    exit() # Salir si no se puede crear el directorio

# --- Inicializar archivo de datos con cabecera ---
try:
    with open(FULL_FILE_PATH, 'w') as f:
        f.write("Tipo;X;Y;Z\n") # Usando punto y coma como en tu ejemplo original

    print(f"[INFO] Archivo '{FULL_FILE_PATH}' inicializado con cabecera.")
except IOError as e:
    print(f"[ERROR] No se pudo escribir la cabecera en '{FULL_FILE_PATH}': {e}")
    exit() # Salir si no se puede inicializar el archivo

# --- Bucle principal para leer del puerto serie y escribir en archivo ---
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
                print(f"[RAW] {linea_str}") # Mostrar la línea cruda para depuración
                
                partes = linea_str.split(',') # Separar por coma


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

                    except ValueError:
                        print(f"[WARN] Línea ignorada: no se pudieron convertir los valores a float. Línea: '{linea_str}'")
                    except IOError as e:
                        print(f"[ERROR] No se pudo escribir en el archivo '{FULL_FILE_PATH}': {e}")
                else:
                    if linea_str: 
                        print(f"[WARN] Línea ignorada: formato no reconocido. Línea: '{linea_str}'")


except serial.SerialException as e:
    print(f"[ERROR] No se pudo conectar o hubo un problema con el puerto serie {SERIAL_PORT}: {e}")
except KeyboardInterrupt:
    print("\n[INFO] Programa interrumpido por el usuario.")
except Exception as e:
    print(f"[ERROR] Ocurrió un error inesperado: {e}")
finally:
    if ser and ser.is_open:
        ser.close()
        print("[INFO] Puerto serie cerrado.")
    print("[INFO] Programa finalizado.")