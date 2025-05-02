import serial

puerto = 'COM6'
velocidad = 115200

try:
    ser = serial.Serial(puerto, velocidad, timeout=1)
    print(f"Conectado a {puerto} a {velocidad} baudios.")
    
    while True:
        linea = ser.readline().decode('utf-8', errors='ignore').strip()
        if linea:
            print(linea)

except Exception as e:
    print(f"Error al conectar: {e}")
